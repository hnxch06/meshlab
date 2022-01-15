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
    connect(ui->frameExecuteCheckBox, SIGNAL(stateChanged(int)), this, SLOT(frameExecuteStateChange(int)));
    connect(ui->resetPauseFrame, SIGNAL(released()), this, SLOT(pauseFrame()));
    connect(ui->debugButton, SIGNAL(released()), this, SLOT(debugClicked()));
    connect(ui->selectWorkFlowJobTreeWidget, SIGNAL(itemPressed(QTreeWidgetItem * , int  )) , this, SLOT(jobDetailClicked(QTreeWidgetItem * , int ) ) );
    
    mFrameTimeID = startTimer(16);
    sat::DisplayManager::getInstance()->initInMainThread();
}

SADialog::~SADialog()
{
    sat::DisplayManager::getInstance()->removeWorkFlowChangedListener(this);
    killTimer(mFrameTimeID);
    
    if (ui != NULL)
    {
        delete ui;
        ui = NULL;
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
    
    std::function<void(sat::WorkFlow*, sat::WorkFlow*)> fn = std::bind(&SADialog::workFlowChangedListener, this, std::placeholders::_1, std::placeholders::_2, true);
    sat::DisplayManager::getInstance()->addWorkFlowChangedListener(this, fn);
}

void SADialog::timerEvent(QTimerEvent *)
{
    sat::DisplayManager::getInstance()->mainThreadRelease();
    
    std::shared_ptr<sat::WorkFlow> displaying = sat::DisplayManager::getInstance()->getDisplayingWorkFlow();
    if (displaying != nullptr && displaying->getAndResetStatusChangeFlag())
    {
        workFlowChangedListener(nullptr, displaying.get(), false);
        if (glarea != NULL)
        {
            glarea->repaint();
        }
    }
    
//    if (glarea != NULL)
//    {
//        glarea->repaint();
//    }
}

void SADialog::frameExecuteStateChange(int state)
{
    std::shared_ptr<sat::WorkFlow> displaying = sat::DisplayManager::getInstance()->getDisplayingWorkFlow();
    if (displaying != nullptr)
    {
        if (state != 0)
        {
            int frameCount = ui->frameExecuteText->text().toInt();
            if (frameCount == 0) frameCount = INT_MAX;
            displaying->setPauseInFrame(frameCount);
        } else
        {
            displaying->setPauseInFrame(INT_MAX);
        }
    }
}

void SADialog::debugClicked()
{
    printf("debug clicked\n");
}

void SADialog::pauseFrame()
{
    std::shared_ptr<sat::WorkFlow> displaying = sat::DisplayManager::getInstance()->getDisplayingWorkFlow();
    if (displaying != nullptr && displaying->getStatus() == sat::WorkFlow::Status::EXECUTING)
    {
        displaying->setPauseInFrame(1);
    }
}

void SADialog::workFlowChangedListener(sat::WorkFlow* from, sat::WorkFlow* to, bool refreshUi)
{
    ui->selectWorkFlowJobTreeWidget->clear();
    
    if (to != nullptr)
    {
        sat::WorkFlow* workFlow = to;
        const sat::Job* eJob = workFlow->executingJob();
        const sat::Job* vJob = workFlow->viewingJob();
        for (int i = 0, size = workFlow->getJobSize(); i < size; i++)
        {
            sat::Job* job = workFlow->getJobAt(i);
            JobWidgetItem* item = new JobWidgetItem(job);
            
            if (job->getStatus() == sat::Job::Status::EXECUTING)
            {
                if (workFlow->getStatus() == sat::WorkFlow::Status::EXECUTING)
                {
                    item->setIcon(0, QIcon(":/images/execute_running.png"));
                } else {
                    item->setIcon(0, QIcon(":/images/execute_pause.png"));
                }
            } else if (job->getStatus() == sat::Job::Status::FINISHED)
            {
                item->setIcon(0, QIcon(":/images/execute_success.png"));
            } else if (job->getStatus() == sat::Job::Status::FINISHED_SKIP)
            {
                item->setIcon(0, QIcon(":/images/execute_error1.png"));
            }  else if (job->getStatus() == sat::Job::Status::ERROR)
            {
                item->setIcon(0, QIcon(":/images/execute_error.png"));
            } else if (job->isFrameBreak())
            {
                item->setIcon(0, QIcon(":/images/break_point.png"));
            } else if (job->getStatus() == sat::Job::Status::INIT)
            {
                item->setIcon(0, QIcon(":/images/execute_hold.png"));
            }
            
            if (job == eJob && workFlow->getStatus() == sat::WorkFlow::Status::CANCELLED)
            {
                item->setIcon(0, QIcon(":/images/execute_cancel.png"));
            }
            
            if (job == eJob)
            {
                item->setBackground(1, QBrush(QColor("#666600")));
            }
            item->setText(1, tr(job->getName()));
            
            if (job == vJob)
            {
                item->setIcon(2, QIcon(":/images/sa_eye.png"));
            } else
            {
                item->setIcon(2, QIcon(":/images/view.png"));
            }
            ui->selectWorkFlowJobTreeWidget->addTopLevelItem(item);
        }
    }
    
    if (refreshUi && glarea != NULL)
    {
        glarea->repaint();
    }
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
        
        void* currentMesh = sat::DisplayManager::getInstance()->getDisplayModel();
        
        bool isCreate = col == 0;
        bool isView = col != 0;
        
        // create or recreate or view
        sat::WorkFlowFactory* factory = (sat::WorkFlowFactory*)workFlowItem->factory;
        if (factory->latestPtr == nullptr)
        {
            std::shared_ptr<sat::WorkFlow> workFlow = factory->create();
            workFlow->setDebugLevel(1);
            sat::DisplayManager::getInstance()->addWorkFlow(item, workFlow);
            factory->latestPtr = workFlow;
        }
        
        if (isCreate && factory->latestPtr->isOver())
        {
            std::shared_ptr<sat::WorkFlow> workFlow = factory->create();
            workFlow->setDebugLevel(1);
            sat::DisplayManager::getInstance()->addWorkFlow(item, workFlow);
            factory->latestPtr = workFlow;
        }
        
        if (isCreate && currentMesh != nullptr && !factory->latestPtr->getSharedContext().contains(factory->latestPtr->getInputLabel()))
        {
            MeshModel* mp = (MeshModel*)currentMesh;
            sat::Model* model = SADataUtil::convertMeshFromMeshlabToSAGeo(mp);
            factory->latestPtr->getSharedContext().addRef(factory->latestPtr->getInputLabel(), model, sat::deleteShareData<sat::Model>);
        }
        
        if (isCreate && currentMesh != nullptr && factory->latestPtr->getStatus() != sat::WorkFlow::Status::EXECUTING)
        {
            if (ui->frameExecuteCheckBox)
            {
                int frameCount = ui->frameExecuteText->text().toInt();
                if (frameCount == 0) frameCount = INT_MAX;
                factory->latestPtr->setPauseInFrame(frameCount);
            } else
            {
                factory->latestPtr->setPauseInFrame(INT_MAX);
            }
            factory->latestPtr->executeFrameInSubThread();
        }
        sat::DisplayManager::getInstance()->activeWorkFlow(item);
    }
}

