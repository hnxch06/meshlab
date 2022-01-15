/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2009                                          \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#ifndef SADIALOG_H
#define SADIALOG_H

#include <map>
#include <GL/glew.h>
#include <common/ml_document/mesh_model.h>
#include <QLabel>
#include <QDockWidget>
#include "ui_saDialog.h"
#include <QMap>
#include <QLineEdit>
#include <QSlider>
#include <QSignalMapper>

#include "display/DisplayManager.h"

class QGLWidget;
class QTreeWidget;

class WorkFlowWidgetItem : public QTreeWidgetItem
{
public:
    WorkFlowWidgetItem(void* factory);
    ~WorkFlowWidgetItem();
    
public:
    void* factory;
};

class JobWidgetItem : public QTreeWidgetItem
{
public:
    JobWidgetItem(void* job);
    ~JobWidgetItem();
    
public:
    void* job;
};

class SADialog : public QDockWidget
{
    Q_OBJECT

public:
    SADialog(QGLWidget* gla, QWidget *parent = 0);
	~SADialog();
    
    void initSupportWorkFlow();
    
    void timerEvent(QTimerEvent *);
    
public:
    void workFlowChangedListener(sat::WorkFlow* from, sat::WorkFlow* to, bool refreshUi);

private:
	QGLWidget* glarea;
	Ui::SADialogClass* ui;
    int mFrameTimeID;
    
private:
    void updateTreeWidgetSizes(void* tree);

private slots:
    void workFlowClicked(QTreeWidgetItem * , int );
    void adaptLayout(QTreeWidgetItem * item);
    void frameExecuteStateChange(int state);
    void pauseFrame();
    void debugClicked();
    void jobDetailClicked(QTreeWidgetItem * , int );
    
};

#endif
