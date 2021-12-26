#include "sadelegate.h"
#include "workflow/WorkFlowSharedData.h"
#include "../../meshlab/glarea.h"
#include "../../common/ml_document/mesh_document.h"
#include "../../vcglib/vcg/complex/allocate.h"
#include "display/render/GLRender.h"

sat::Model* SADataUtil::convertMeshFromMeshlabToSAGeo(MeshModel* meshModel)
{
    sat::Model* model = new sat::Model();
    
    sat::Mesh mesh;
    mesh.withColor = false;
    mesh.name = meshModel->shortName().toStdString();
    
    int vSize = meshModel->cm.VertexNumber();
    mesh.verts.reserve(vSize);
    
    for (int i = 0; i < vSize; i++)
    {
        auto& v = meshModel->cm.vert[i];
        sat::Vertex satV;
        satV.pos(0) = v.P().X();
        satV.pos(1) = v.P().Y();
        satV.pos(2) = v.P().Z();
        
        if (v.vcg::vertex::EmptyCore<CUsedTypesO>::HasColor())
        {
            satV.color(0) = v.C().X();
            satV.color(1) = v.C().Y();
            satV.color(2) = v.C().Z();
            satV.color(3) = v.C().W();
        } else
        {
            satV.color(0) = satV.color(1) = satV.color(2) = satV.color(3) = 1.0f;
        }
        if (v.vcg::vertex::EmptyCore<CUsedTypesO>::HasTexCoord())
        {
            satV.uv(0) = v.T().U();
            satV.uv(1) = v.T().V();
        } else
        {
            satV.uv(0) = satV.uv(1) = 0.0f;
        }
        
        mesh.verts.push_back(satV);
    }
    
    int fSize = meshModel->cm.SimplexNumber();
    mesh.faces.reserve(fSize);
    
    for (int i = 0; i < fSize; i++)
    {
        auto& f = meshModel->cm.face[i];
        sat::Face satF;
        satF.count = 3;
        satF.idx[0] = f.V0(0)->Index();
        satF.idx[1] = f.V0(1)->Index();
        satF.idx[2] = f.V0(2)->Index();
        mesh.faces.push_back(satF);
    }
    
    model->addMesh(mesh);
    model->lockMesh();
    
    return model;
}

MeshModel* SADataUtil::addMeshToDoc(void* meshDocumentPtr, void* glArea, sat::Model* model, const char* name)
{
    const sat::Mesh& fm = model->getFlattenMesh();
    return SADataUtil::addMeshToDoc(meshDocumentPtr, glArea, &fm, name);
}