void SADialog::jobDetailClicked(QTreeWidgetItem * item, int col)
{
    if(item)
    {
        JobWidgetItem* jobDetailItem = dynamic_cast<JobWidgetItem*>(item);
        if (jobDetailItem->job == nullptr) return;
        std::shared_ptr<sat::WorkFlow> workFlow = sat::DisplayManager::getInstance()->getDisplayingWorkFlow();
        if (workFlow == nullptr) return;
        
        bool isSetBreakFrame = col == 0;
        
        if (!isSetBreakFrame)
        {
            if (jobDetailItem->job == workFlow->executingJob())
            {
                workFlow->setViewingJobIndex(-1);
            } else
            {
                int jobIndex = workFlow->getJobIndex((sat::Job*)jobDetailItem->job);
                if (jobIndex < 0) return;
                workFlow->setViewingJobIndex(jobIndex);
            }
        } else
        {
            sat::Job* job = (sat::Job*)(jobDetailItem->job);
            int jobIndex = workFlow->getJobIndex(job);
            job->toggleBreakFrame();
            workFlow->getFactory()->toggleBreakFrame(jobIndex, 0);
            workFlow->setStatusChanged();
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

JobWidgetItem::JobWidgetItem(void* job)
{
    this->job = job;
}
JobWidgetItem::~JobWidgetItem()
{
    
}
