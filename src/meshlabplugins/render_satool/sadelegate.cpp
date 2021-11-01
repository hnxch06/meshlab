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
        
//        if (v.vcg::vertex::EmptyCore<CUsedTypesO>::HasTexCoord())
//        {
//            //        v.T().U();
//            //        v.T().V();
//        }
        
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

MeshModel* SADataUtil::addMeshToDoc(void* meshDocumentPtr, sat::Model* model)
{
    MeshDocument* meshDocument = (MeshDocument*)meshDocumentPtr;
    MeshDocument& md = *meshDocument;
    
    CMeshO omesh;
    omesh.Clear();
    
    const sat::Mesh& fm = model->getFlattenMesh();
    int vertexCount = fm.verts.size();
    omesh.vert.reserve(vertexCount);
    int triangleCount = 0;
    for (int i = 0, isize = fm.faces.size(); i < isize; i++)
    {
        triangleCount += (fm.faces[i].count - 2);
    }
    omesh.face.reserve(triangleCount);
    
    vcg::tri::Allocator<CMeshO>::AddVertices(omesh, vertexCount);
    for (int i = 0; i < vertexCount; i++)
    {
        CVertexO& wv = omesh.vert[i];
        const sat::Vertex& rv = fm.verts[i];
        wv.P() = CMeshO::CoordType(rv.pos(0), rv.pos(1), rv.pos(2));
        // aka vcg::Point3<float>;

        if (fm.withColor)
        {
            wv.C() = vcg::Point4<unsigned char>(rv.color(0), rv.color(1), rv.color(2), rv.color(3));
        }
    }
    
    for (int i = 0, isize = fm.faces.size(); i < isize; i++)
    {
        const sat::Face& rf = fm.faces[i];
        for (int j = 2; j < rf.count; j++)
        {
            CVertexO* v0 = &(omesh.vert[rf.idx[0]]);
            CVertexO* v1 = &(omesh.vert[rf.idx[j - 1]]);
            CVertexO* v2 = &(omesh.vert[rf.idx[j]]);
            vcg::tri::Allocator<CMeshO>::AddFace(omesh, v0, v1, v2);
        }
    }
    
    
//    for (int i = 0; i < vertexCount; i++)
//    {
//        CVertexO wv;
//        const sat::Vertex& rv = fm.verts[i];
//        wv.P() = CMeshO::CoordType(rv.pos(0), rv.pos(1), rv.pos(2));
//
//        if (fm.withColor)
//        {
//            wv.C() = vcg::Point4<unsigned char>(rv.color(0), rv.color(1), rv.color(2), rv.color(3));
//        }
//        omesh.vert.push_back(wv);
//    }
    
//    for (int i = 0, isize = fm.faces.size(); i < isize; i++)
//    {
//        const sat::Face& rf = fm.faces[i];
//        for (int j = 2; j < rf.count; j++)
//        {
//            CFaceO wf;
////            wf.V(0) = rf.idx[0];
////            wf.V(1) = rf.idx[j - 1];
////            wf.V(2) = rf.idx[j];
//            omesh.face.push_back(wf);
//        }
//    }
    return md.addNewMesh(omesh, "sa_gen_mesh", false);
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
