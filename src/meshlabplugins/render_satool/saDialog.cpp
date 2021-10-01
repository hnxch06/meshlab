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

#include "saDialog.h"
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QColor>
#include <QLineEdit>
#include <QColorDialog>
#include <QGLWidget>
#include <QCheckBox>
#include <QPushButton>
#include "MeshLabWorkFlow.h"

#define DECFACTOR 100000.0f

using namespace vcg;

SADialog::SADialog(QGLWidget* gla, QWidget *parent)
: QDockWidget(parent), ui(NULL)
{
    ui = new Ui::SADialogClass();
    
    setWindowFlags( windowFlags() | Qt::WindowStaysOnTopHint | Qt::SubWindow );
    this->QWidget::setAttribute( Qt::WA_MacAlwaysShowToolWindow);
    setVisible(true);
    
	ui->setupUi(this);
	this->setFeatures(QDockWidget::AllDockWidgetFeatures);
	this->setAllowedAreas(Qt::LeftDockWidgetArea);

	glarea = gla;
    
    this->initSupportWorkFlow();
    
    // The following connection is used to associate the click with the change of the current mesh.
    connect(ui->workFlowTreeWidget, SIGNAL(itemPressed(QTreeWidgetItem * , int  )) , this,  SLOT(workFlowClicked(QTreeWidgetItem * , int ) ) );

    connect(ui->workFlowTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem * )) , this,  SLOT(adaptLayout(QTreeWidgetItem *)));
    connect(ui->workFlowTreeWidget, SIGNAL(itemCollapsed(QTreeWidgetItem * )) , this,  SLOT(adaptLayout(QTreeWidgetItem *)));
}

SADialog::~SADialog()
{
    if (ui != NULL)
    {
        delete ui;
        ui = NULL;
    }
}

void SADialog::initSupportWorkFlow()
{
    std::vector<sat::WorkFlowFactory*> workflowFactories = sat::meshlab::supportedWorkFlows();
    for (int i = 0, in = workflowFactories.size(); i < in; i++)
    {
        sat::WorkFlowFactory* factory = workflowFactories[i];
        
        WorkFlowWidgetItem* item = new WorkFlowWidgetItem(factory);
        item->setText(1, tr(factory->getWorkFlowName()));
        ui->workFlowTreeWidget->addTopLevelItem(item);
    }
}

void SADialog::workFlowClicked (QTreeWidgetItem * item , int col)
{
    if(item)
    {
        WorkFlowWidgetItem* workFlowItem = dynamic_cast<WorkFlowWidgetItem*>(item);
        sat::WorkFlowFactory* factory = (sat::WorkFlowFactory*)workFlowItem->factory;
        sat::WorkFlow* workFlow = factory->create();
        printf("%ld\n", workFlow);
        printf("%d\n", col);
        
        delete workFlow;
    }
}

void SADialog::adaptLayout(QTreeWidgetItem * item)
{
    item->setExpanded(item->isExpanded());
    updateTreeWidgetSizes(ui->workFlowTreeWidget);

    //Update expandedMap
    WorkFlowWidgetItem *mItem = dynamic_cast<WorkFlowWidgetItem *>(item);
}

void SADialog::updateTreeWidgetSizes(void* vtree)
{
    QTreeWidget* tree = (QTreeWidget*)vtree;
    for(int i=0; i< tree->columnCount(); i++) {
        int oldSize = tree->columnWidth(i);
        tree->resizeColumnToContents(i);
        if (tree->columnWidth(i) > 200){
            tree->setColumnWidth(i, oldSize);
        }
    }
}

WorkFlowWidgetItem::WorkFlowWidgetItem(void* factory)
{
    this->factory = factory;
    this->updateIcon();
}
WorkFlowWidgetItem::~WorkFlowWidgetItem()
{
    
}
void WorkFlowWidgetItem::updateIcon()
{
    setIcon(0,QIcon(":/images/sa_parameteration.png"));
}
