#include "sadelegate.h"
#include "../../meshlab/glarea.h"

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



/*  /////////////////////////////////////
 *  Custom Render Functions.......
 *  /////////////////////////////////////
 */

void SARenderUtil::render(GLArea *gla, sat::Model* model, const std::vector<GLuint>& texIds)
{
    MLSceneGLSharedDataContext* shared = gla->mvc()->sharedDataContext();
    
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
//    glMultMatrix(_tr);
    
    glPopMatrix();
    glPopAttrib();
    glFlush();
    glFinish();
}
