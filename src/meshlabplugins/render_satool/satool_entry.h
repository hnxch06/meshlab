#ifndef SATOOL_ENTRY_H
#define SATOOL_ENTRY_H

#include <QObject>
#include <common/plugins/interfaces/render_plugin.h>
#include "saDialog.h"


class SAToolRenderPlugin : public QObject, public RenderPlugin, virtual public MeshLabPluginLogger {
    Q_OBJECT
    MESHLAB_PLUGIN_IID_EXPORTER(RENDER_PLUGIN_IID)
    Q_INTERFACES(RenderPlugin)
    
public:
    SAToolRenderPlugin();
    virtual ~SAToolRenderPlugin(){}
    
    void timerEvent(QTimerEvent *);
    
	QString pluginName() const;
	
    bool isSupported() { return true; }
    QList<QAction *> actions();

    void init(QAction *, MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& /*mp*/, GLArea *);
    void render(QAction *, MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& mp, GLArea *);
    void finalize(QAction *, MeshDocument *, GLArea *);
    
    void setMainWindow(void* mainWindow);
    
    void setFrameDocument( MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& mp, GLArea *);
    bool hasCustomRenderContent();
    unsigned int hasSpecifyRenderModel(MeshDocument& md, GLArea* glarea);
    void logInConsole(const char* msg);
    
private:
    SADialog* saDialog;
    QList <QAction *> actionList;
    void* mainWindow;
    
    int mInitTimer;
};

#endif
