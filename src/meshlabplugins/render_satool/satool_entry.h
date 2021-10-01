#ifndef SATOOL_ENTRY_H
#define SATOOL_ENTRY_H

#include <QObject>
#include <common/plugins/interfaces/render_plugin.h>
#include "saDialog.h"


class SAToolRenderPlugin : public QObject, public RenderPlugin {
    Q_OBJECT
    MESHLAB_PLUGIN_IID_EXPORTER(RENDER_PLUGIN_IID)
    Q_INTERFACES(RenderPlugin)
    
public:
    SAToolRenderPlugin();
    virtual ~SAToolRenderPlugin(){}
	QString pluginName() const;
	
    bool isSupported() { return true; }
    QList<QAction *> actions();

    void init(QAction *, MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& /*mp*/, GLArea *);
    void render(QAction *, MeshDocument &, MLSceneGLSharedDataContext::PerMeshRenderingDataMap& mp, GLArea *);
    void finalize(QAction *, MeshDocument *, GLArea *);
    
    void setMainWindow(void* mainWindow);
    
private:
    SADialog* saDialog;
    QList <QAction *> actionList;
    void* mainWindow;
};

#endif
