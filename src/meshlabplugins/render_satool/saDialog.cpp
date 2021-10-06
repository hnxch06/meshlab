// free icon https://www.iconpacks.net/free-icon/rocket-3420.html

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
#include "sadelegate.h"

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
    sat::DisplayManager::getInstance()->removeWorkFlowChangedListener(this);
    
    if (ui != NULL)
    {
        delete ui;
        ui = NULL;
    }
}

void SADialog::workFlowChangedListener(sat::WorkFlow* from, sat::WorkFlow* to)
{
    if (glarea != NULL)
    {
        glarea->repaint();
    }
}

void SADialog::initSupportWorkFlow()
{
    WorkFlowWidgetItem* item = new WorkFlowWidgetItem(NULL);
    item->setIcon(0, QIcon(":/images/convert-3217.png"));
    ui->workFlowTreeWidget->addTopLevelItem(item);
    
    std::vector<sat::WorkFlowFactory*> workflowFactories = sat::meshlab::supportedWorkFlows();
    for (int i = 0, in = workflowFactories.size(); i < in; i++)
    {
        sat::WorkFlowFactory* factory = workflowFactories[i];
        
        WorkFlowWidgetItem* item = new WorkFlowWidgetItem(factory);
        item->setIcon(0, QIcon(":/images/start.png"));
        item->setText(1, tr(factory->getWorkFlowName()));
        item->setIcon(2, QIcon(":/images/view.png"));
        ui->workFlowTreeWidget->addTopLevelItem(item);
    }
    
    std::function<void(sat::WorkFlow*, sat::WorkFlow*)> fn = std::bind(&SADialog::workFlowChangedListener , this, std::placeholders::_1, std::placeholders::_2);
    sat::DisplayManager::getInstance()->addWorkFlowChangedListener(this, fn);
}

void SADialog::workFlowClicked (QTreeWidgetItem * item , int col)
{
    if(item)
    {
        WorkFlowWidgetItem* workFlowItem = dynamic_cast<WorkFlowWidgetItem*>(item);
        if (workFlowItem->factory == nullptr)
        {
            sat::DisplayManager::getInstance()->activeWorkFlow(nullptr);
            return;
        }
        
        if (col == 0)
        {
            void* model = sat::DisplayManager::getInstance()->getDisplayModel();
            if (model == nullptr)
            {
                printf("no model selected\n");
                return;
            }
            
            WorkFlowWidgetItem* workFlowItem = dynamic_cast<WorkFlowWidgetItem*>(item);
            sat::WorkFlowFactory* factory = (sat::WorkFlowFactory*)workFlowItem->factory;
            sat::WorkFlow* workFlow = factory->create();
            
            sat::WorkFlowSharedData inputData;
            void* currentMesh = sat::DisplayManager::getInstance()->getDisplayModel();
            if (currentMesh != nullptr)
            {
                MeshModel* mp = (MeshModel*)currentMesh;
                sat::Model* model = SAUtil::convertMeshFromMeshlabToSAGeo(mp);
                inputData.setRef(model, sat::deleteShareData);
                workFlow->addSharedData(workFlow->getInputLabel(), inputData);
                workFlow->executeFrame();
                
                sat::DisplayManager::getInstance()->addWorkFlow(item, workFlow);
                sat::DisplayManager::getInstance()->activeWorkFlow(item);
            }
        } else
        {
            sat::DisplayManager::getInstance()->activeWorkFlow(item);
        }
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
}
WorkFlowWidgetItem::~WorkFlowWidgetItem()
{
    
}
