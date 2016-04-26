#include "QOSG/ProjectiveARWindow.h"
#include "QOSG/ProjectiveARViewer.h"
#include "QOSG/ProjectiveAREventHandler.h"

#include "QOSG/ViewerQT.h"

#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFormLayout>
#include <QCloseEvent>
#include <QDebug>

#include "Util/ApplicationConfig.h"

QOSG::ProjectiveARWindow::ProjectiveARWindow( QWidget* parent, ViewerQT* sourceViewer, QApplication* app ) : QDialog( parent )
{
    this->app = app;
    viewerWidget = new ProjectiveARViewer( this, 0, 0, 0, sourceViewer );
    viewerWidget->addEventHandler(new ProjectiveAREventHandler(this));
    createLeftToolBar();

    setModal( false );
    setWindowTitle( tr( "Projector View Window" ) );

    QHBoxLayout* mainLayout		= new QHBoxLayout;
    //set layout
    mainLayout->addWidget( toolBar, Qt::AlignLeft );
    mainLayout->addWidget( viewerWidget, Qt::AlignCenter );
    mainLayout->setSizeConstraint( QLayout::SetMinimumSize );
    mainLayout->setMargin(0);
    setLayout( mainLayout );
    adjustSize();

    resize(1024, 764);

}

void QOSG::ProjectiveARWindow::toggleToolBars()
{
    if(toolBar->isHidden())
    {
        toolBar->show();
    }
    else
    {
        toolBar->hide();
    }
}

void QOSG::ProjectiveARWindow::createLeftToolBar()
{
    QFrame* line = NULL;

    QWidget* wScene = createSceneTab( line );

    toolBox = new QToolBox();
    toolBox->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum ) );
    toolBox->setMinimumWidth( 163 );
    toolBox->addItem( wScene, tr( "Scene" ) );
    toolBar = new QToolBar( "Tools",this );

    QFrame* frame = createHorizontalFrame();
    frame->layout()->addWidget( toolBox );
    frame->layout()->setMargin(0);
    toolBar->addWidget( frame );

    toolBar->setMaximumWidth( 200 );
    //toolBar->setContentsMargins(0,0,0,0);
    //toolBar->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy:: ) );
    toolBar->setMovable( false );
}

