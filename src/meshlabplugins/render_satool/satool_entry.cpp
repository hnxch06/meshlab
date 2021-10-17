#include <common/GLExtensionsManager.h>
#include <common/globals.h>
#include <common/mlapplication.h>
#include <math.h>
#include <stdlib.h>
#include <vcg/space/colorspace.h>
#include "satool_entry.h"
#include <vcg/complex/algorithms/clean.h>
#include <vcg/complex/algorithms/stat.h>
#include <vcg/complex/algorithms/point_outlier.h>
#include <QGLWidget>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>

#include <QCoreApplication>
#include "../../meshlab/mainwindow.h"
#include "../../meshlab/glarea.h"

#include "display/DisplayManager.h"

using namespace vcg;

// ERROR CHECKING UTILITY
#define CheckError(x,y); if ((x)) {this->errorMessage = (y); return false;}
///////////////////////////////////////////////////////

SAToolRenderPlugin::SAToolRenderPlugin():saDialog(NULL), mainWindow(NULL)
{
    QAction* qa = new QAction(QIcon(":images/sel_satool.png"), tr("SATool"), this);
    qa->setCheckable(false);
    qa->setPriority(QAction::HighPriority);
    actionList << qa;
}

QString SAToolRenderPlugin::pluginName() const
{
    return "SARenderTool";
}

QList<QAction *> SAToolRenderPlugin::actions()
{
    return actionList;
}

void SAToolRenderPlugin::setMainWindow(void* mainWindow)
{
    this->mainWindow = mainWindow;
}

void SAToolRenderPlugin::setFrameDocument(MeshDocument &md, MLSceneGLSharedDataContext::PerMeshRenderingDataMap&mp, GLArea *gla)
{
    for(const MeshModel& mp : md.meshIterator()) {
        if (gla->meshVisibilityMap[mp.id()])
        {
            sat::DisplayManager::getInstance()->setDisplayModel((void*)(&mp));
            break;
        }
    }
}

bool SAToolRenderPlugin::hasCustomRenderContent()
{
    std::shared_ptr<sat::WorkFlow> workFlow = sat::DisplayManager::getInstance()->getDisplayingWorkFlow();
    if (workFlow == nullptr) return false;
    return true;
}

void SAToolRenderPlugin::init(QAction *a, MeshDocument &md, MLSceneGLSharedDataContext::PerMeshRenderingDataMap&mp, GLArea *gla) {
    if (saDialog) {
        saDialog->close();
        delete saDialog;
        saDialog = nullptr;
    }
    
    for(const MeshModel& mp : md.meshIterator()) {
        if (gla->meshVisibilityMap[mp.id()])
        {
            sat::DisplayManager::getInstance()->setDisplayModel((void*)(&mp));
            break;
        }
    }
    
    if (this->mainWindow != nullptr)
    {
        MainWindow* mainWin = (MainWindow*)(this->mainWindow);
        saDialog = new SADialog(gla, mainWin);
        saDialog->setAllowedAreas(Qt::LeftDockWidgetArea);
        saDialog->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
        mainWin->addDockWidget(Qt::LeftDockWidgetArea, saDialog);
    } else
    {
        saDialog = new SADialog(gla, gla);
        saDialog->move(10, 100);
        saDialog->show();
    }
}
void SAToolRenderPlugin::render(QAction *a, MeshDocument &md, MLSceneGLSharedDataContext::PerMeshRenderingDataMap&mp, GLArea *gla)
{
}
void SAToolRenderPlugin::finalize(QAction * a, MeshDocument *md, GLArea *gla)
{
    if (saDialog != nullptr)
    {
        delete saDialog;
        saDialog = nullptr;
    }
}
MESHLAB_PLUGIN_NAME_EXPORTER(SAToolRenderPlugin)
