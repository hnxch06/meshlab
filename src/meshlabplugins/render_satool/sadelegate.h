#ifndef SATOOL_DELEGATE_H
#define SATOOL_DELEGATE_H

#include <map>
#include <common/ml_document/mesh_model.h>
#include <QLabel>
#include <QDockWidget>
#include <QMap>
#include <QLineEdit>
#include <QSlider>
#include <QSignalMapper>

#include "Model/Geometory.h"
//#include "display/DisplayManager.h"
//class GLArea;

class SAUtil
{
public:
    static sat::Model* convertMeshFromMeshlabToSAGeo(MeshModel* meshModel);
};

#endif