MeshModel* SADataUtil::addMeshToDoc(void* meshDocumentPtr, void* glAreaV, const sat::Mesh* mesh, const char* name)
{
    GLArea* glArea = (GLArea*)glAreaV;
    MeshDocument* meshDocument = (MeshDocument*)meshDocumentPtr;
    MeshDocument& md = *meshDocument;
    
    CMeshO omesh;
    omesh.Clear();
    
    int mask = 0;
    mask |= vcg::tri::io::Mask::IOM_VERTQUALITY;
    mask |= vcg::tri::io::Mask::IOM_FACEQUALITY;
    if (mesh->withColor || (mesh->syncDisplayMesh != nullptr && mesh->syncDisplayMesh->withColor))
    {
        mask |= vcg::tri::io::Mask::IOM_VERTCOLOR;
    }
    
    int vertexCount = mesh->verts.size();
    int syncDisplayVertexCount = mesh->syncDisplayMesh != nullptr ? mesh->syncDisplayMesh->verts.size() : 0;
    omesh.vert.reserve(vertexCount  + syncDisplayVertexCount);
    int triangleCount = 0;
    int syncDisplayTriangleCount = mesh->syncDisplayMesh != nullptr ? mesh->syncDisplayMesh->faces.size() : 0;
    for (int i = 0, isize = mesh->faces.size(); i < isize; i++)
    {
        triangleCount += (mesh->faces[i].count - 2);
    }
    omesh.face.reserve(triangleCount + syncDisplayTriangleCount);
    
    vcg::tri::Allocator<CMeshO>::AddVertices(omesh, vertexCount + syncDisplayVertexCount);
    for (int i = 0; i < vertexCount; i++)
    {
        CVertexO& wv = omesh.vert[i];
        const sat::Vertex& rv = mesh->verts[i];
        wv.P() = CMeshO::CoordType(rv.pos(0), rv.pos(1), rv.pos(2));
        // aka vcg::Point3<float>;

        if (mesh->withColor)
        {
            wv.C() = vcg::Point4<unsigned char>(rv.color(0) * 255, rv.color(1) * 255, rv.color(2) * 255, rv.color(3) * 255);
        } else
        {
            wv.C() = vcg::Point4<unsigned char>(255, 255, 255, 255);
        }
    }
    
    for (int i = 0; i < syncDisplayVertexCount; i++)
    {
        CVertexO& wv = omesh.vert[i + vertexCount];
        const sat::Vertex& rv = mesh->syncDisplayMesh->verts[i];
        wv.P() = CMeshO::CoordType(rv.pos(0), rv.pos(1), rv.pos(2));
        // aka vcg::Point3<float>;

        if (mesh->syncDisplayMesh->withColor)
        {
            wv.C() = vcg::Point4<unsigned char>(rv.color(0) * 255, rv.color(1) * 255, rv.color(2) * 255, rv.color(3) * 255);
        } else
        {
            wv.C() = vcg::Point4<unsigned char>(255, 255, 255, 255);
        }
    }
    
    for (int i = 0, isize = mesh->faces.size(); i < isize; i++)
    {
        const sat::Face& rf = mesh->faces[i];
        for (int j = 2; j < rf.count; j++)
        {
            CVertexO* v0 = &(omesh.vert[rf.idx[0]]);
            CVertexO* v1 = &(omesh.vert[rf.idx[j - 1]]);
            CVertexO* v2 = &(omesh.vert[rf.idx[j]]);
            vcg::tri::Allocator<CMeshO>::AddFace(omesh, v0, v1, v2);
        }
    }
    
    for (int i = 0; i < syncDisplayTriangleCount; i++)
    {
        const sat::Face& f = mesh->syncDisplayMesh->faces[i];
        CVertexO* v0 = &(omesh.vert[vertexCount + f.idx[0]]);
        CVertexO* v1 = &(omesh.vert[vertexCount + f.idx[1]]);
        CVertexO* v2 = &(omesh.vert[vertexCount + f.idx[2]]);
        vcg::tri::Allocator<CMeshO>::AddFace(omesh, v0, v1, v2);
    }
    
    MeshModel* newMM = md.addNewMesh(omesh, name, false);
    newMM->enable(mask);
    
    if (md.mm() != NULL)
    {
        MLSceneGLSharedDataContext* datacont = glArea->mvc()->sharedDataContext();
        MLRenderingData copyFromDt;
        datacont->getRenderInfoPerMeshView(md.mm()->id(), glArea->context(), copyFromDt);
        datacont->setRenderingDataPerMeshView(newMM->id(), glArea->context(), copyFromDt);
    }
    return newMM;
}

void SADataUtil::removeMeshFromDoc(void* meshDocumentPtr, MeshModel* mm)
{
    MeshDocument* meshDocument = (MeshDocument*)meshDocumentPtr;
    MeshDocument& md = *meshDocument;
    md.delMesh(mm->id());
}



/*  /////////////////////////////////////
 *  Custom Render Functions.......
 *  /////////////////////////////////////
 */
void SARenderUtil::render(GLArea *gla, sat::DisplayFrame* frame, const std::vector<GLuint>& texIds)
{
    MLSceneGLSharedDataContext* shared = gla->mvc()->sharedDataContext();

    sat::GLFrameStaticRenderState* renderState = sat::GLRender::getInstance()->prepareStaticRenderState(frame);
    
//    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
//    glMultMatrix(_tr);
    
    renderState->staticDraw();
    
    glPopMatrix();
//    glPopAttrib();
    glFlush();
    glFinish();
}