QWidget* QOSG::ProjectiveARWindow::createSceneTab( QFrame* line )
{
    QWidget* wScene = new QWidget();
    QFormLayout* lScene = new QFormLayout( wScene );
    lScene->setContentsMargins( 1,1,1,1 );
    lScene->setSpacing( 2 );

    osg::Vec3d projectorPos = viewerWidget->getProjectorPos();
    osg::Vec3d projectorDir = viewerWidget->getProjectorDir();
    double projectorFOV = viewerWidget->getProjectorFOV();

    osg::Vec3d viewerPos = viewerWidget->getViewerPos();
    osg::Vec3d viewerDir = viewerWidget->getViewerDir();
    double viewerFOV = viewerWidget->getViewerFOV();

    // init options
    dsb_projectorFOV = new QDoubleSpinBox();
    dsb_projectorFOV->setValue(projectorFOV);
    dsb_projectorFOV->setToolTip("Projector field of view");
    dsb_projectorPosX = new QDoubleSpinBox();
    dsb_projectorPosX->setValue(projectorPos.x());
    dsb_projectorPosX->setToolTip("Projector X position");
    dsb_projectorPosY = new QDoubleSpinBox();
    dsb_projectorPosY->setValue(projectorPos.y());
    dsb_projectorPosY->setToolTip("Projector Y position");
    dsb_projectorPosZ = new QDoubleSpinBox();
    dsb_projectorPosZ->setValue(projectorPos.z());
    dsb_projectorPosZ->setToolTip("Projector Z position");
    dsb_projectorDirX = new QDoubleSpinBox();
    dsb_projectorDirX->setValue(projectorDir.x());
    dsb_projectorDirX->setToolTip("Projector X direction");
    dsb_projectorDirY = new QDoubleSpinBox();
    dsb_projectorDirY->setValue(projectorDir.y());
    dsb_projectorDirY->setToolTip("Projector Y direction");
    dsb_projectorDirZ = new QDoubleSpinBox();
    dsb_projectorDirZ->setValue(projectorDir.z());
    dsb_projectorDirZ->setToolTip("Projector Z direction");

    dsb_viewerFOV = new QDoubleSpinBox();
    dsb_viewerFOV->setValue(viewerFOV);
    dsb_viewerFOV->setToolTip("Viewer field of view");
    dsb_viewerPosX = new QDoubleSpinBox();
    dsb_viewerPosX->setValue(viewerPos.x());
    dsb_viewerPosX->setToolTip("Viewer X position");
    dsb_viewerPosY = new QDoubleSpinBox();
    dsb_viewerPosY->setValue(viewerPos.y());
    dsb_viewerPosY->setToolTip("Viewer Y position");
    dsb_viewerPosZ = new QDoubleSpinBox();
    dsb_viewerPosZ->setValue(viewerPos.z());
    dsb_viewerPosZ->setToolTip("Viewer Z position");
    dsb_viewerDirX = new QDoubleSpinBox();
    dsb_viewerDirX->setValue(viewerDir.x());
    dsb_viewerDirX->setToolTip("Viewer X direction");
    dsb_viewerDirY = new QDoubleSpinBox();
    dsb_viewerDirY->setValue(viewerDir.y());
    dsb_viewerDirY->setToolTip("Viewer Y direction");
    dsb_viewerDirZ = new QDoubleSpinBox();
    dsb_viewerDirZ->setValue(viewerDir.z());
    dsb_viewerDirZ->setToolTip("Viewer Z direction");

    b_applyScene = new QPushButton(tr("Apply scene"));


    // set tab layout
    lScene->addRow( new QLabel( "Projector" ) );
    dsb_projectorFOV->setMaximumWidth(50);
    lScene->addRow( new QLabel( "FOV:" ), dsb_projectorFOV );
    dsb_projectorPosX->setMaximumWidth(50);
    lScene->addRow( new QLabel( "Pos X:" ), dsb_projectorPosX );
    dsb_projectorPosY->setMaximumWidth(50);
    lScene->addRow( new QLabel( "Pos Y:" ), dsb_projectorPosY );
    dsb_projectorPosZ->setMaximumWidth(50);
    lScene->addRow( new QLabel( "Pos Z:" ), dsb_projectorPosZ );
    dsb_projectorDirX->setMaximumWidth(50);
    lScene->addRow( new QLabel( "Dir X:" ), dsb_projectorDirX );
    dsb_projectorDirY->setMaximumWidth(50);
    lScene->addRow( new QLabel( "Dir Y:" ), dsb_projectorDirY );
    dsb_projectorDirZ->setMaximumWidth(50);
    lScene->addRow( new QLabel( "Dir Z:" ), dsb_projectorDirZ );
    lScene->addRow(createLine());

    lScene->addRow( new QLabel( "Projector" ) );
    dsb_viewerFOV->setMaximumWidth(50);
    lScene->addRow( new QLabel( "FOV:" ), dsb_viewerFOV );
    dsb_viewerPosX->setMaximumWidth(50);
    lScene->addRow( new QLabel( "Pos X:" ), dsb_viewerPosX );
    dsb_viewerPosY->setMaximumWidth(50);
    lScene->addRow( new QLabel( "Pos Y:" ), dsb_viewerPosY );
    dsb_viewerPosZ->setMaximumWidth(50);
    lScene->addRow( new QLabel( "Pos Z:" ), dsb_viewerPosZ );
    dsb_viewerDirX->setMaximumWidth(50);
    lScene->addRow( new QLabel( "Dir X:" ), dsb_viewerDirX );
    dsb_viewerDirY->setMaximumWidth(50);
    lScene->addRow( new QLabel( "Dir Y:" ), dsb_viewerDirY );
    dsb_viewerDirZ->setMaximumWidth(50);
    lScene->addRow( new QLabel( "Dir Z:" ), dsb_viewerDirZ );
    lScene->addRow(createLine());

    lScene->addRow(b_applyScene);
    connect( b_applyScene, SIGNAL( clicked() ), this, SLOT( applySceneSettings() ) );

    wScene->setLayout(lScene);

    return wScene;
}
/*
QGroupBox* QOSG::ProjectiveARWindow::createProjectorGroupBox()
{
    QGroupBox* group = createGroupBox( tr( "Radial layout" ) );
    QFormLayout* layoutGroup = createGroupLayout();
    b_SetRestriction_RadialLayout->setMinimumWidth( 68 );



    group->setLayout( layoutGroup );

    return group;
}*/

QFrame* QOSG::ProjectiveARWindow::createLine()
{
    QFrame* line = new QFrame();
    line->setFrameShape( QFrame::HLine );
    line->setFrameShadow( QFrame::Sunken );
    line->setMinimumHeight( 15 );
    line->setMaximumWidth( 138 );

    return line;
}

QFrame* QOSG::ProjectiveARWindow::createHorizontalFrame()
{
    QFrame* frame = new QFrame();
    QHBoxLayout* layout = new QHBoxLayout();
    frame->setLayout( layout );
    layout->setMargin( 0 );
    layout->setSpacing( 0 );

    return frame;
}

void QOSG::ProjectiveARWindow::closeEvent( QCloseEvent* event )
{
    qDebug() << "Closing ProjectiveARWindow Window";

	disconnect();

	event->accept();
}


void QOSG::ProjectiveARWindow::applySceneSettings()
{
    /*viewerWidget->setProjectorPos(dsb_projectorPosX->value(),
                                  dsb_projectorPosY->value(),
                                  dsb_projectorPosZ->value());
    viewerWidget->setProjectorDir(dsb_projectorDirX->value(),
                                  dsb_projectorDirY->value(),
                                  dsb_projectorDirZ->value());
    viewerWidget->setProjectorFOV(dsb_projectorFOV->value());

    viewerWidget->setViewerPos(dsb_viewerPosX->value(),
                               dsb_viewerPosY->value(),
                               dsb_viewerPosZ->value());
    viewerWidget->setViewerDir(dsb_viewerDirX->value(),
                               dsb_viewerDirY->value(),
                               dsb_viewerDirZ->value());
    viewerWidget->setViewerFOV(dsb_viewerFOV->value());*/

    viewerWidget->updateScene();
}
