#include "sadelegate.h"
#include "workflow/WorkFlowSharedData.h"
#include "../../meshlab/glarea.h"
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



/*  /////////////////////////////////////
 *  Custom Render Functions.......
 *  /////////////////////////////////////
 */

typedef struct SAFrameRenderState
{
    sat::DisplayFrame* frame = nullptr;
    bool inited = false;
    
    GLuint vbo_count = 0;
    GLuint vbos[128];
} SAFrameRenderState;

void SAFrameRenderStateInit(SAFrameRenderState* renderState)
{
    renderState->vbo_count = std::min(renderState->frame->model->getMeshCount(), 128);
    glGenBuffers(renderState->vbo_count, renderState->vbos);
    
    for (int i = 0; i < renderState->vbo_count; i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, renderState->vbos[i]);
        
        sat::Mesh* mesh = renderState->frame->model->getMesh(i);
        int bufferSize = mesh->verts.size() * sizeof(sat::Vertex);
        glBufferData(GL_ARRAY_BUFFER, bufferSize, &(mesh->verts[0]), GL_STATIC_DRAW);
    }
}

void SAFrameRenderStateRelease(void* userData)
{
    SAFrameRenderState* renderState = (SAFrameRenderState*)userData;
    
    if (renderState->vbo_count > 0)
    {
        glDeleteBuffers(renderState->vbo_count, renderState->vbos);
        renderState->vbo_count = 0;
    }
}

void SARenderUtil::render(GLArea *gla, sat::DisplayFrame* frame, const std::vector<GLuint>& texIds)
{
    MLSceneGLSharedDataContext* shared = gla->mvc()->sharedDataContext();
    
    if (frame->userData == nullptr)
    {
        SAFrameRenderState* state = new SAFrameRenderState();
        state->frame = frame;
        frame->userData = state;
        frame->userDataReleaseFn = SAFrameRenderStateRelease;
    }
    
    SAFrameRenderState* renderState = (SAFrameRenderState*)frame->userData;
    if (!renderState->inited)
    {
        renderState->inited = true;
        SAFrameRenderStateInit(renderState);
    }
    
    sat::GLProgram* program = sat::GLDefaultProgram::getInstance()->getProgram(sat::GLProgramReservedName::SIMPLE_POSITION_COLOR);
    
    if (program != NULL)
    {
        program->useProgram();
    }
    
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
//    glMultMatrix(_tr);
    
    glPopMatrix();
    glPopAttrib();
    glFlush();
    glFinish();
}
