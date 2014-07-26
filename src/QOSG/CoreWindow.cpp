#include "QOSG/CoreWindow.h"

#include "QOSG/ViewerQT.h"
#include "QOSG/OptionsWindow.h"
#include "QOSG/LoadGraphWindow.h"
#include "QOSG/MessageWindows.h"
#include "QOSG/qtcolorpicker.h"

#include "Network/Server.h"
#include "Network/Client.h"

#include "Viewer/CoreGraph.h"
#include "Viewer/CameraManipulator.h"
#include "Viewer/PickHandler.h"

#include "Manager/Manager.h"

#include "Model/DB.h"
#include "Model/GraphLayoutDAO.h"

#include "Data/GraphLayout.h"

#include "Layout/LayoutThread.h"
#include "Layout/FRAlgorithm.h"
#include "Layout/ShapeGetter_CylinderSurface_ByCamera.h"
#include "Layout/ShapeGetter_SphereSurface_ByTwoNodes.h"
#include "Layout/ShapeGetter_Sphere_ByTwoNodes.h"
#include "Layout/ShapeGetter_ConeSurface_ByCamera.h"
#include "Layout/ShapeGetter_Plane_ByThreeNodes.h"
#include "Layout/RadialLayout.h"

#include "Importer/GraphOperations.h"

#ifdef OPENCV_FOUND
#include "OpenCV/OpenCVCore.h"
#endif

#ifdef OPENCV_FOUND
#ifdef OPENNI2_FOUND
#ifdef NITE2_FOUND
#include "Kinect/KinectCore.h"
#include "Kinect/RansacSurface/Ransac.h"
#endif
#endif
#endif

#include "Util/Cleaner.h"

#include "Core/Core.h"

#include "Data/GraphSpanningTree.h"

#include <iostream>
#include "QDebug"

using namespace QOSG;
using namespace std;

CoreWindow::CoreWindow(QWidget *parent, Vwr::CoreGraph* coreGraph, QApplication* app, Layout::LayoutThread * thread ) : QMainWindow(parent)
{
	//inicializacia premennych
	isPlaying = true;
	application = app;
	layout = thread;

	client = new Network::Client(this);
	new Network::Server(this);

	//vytvorenie menu a toolbar-ov
	createActions();
	createMenus();
	createLeftToolBar();
	createRightToolBar();
	createCollaborationToolBar();
	createAugmentedRealityToolBar();

	viewerWidget = new ViewerQT(this, 0, 0, 0, coreGraph);
	viewerWidget->setSceneData(coreGraph->getScene());

	setCentralWidget(viewerWidget);

	statusBar()->showMessage("Ready");

	dock = new QDockWidget(this);
	dock->setAllowedAreas(Qt::TopDockWidgetArea);

	lineEdit = new QLineEdit();

	//vytvorenie sql vstupu
	QWidget * widget = new QWidget();
	QHBoxLayout *layoutt = new QHBoxLayout();
	layoutt->addWidget(lineEdit);
	widget->setLayout(layoutt);
	dock->setWidget(widget);
	addDockWidget(Qt::TopDockWidgetArea, dock);
	dock->hide();

	this->coreGraph = coreGraph;
	nodeLabelsVisible = edgeLabelsVisible = false;

    // Duransky start - Inicializacia premennych

        // inicializacia poctu rovin pre vertigo mode
        numberOfPlanes = 0;

        // koeficient zmeny vzdialenosti rovin pre vertigo mode
        deltaVertigoDistance = 20;

        // vychodzia hodnota pre vzdialenost medzi rovinami pre vertigo mode
        vertigoPlanesDistance = 100;

        // ohranicenie vzdialenosti medzi rovinami pre vertigo mode
        vertigoPlanesMinDistance = 50;
        vertigoPlanesMaxDistance = 300;

    // Duransky end - Inicializacia premennych

	connect(lineEdit,SIGNAL(returnPressed()),this,SLOT(sqlQuery()));


	// connect checkbox for interchanging between rotation of camera or rotation of graph
	QObject::connect( chb_camera_rot, SIGNAL(clicked(bool)),
					  viewerWidget->getCameraManipulator(), SLOT(setCameraCanRot(bool)));

}

void CoreWindow::createActions()
{
	quit = new QAction("Quit", this);
	connect(quit, SIGNAL(triggered()), application, SLOT(quit()));

	options = new QAction("Options", this);
	connect(options,SIGNAL(triggered()),this,SLOT(showOptions()));

	load = new QAction(QIcon("../share/3dsoftviz/img/gui/open.png"),"&Load graph from file", this);
	connect(load, SIGNAL(triggered()), this, SLOT(loadFile()));

	loadGraph = new QAction(QIcon("../share/3dsoftviz/img/gui/loadFromDB.png"),"&Load graph from database", this);
	connect(loadGraph, SIGNAL(triggered()), this, SLOT(showLoadGraph()));

	saveGraph = new QAction(QIcon("../share/3dsoftviz/img/gui/saveToDB.png"),"&Save graph", this);
	connect(saveGraph, SIGNAL(triggered()), this, SLOT(saveGraphToDB()));

	saveLayout = new QAction(QIcon("../share/3dsoftviz/img/gui/saveToDB.png"),"&Save layout", this);
	connect(saveLayout, SIGNAL(triggered()), this, SLOT(saveLayoutToDB()));

	play = new QPushButton();
	play->setIcon(QIcon("../share/3dsoftviz/img/gui/pause.png"));
	play->setToolTip("&Play");
	play->setFocusPolicy(Qt::NoFocus);
	connect(play, SIGNAL(clicked()), this, SLOT(playPause()));

	addMeta = new QPushButton();
	addMeta->setIcon(QIcon("../share/3dsoftviz/img/gui/meta.png"));
	addMeta->setToolTip("&Add meta node");
	addMeta->setFocusPolicy(Qt::NoFocus);
	connect(addMeta, SIGNAL(clicked()), this, SLOT(addMetaNode()));

	removeMeta = new QPushButton();
	removeMeta->setIcon(QIcon("../share/3dsoftviz/img/gui/removemeta.png"));
	removeMeta->setToolTip("&Remove meta nodes");
	removeMeta->setFocusPolicy(Qt::NoFocus);
	connect(removeMeta, SIGNAL(clicked()), this, SLOT(removeMetaNodes()));

	fix = new QPushButton();
	fix->setIcon(QIcon("../share/3dsoftviz/img/gui/fix.png"));
	fix->setToolTip("&Fix nodes");
	fix->setFocusPolicy(Qt::NoFocus);
	connect(fix, SIGNAL(clicked()), this, SLOT(fixNodes()));

	unFix = new QPushButton();
	unFix->setIcon(QIcon("../share/3dsoftviz/img/gui/unfix.png"));
	unFix->setToolTip("&Unfix nodes");
	unFix->setFocusPolicy(Qt::NoFocus);
	connect(unFix, SIGNAL(clicked()), this, SLOT(unFixNodes()));

	merge = new QPushButton();
	merge->setIcon(QIcon("../share/3dsoftviz/img/gui/merge.png"));
	merge->setToolTip("&Merge nodes together");
	merge->setFocusPolicy(Qt::NoFocus);
	connect(merge, SIGNAL(clicked()), this, SLOT(mergeNodes()));

	separate = new QPushButton();
	separate->setIcon(QIcon("../share/3dsoftviz/img/gui/separate.png"));
	separate->setToolTip("&Separate merged nodes");
	separate->setFocusPolicy(Qt::NoFocus);
	connect(separate, SIGNAL(clicked()), this, SLOT(separateNodes()));

	label = new QPushButton();
	label->setIcon(QIcon("../share/3dsoftviz/img/gui/label.png"));
	label->setToolTip("&Turn on/off labels");
	label->setCheckable(true);
	label->setFocusPolicy(Qt::NoFocus);
	connect(label, SIGNAL(clicked(bool)), this, SLOT(labelOnOff(bool)));

	applyColor = new QPushButton();
	applyColor->setText("Apply color");
	applyColor->setToolTip("Apply selected color");
	applyColor->setFocusPolicy(Qt::NoFocus);
	connect(applyColor,SIGNAL(clicked()),this,SLOT(applyColorClick()));

	applyLabel = new QPushButton();
	applyLabel->setText("Apply label");
	applyLabel->setToolTip("Apply selected label");
	applyLabel->setFocusPolicy(Qt::NoFocus);
	connect(applyLabel,SIGNAL(clicked()),this,SLOT(applyLabelClick()));

	le_applyLabel = new QLineEdit;

	//add edge
	add_Edge = new QPushButton();
	add_Edge->setText("Add Edge");
	add_Edge->setToolTip("Create new edge between two selected Nodes");
	add_Edge->setFocusPolicy(Qt::NoFocus);
	connect(add_Edge,SIGNAL(clicked()),this,SLOT(add_EdgeClick()));

	//add Node
	add_Node = new QPushButton();
	add_Node->setText("Add Node");
	add_Node->setToolTip("Create node");
	add_Node->setFocusPolicy(Qt::NoFocus);
	connect(add_Node, SIGNAL(clicked()), this, SLOT(add_NodeClick()));

	//remove
	remove_all = new QPushButton();
	remove_all->setText("Remove");
	remove_all->setToolTip("Remove nodes and edges");
	remove_all->setFocusPolicy(Qt::NoFocus);
	connect(remove_all, SIGNAL(clicked()), this, SLOT(removeClick()));

	//mody - ziadny vyber, vyber jedneho, multi vyber centrovanie
	noSelect = new QPushButton();
	noSelect->setIcon(QIcon("../share/3dsoftviz/img/gui/noselect.png"));
	noSelect->setToolTip("&No-select mode");
	noSelect->setCheckable(true);
	noSelect->setFocusPolicy(Qt::NoFocus);
	connect(noSelect, SIGNAL(clicked(bool)), this, SLOT(noSelectClicked(bool)));

	singleSelect = new QPushButton();
	singleSelect->setIcon(QIcon("../share/3dsoftviz/img/gui/singleselect.png"));
	singleSelect->setToolTip("&Single-select mode");
	singleSelect->setCheckable(true);
	singleSelect->setFocusPolicy(Qt::NoFocus);
	connect(singleSelect, SIGNAL(clicked(bool)), this, SLOT(singleSelectClicked(bool)));

	multiSelect = new QPushButton();
	multiSelect->setIcon(QIcon("../share/3dsoftviz/img/gui/multiselect.png"));
	multiSelect->setToolTip("&Multi-select mode");
	multiSelect->setCheckable(true);
	multiSelect->setFocusPolicy(Qt::NoFocus);
	connect(multiSelect, SIGNAL(clicked(bool)), this, SLOT(multiSelectClicked(bool)));

	center = new QPushButton();
	center->setIcon(QIcon("../share/3dsoftviz/img/gui/center.png"));
	center->setToolTip("&Center view");
	center->setFocusPolicy(Qt::NoFocus);
	connect(center, SIGNAL(clicked(bool)), this, SLOT(centerView(bool)));

	// layout restrictions
	b_SetRestriction_SphereSurface = new QPushButton();
	b_SetRestriction_SphereSurface->setIcon(QIcon("../share/3dsoftviz/img/gui/restriction_sphere_surface.png"));
	b_SetRestriction_SphereSurface->setToolTip("&Set restriction - sphere surface");
	b_SetRestriction_SphereSurface->setFocusPolicy(Qt::NoFocus);
	connect(b_SetRestriction_SphereSurface, SIGNAL(clicked()), this, SLOT(setRestriction_SphereSurface ()));

	b_SetRestriction_Sphere = new QPushButton();
	b_SetRestriction_Sphere->setIcon(QIcon("../share/3dsoftviz/img/gui/restriction_sphere.png"));
    b_SetRestriction_Sphere->setToolTip("&Set restriction - sphere");
	b_SetRestriction_Sphere->setFocusPolicy(Qt::NoFocus);
	connect(b_SetRestriction_Sphere, SIGNAL(clicked()), this, SLOT(setRestriction_Sphere ()));

	b_SetRestriction_Plane = new QPushButton();
	b_SetRestriction_Plane->setIcon(QIcon("../share/3dsoftviz/img/gui/restriction_plane.png"));
	b_SetRestriction_Plane->setToolTip("&Set restriction - plane");
	b_SetRestriction_Plane->setFocusPolicy(Qt::NoFocus);
	connect(b_SetRestriction_Plane, SIGNAL(clicked()), this, SLOT(setRestriction_Plane ()));

	b_SetRestriction_SpherePlane = new QPushButton();
	b_SetRestriction_SpherePlane->setIcon(QIcon("../share/3dsoftviz/img/gui/restriction_sphereplane.png"));
	b_SetRestriction_SpherePlane->setToolTip("&Set restriction - sphere and plane");
	b_SetRestriction_SpherePlane->setFocusPolicy(Qt::NoFocus);
	connect(b_SetRestriction_SpherePlane, SIGNAL(clicked()), this, SLOT(setRestriction_SpherePlane ()));

	b_SetRestriction_Circle = new QPushButton();
	b_SetRestriction_Circle->setIcon(QIcon("../share/3dsoftviz/img/gui/restriction_circle.png"));
	b_SetRestriction_Circle->setToolTip("&Set restriction - circle");
	b_SetRestriction_Circle->setFocusPolicy(Qt::NoFocus);
	connect(b_SetRestriction_Circle, SIGNAL(clicked()), this, SLOT(setRestriction_Circle ()));

	b_SetRestriction_Cone = new QPushButton();
	b_SetRestriction_Cone->setIcon(QIcon("../share/3dsoftviz/img/gui/restriction_cone.png"));
	b_SetRestriction_Cone->setToolTip("&Set restriction - cone");
	b_SetRestriction_Cone->setFocusPolicy(Qt::NoFocus);
	connect(b_SetRestriction_Cone, SIGNAL(clicked()), this, SLOT(setRestriction_Cone ()));

	b_SetRestriction_ConeTree = new QPushButton();
	b_SetRestriction_ConeTree->setIcon(QIcon("../share/3dsoftviz/img/gui/restriction_conetree.png"));
	b_SetRestriction_ConeTree->setToolTip("&Set restriction - cone tree");
	b_SetRestriction_ConeTree->setFocusPolicy(Qt::NoFocus);
	connect(b_SetRestriction_ConeTree, SIGNAL(clicked()), this, SLOT(setRestriction_ConeTree ()));

	b_UnsetRestriction = new QPushButton();
	b_UnsetRestriction->setIcon(QIcon("../share/3dsoftviz/img/gui/restriction_unset.png"));
	b_UnsetRestriction->setToolTip("&Unset restriction");
	b_UnsetRestriction->setFocusPolicy(Qt::NoFocus);
	connect(b_UnsetRestriction, SIGNAL(clicked()), this, SLOT(unsetRestriction()));

	b_SetRestriction_CylinderSurface = new QPushButton();
	b_SetRestriction_CylinderSurface->setIcon(QIcon("../share/3dsoftviz/img/gui/restriction_cylinder_surface.png"));
	b_SetRestriction_CylinderSurface->setToolTip("&Set restriction - cylinder surface");
	b_SetRestriction_CylinderSurface->setFocusPolicy(Qt::NoFocus);
	connect(b_SetRestriction_CylinderSurface, SIGNAL(clicked()), this, SLOT(setRestriction_CylinderSurface()));

	b_SetRestriction_CylinderSurface_Slider = new QSlider(Qt::Horizontal);
	b_SetRestriction_CylinderSurface_Slider->setToolTip("&Modify base radius of the restriction");
	b_SetRestriction_CylinderSurface_Slider->setFocusPolicy(Qt::NoFocus);
	b_SetRestriction_CylinderSurface_Slider->setValue(25);

    //volovar_zac
    b_SetRestriction_RadialLayout_Slider = new QSlider(Qt::Horizontal);
    b_SetRestriction_RadialLayout_Slider->setToolTip("&Change radius of Radial layout");
    b_SetRestriction_RadialLayout_Slider->setRange(0, 300);
    b_SetRestriction_RadialLayout_Slider->setSingleStep(10);
    b_SetRestriction_RadialLayout_Slider->setFocusPolicy(Qt::ClickFocus);
    b_SetRestriction_RadialLayout_Slider->setValue(100);

    b_SetAlpha_RadialLayout_Slider = new QSlider(Qt::Horizontal);
    b_SetAlpha_RadialLayout_Slider->setToolTip("&Change alpha of Radial layout");
    b_SetAlpha_RadialLayout_Slider->setRange(0, 100);
    b_SetAlpha_RadialLayout_Slider->setSingleStep(1);
    b_SetAlpha_RadialLayout_Slider->setFocusPolicy(Qt::ClickFocus);
    b_SetAlpha_RadialLayout_Slider->setValue(10);

    b_SetVisibleSpheres_RadialLayout_Slider = new QSlider(Qt::Horizontal);
    b_SetVisibleSpheres_RadialLayout_Slider->setToolTip("&Change number of visible spheres in Radial layout");
    b_SetVisibleSpheres_RadialLayout_Slider->setRange(0, 100);
    b_SetVisibleSpheres_RadialLayout_Slider->setSingleStep(1);
    b_SetVisibleSpheres_RadialLayout_Slider->setFocusPolicy(Qt::ClickFocus);
    b_SetVisibleSpheres_RadialLayout_Slider->setValue(100);


    b_SetForceScale_RadialLayout_Slider = new QSlider(Qt::Horizontal);
    b_SetForceScale_RadialLayout_Slider->setToolTip("&Change force between two nodes in Radial layout");
    b_SetForceScale_RadialLayout_Slider->setRange(1, 5000);
    b_SetForceScale_RadialLayout_Slider->setSingleStep(1);
    b_SetForceScale_RadialLayout_Slider->setFocusPolicy(Qt::ClickFocus);
    b_SetForceScale_RadialLayout_Slider->setValue(1000);

    b_SetForceSphereScale_RadialLayout_Slider = new QSlider(Qt::Horizontal);
    b_SetForceSphereScale_RadialLayout_Slider->setToolTip("&Change force between two nodes in same sphere in Radial layout");
    b_SetForceSphereScale_RadialLayout_Slider->setRange(1, 5000);
    b_SetForceSphereScale_RadialLayout_Slider->setSingleStep(1);
    b_SetForceSphereScale_RadialLayout_Slider->setFocusPolicy(Qt::ClickFocus);
    b_SetForceSphereScale_RadialLayout_Slider->setValue(1000);
    //volovar_kon

	b_SetRestriction_ConeSurface = new QPushButton();
	b_SetRestriction_ConeSurface->setIcon(QIcon("../share/3dsoftviz/img/gui/restriction_cone_surface.png"));
	b_SetRestriction_ConeSurface->setToolTip("&Set restriction - cone surface");
	b_SetRestriction_ConeSurface->setFocusPolicy(Qt::NoFocus);
	connect(b_SetRestriction_ConeSurface, SIGNAL(clicked()), this, SLOT(setRestriction_ConeSurface()));

	b_SetRestriction_ConeSurface_Slider = new QSlider(Qt::Horizontal);
	b_SetRestriction_ConeSurface_Slider->setToolTip("&Modify base radius of the restriction");
	b_SetRestriction_ConeSurface_Slider->setFocusPolicy(Qt::NoFocus);
	b_SetRestriction_ConeSurface_Slider->setValue(25);

    //volovar_zac
    b_SetRestriction_RadialLayout = new QPushButton();
    b_SetRestriction_RadialLayout->setIcon(QIcon("../share/3dsoftviz/img/gui/restriction_radial_layout.png"));
    b_SetRestriction_RadialLayout->setToolTip("&Set restriction - radial Layout");
    b_SetRestriction_RadialLayout->setFocusPolicy(Qt::NoFocus);
    connect(b_SetRestriction_RadialLayout, SIGNAL(clicked()), this, SLOT(setRestriction_RadialLayout()));

    b_drawMethod_RadialLayout = new QPushButton();
    b_drawMethod_RadialLayout->setIcon(QIcon("../share/3dsoftviz/img/gui/drawMethod_radial_layout.png"));
    b_drawMethod_RadialLayout->setToolTip("&Set restriction - radial Layout");
    b_drawMethod_RadialLayout->setFocusPolicy(Qt::NoFocus);
    connect(b_drawMethod_RadialLayout, SIGNAL(clicked()), this, SLOT(changeDrawMethod_RadialLayout()));

    b_mode_RadialLayout = new QPushButton();
    b_mode_RadialLayout->setIcon(QIcon("../share/3dsoftviz/img/gui/mode_radial_layout.png"));
    b_mode_RadialLayout->setToolTip("&Set restriction - radial Layout");
    b_mode_RadialLayout->setFocusPolicy(Qt::NoFocus);
    connect(b_mode_RadialLayout, SIGNAL(clicked()), this, SLOT(changeMode_RadialLayout()));

    //volovar_kon

	b_UnsetRestrictionFromAll = new QPushButton();
	b_UnsetRestrictionFromAll->setIcon(QIcon("../share/3dsoftviz/img/gui/restriction_unset.png"));
	b_UnsetRestrictionFromAll->setToolTip("&Unset restriction from all nodes");
	b_UnsetRestrictionFromAll->setFocusPolicy(Qt::NoFocus);
	connect(b_UnsetRestrictionFromAll, SIGNAL(clicked()), this, SLOT(unsetRestrictionFromAll()));

	b_start_server = new QPushButton();
	b_start_server->setText("Host session");
	connect(b_start_server, SIGNAL(clicked()), this, SLOT(start_server()));

	b_start_client = new QPushButton();
	b_start_client->setText("Connect to session");
	connect(b_start_client, SIGNAL(clicked()), this, SLOT(start_client()));

	b_send_message = new QPushButton();
	b_send_message->setText("Send");
	connect(b_send_message, SIGNAL(clicked()), this, SLOT(send_message()));

	chb_center = new QCheckBox("&Center");
	connect(chb_center, SIGNAL(clicked()), this, SLOT(toggleSpyWatch()));

	chb_spy = new QCheckBox("&Spy");
	connect(chb_spy, SIGNAL(clicked()), this, SLOT(toggleSpyWatch()));

	chb_attention = new QCheckBox("S&hout");
	connect(chb_attention, SIGNAL(clicked()), this, SLOT(toggleAttention()));

	le_client_name = new QLineEdit("Nick");
	le_server_addr = new QLineEdit("localhost");
	le_message= new QLineEdit("Message");

	lw_users = new QListWidget();
	lw_users->setSelectionMode(QListWidget::SingleSelection);
	lw_users->setSortingEnabled(true);
	lw_users->setMaximumHeight(200);

	sl_avatarScale = new QSlider(Qt::Vertical,this);
	sl_avatarScale->setTickPosition(QSlider::TicksAbove);
	sl_avatarScale->setRange(1,20);
	sl_avatarScale->setPageStep(1);
	sl_avatarScale->setValue(1);
	sl_avatarScale->setFocusPolicy(Qt::NoFocus);
	connect(sl_avatarScale,SIGNAL(valueChanged(int)),this,SLOT(setAvatarScale(int)));

    // Duransky start - Nastavenie widgetov

    chb_vertigo = new QCheckBox("Vertigo zoom");
    connect(chb_vertigo, SIGNAL(clicked()), this, SLOT(toggleVertigo()));

    //Add distance
    add_Distance = new QPushButton();
    add_Distance->setText("+");
    add_Distance->setToolTip("Adds distance between planes");
    add_Distance->setFocusPolicy(Qt::NoFocus);
    connect(add_Distance,SIGNAL(clicked()),this,SLOT(add_DistanceClick()));

    //Subtract distance
    subtract_Distance = new QPushButton();
    subtract_Distance->setText("-");
    subtract_Distance->setToolTip("Subtracts distance between planes");
    subtract_Distance->setFocusPolicy(Qt::NoFocus);
    connect(subtract_Distance,SIGNAL(clicked()),this,SLOT(subtract_DistanceClick()));

    //Add planes
    add_Planes = new QPushButton();
    add_Planes->setText("Add Planes");
    add_Planes->setToolTip("Adds two planes for the vertigo zoom");
    add_Planes->setFocusPolicy(Qt::NoFocus);
    connect(add_Planes,SIGNAL(clicked()),this,SLOT(add_PlanesClick()));

    //Remove planes
    remove_Planes = new QPushButton();
    remove_Planes->setText("Remove Planes");
    remove_Planes->setToolTip("Removes two planes for the vertigo zoom");
    remove_Planes->setFocusPolicy(Qt::NoFocus);
    connect(remove_Planes,SIGNAL(clicked()),this,SLOT(remove_PlanesClick()));

    //Change repulsive forces between the node on the same plane
    change_Forces = new QSpinBox;
    change_Forces->setRange(1,1000);
    change_Forces->setValue(1);
    change_Forces->setToolTip("Changes the repulsive forces between nodes restricted in one plane");
    change_Forces->setFocusPolicy(Qt::NoFocus);
    connect(change_Forces,SIGNAL(valueChanged(int)),this,SLOT(repulsive_Forces_ValueChanged()));

    // Duransky end - Nastavenie widgetov

}

void CoreWindow::createMenus()
{
	file = menuBar()->addMenu("File");
	file->addAction(load);
	file->addAction(loadGraph);
	file->addSeparator();
	file->addAction(saveGraph);
	file->addAction(saveLayout);
	file->addSeparator();
	file->addAction(quit);

	edit = menuBar()->addMenu("Edit");
	edit->addAction(options);
}

void CoreWindow::createLeftToolBar()
{
	//inicializacia comboboxu typov vyberu
	nodeTypeComboBox = new QComboBox();
	nodeTypeComboBox->insertItems(0,(QStringList() << "All" << "Node" << "Edge"));
	nodeTypeComboBox->setFocusPolicy(Qt::NoFocus);
	connect(nodeTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(nodeTypeComboBoxChanged(int)));

	toolBar = new QToolBar("Tools",this);

	QFrame * frame = createHorizontalFrame();

	frame->layout()->addWidget(noSelect);
	frame->layout()->addWidget(singleSelect);
	toolBar->addWidget(frame);

	frame = createHorizontalFrame();
	toolBar->addWidget(frame);
	frame->layout()->addWidget(multiSelect);
	frame->layout()->addWidget(center);

	toolBar->addWidget(nodeTypeComboBox);
	toolBar->addSeparator();

	frame = createHorizontalFrame();

	toolBar->addWidget(frame);
	frame->layout()->addWidget(addMeta);
	frame->layout()->addWidget(removeMeta);

	frame = createHorizontalFrame();

	toolBar->addWidget(frame);
	frame->layout()->addWidget(fix);
	frame->layout()->addWidget(unFix);

	frame = createHorizontalFrame();

	toolBar->addWidget(frame);
	frame->layout()->addWidget(merge);
	frame->layout()->addWidget(separate);

	toolBar->addWidget(label);
	toolBar->addSeparator();
	toolBar->addWidget(play);
	toolBar->addSeparator();
	toolBar->addWidget(add_Edge);
	toolBar->addWidget(add_Node);
	toolBar->addWidget(remove_all);
	toolBar->addWidget(applyColor);

	//inicializacia colorpickera
	QtColorPicker * colorPicker = new QtColorPicker();
	colorPicker->setStandardColors();
	connect(colorPicker,SIGNAL(colorChanged(const QColor &)),this,SLOT(colorPickerChanged(const QColor &)));
	toolBar->addWidget(colorPicker);

	toolBar->addWidget(applyLabel);
	toolBar->addWidget(le_applyLabel);

	toolBar->addSeparator();

	// layout restrictions
	frame = createHorizontalFrame();
	toolBar->addWidget(frame);
	frame->layout()->addWidget(b_SetRestriction_SphereSurface);
	frame->layout()->addWidget(b_SetRestriction_Sphere);

	frame = createHorizontalFrame();
	toolBar->addWidget(frame);
	frame->layout()->addWidget(b_SetRestriction_Plane);
	frame->layout()->addWidget(b_SetRestriction_SpherePlane);

	frame = createHorizontalFrame();
	toolBar->addWidget(frame);
	frame->layout()->addWidget(b_SetRestriction_Circle);
	frame->layout()->addWidget(b_SetRestriction_Cone);


	frame = createHorizontalFrame();
	toolBar->addWidget(frame);
	frame->layout()->addWidget(b_SetRestriction_ConeTree);
	frame->layout()->addWidget(b_UnsetRestriction);

    //volovar_zac
    frame = createHorizontalFrame();
    toolBar->addWidget(frame);
    frame->layout()->addWidget(b_SetRestriction_RadialLayout);
    frame->layout()->addWidget(b_drawMethod_RadialLayout);

    frame = createHorizontalFrame();
    toolBar->addWidget(frame);
    frame->layout()->addWidget(b_mode_RadialLayout);
    //volovar_kon
	toolBar->addSeparator();

	frame = createHorizontalFrame();
	toolBar->addWidget(frame);
    //volovar_zac
    frame->layout()->addWidget(b_SetRestriction_RadialLayout_Slider);
    connect(b_SetRestriction_RadialLayout_Slider,SIGNAL(valueChanged(int)),this,SLOT(RadialLayoutSizeChanged(int)));
    frame = createHorizontalFrame();
    toolBar->addWidget(frame);
    frame->layout()->addWidget(b_SetAlpha_RadialLayout_Slider);
    connect(b_SetAlpha_RadialLayout_Slider,SIGNAL(valueChanged(int)),this,SLOT(RadialLayoutAlphaChanged(int)));


    frame = createHorizontalFrame();
    toolBar->addWidget(frame);
    frame->layout()->addWidget(b_SetVisibleSpheres_RadialLayout_Slider);
    connect(b_SetVisibleSpheres_RadialLayout_Slider,SIGNAL(valueChanged(int)),this,SLOT(RadialLayoutSetVisibleSpheres(int)));

    frame = createHorizontalFrame();
    toolBar->addWidget(frame);
    frame->layout()->addWidget(b_SetForceScale_RadialLayout_Slider);
    connect(b_SetForceScale_RadialLayout_Slider,SIGNAL(valueChanged(int)),this,SLOT(RadialLayoutSetForceScale(int)));

    frame = createHorizontalFrame();
    toolBar->addWidget(frame);
    frame->layout()->addWidget(b_SetForceSphereScale_RadialLayout_Slider);
    connect(b_SetForceSphereScale_RadialLayout_Slider,SIGNAL(valueChanged(int)),this,SLOT(RadialLayoutSetForceSphereScale(int)));


    //volovar_kon
    frame = createHorizontalFrame();
    toolBar->addWidget(frame);
	frame->layout()->addWidget(b_SetRestriction_CylinderSurface);
	frame->layout()->addWidget(b_SetRestriction_CylinderSurface_Slider);

	frame = createHorizontalFrame();
	toolBar->addWidget(frame);
	frame->layout()->addWidget(b_SetRestriction_ConeSurface);
	frame->layout()->addWidget(b_SetRestriction_ConeSurface_Slider);

	frame = createHorizontalFrame();
	toolBar->addWidget(frame);
	frame->layout()->addWidget(b_UnsetRestrictionFromAll);

	toolBar->addSeparator();

	//inicializacia slideru
	slider = new QSlider(Qt::Vertical,this);
	slider->setTickPosition(QSlider::TicksAbove);
	slider->setTickInterval(5);
	slider->setValue(5);
	slider->setFocusPolicy(Qt::NoFocus);
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(sliderValueChanged(int)));

	frame = createHorizontalFrame();
	frame->setMaximumHeight(100);
	frame->layout()->setAlignment(Qt::AlignHCenter);
	toolBar->addWidget(frame);
	frame->layout()->addWidget(slider);

	addToolBar(Qt::LeftToolBarArea,toolBar);
	toolBar->setMaximumWidth(120);
	toolBar->setMovable(false);
}

void CoreWindow::createRightToolBar() {
	toolBar = new QToolBar("Network",this);

	QFrame *frame = createHorizontalFrame();
	QLabel *label = new QLabel("Nick:");
	frame->layout()->addWidget(label);
	frame->layout()->addWidget(le_client_name);

	toolBar->addWidget(frame);

	toolBar->addSeparator();
	toolBar->addWidget(b_start_server);
	toolBar->addSeparator();

	frame = createHorizontalFrame();
	label = new QLabel("Host:");
	frame->layout()->addWidget(label);
	frame->layout()->addWidget(le_server_addr);

	toolBar->addWidget(frame);

	toolBar->addWidget(b_start_client);
	toolBar->addSeparator();
	/*toolBar->addWidget(le_message);
	toolBar->addWidget(b_send_message);*/

	addToolBar(Qt::TopToolBarArea,toolBar);
}

void CoreWindow::createAugmentedRealityToolBar() {
	toolBar = new QToolBar( tr("Augmented Reality"),this);

	b_start_face = new QPushButton( tr("Start camera"));
	QLabel *label = new QLabel( tr("Face & Marker detection"));

#ifdef OPENCV_FOUND
	toolBar->addWidget( label );
	toolBar->addWidget( b_start_face );
	connect(b_start_face, SIGNAL(clicked()), this, SLOT(create_facewindow()));
	toolBar->addSeparator();
#endif

	chb_camera_rot = new QCheckBox( tr("Camera rotation"));
	chb_camera_rot->setChecked(true);
	toolBar->addWidget( chb_camera_rot );
	toolBar->addSeparator();


	// dont rotate camera if video background
	if( Util::ApplicationConfig::get()->getValue("Viewer.SkyBox.Noise").toInt() == 2 ){
		chb_camera_rot->setChecked(false);
	} else {
		chb_camera_rot->setChecked(true);
	}

#ifdef OPENCV_FOUND

#ifdef OPENNI2_FOUND
#ifdef NITE2_FOUND

	QLabel *labelKinect = new QLabel( tr("Kinect"));
	toolBar->addWidget( labelKinect );
	b_start_kinect = new QPushButton();
	b_start_kinect->setText("Start kinect");
	toolBar->addWidget( b_start_kinect );
	connect(b_start_kinect, SIGNAL(clicked()), this, SLOT(createKinectWindow()));
	toolBar->addSeparator();

	//TODO future feature - experimental state
	b_start_ransac = new QPushButton();
	b_start_ransac->setText("Start calculate surface");
	toolBar->addWidget( b_start_ransac );
	connect(b_start_ransac, SIGNAL(clicked()), this, SLOT(calculateRansac()));
	toolBar->addSeparator();

#endif
#endif
#endif

#ifdef SPEECHSDK_FOUND
	QLabel *labelSpeech = new QLabel( tr("Speech"));
	toolBar->addWidget( labelSpeech );
	b_start_speech = new QPushButton();
	b_start_speech->setText("Start Speech");
	toolBar->addWidget( b_start_speech );
	connect(b_start_speech, SIGNAL(clicked()), this, SLOT(startSpeech()));
#endif

	addToolBar(Qt::TopToolBarArea,toolBar);
	toolBar->setMovable(true);


}


void CoreWindow::createCollaborationToolBar() {
	toolBar = new QToolBar("Collaboration",this);

	QFrame *frame = createHorizontalFrame();
	QLabel *label = new QLabel("Collaborators: ");
	frame->layout()->addWidget(label);
	toolBar->addWidget(frame);

	frame = createHorizontalFrame();
	frame->layout()->addWidget(lw_users);
	toolBar->addWidget(frame);

	frame = createHorizontalFrame();
	frame->layout()->addWidget(chb_spy);
	toolBar->addWidget(frame);
	frame = createHorizontalFrame();
	frame->layout()->addWidget(chb_center);
	toolBar->addWidget(frame);

	toolBar->addSeparator();

	frame = createHorizontalFrame();
	frame->layout()->addWidget(chb_attention);
	toolBar->addWidget(frame);

	toolBar->addSeparator();

	frame = createHorizontalFrame();
	frame->layout()->addWidget(new QLabel("Avatar scale"));
	toolBar->addWidget(frame);

	frame = createHorizontalFrame();
	frame->setMaximumHeight(100);
	frame->layout()->setAlignment(Qt::AlignHCenter);
	frame->layout()->addWidget(sl_avatarScale);
	toolBar->addWidget(frame);

	// merge Duransky: this was here, but should be obsolete
	/*
	#ifdef OPENCV_FOUND
	toolBar->addSeparator();
	toolBar->addWidget(b_start_face);
	#endif
	*/

    toolBar->addSeparator();
    frame = createHorizontalFrame();
    frame->layout()->addWidget(chb_vertigo);
    toolBar->addWidget(frame);

    frame = createHorizontalFrame();
    frame->layout()->addWidget(subtract_Distance);
    frame->layout()->addWidget(add_Distance);
    toolBar->addWidget(frame);

    frame = createHorizontalFrame();
    frame->setMaximumHeight(100);
    frame->layout()->addWidget(add_Planes);
    toolBar->addWidget(frame);

    frame = createHorizontalFrame();
    frame->layout()->addWidget(remove_Planes);
    toolBar->addWidget(frame);

    frame = createHorizontalFrame();
    frame->layout()->addWidget(change_Forces);
    toolBar->addWidget(frame);


	addToolBar(Qt::RightToolBarArea,toolBar);
    toolBar->setMaximumHeight(500);
    toolBar->setMaximumWidth(120);
	toolBar->setMovable(true);
}

QFrame* CoreWindow::createHorizontalFrame()
{
	QFrame * frame = new QFrame();
	QHBoxLayout * layout = new QHBoxLayout();
	frame->setLayout(layout);
	layout->setMargin(0);
	layout->setSpacing(0);

	return frame;
}

void CoreWindow::addSQLInput()
{
	if (dock->isVisible())
	{
		dock->hide();
	}
	else
	{
		dock->show();
	}
}

void CoreWindow::showOptions()
{
	OptionsWindow *options = new OptionsWindow(coreGraph, viewerWidget);
	options->show();
}

void CoreWindow::showLoadGraph()
{
	LoadGraphWindow *loadGraph = new LoadGraphWindow(this);
    loadGraph->show();
}

void CoreWindow::saveGraphToDB()
{
	Manager::GraphManager::getInstance()->saveActiveGraphToDB();
}


void CoreWindow::saveLayoutToDB()
{
	bool ok;
	// get name for layout
	QString layout_name = QInputDialog::getText(this, tr("New layout name"),
												tr("Layout name:"),
												QLineEdit::Normal, "", &ok);


	// save layout
	if (ok && !layout_name.isEmpty()) {
		Manager::GraphManager::getInstance()->saveActiveLayoutToDB( layout_name );

	} else {
		qDebug() << "[QOSG::CoreWindow::saveLayoutToDB()] Input dialog canceled";
	}
}

void CoreWindow::sqlQuery()
{
	cout << lineEdit->text().toStdString() << endl;
}

void CoreWindow::playPause()
{
	if(isPlaying)
	{
		play->setIcon(QIcon("../share/3dsoftviz/img/gui/play.png"));
		isPlaying = 0;
		layout->pause();
		coreGraph->setNodesFreezed(true);
	}
	else
	{
		play->setIcon(QIcon("../share/3dsoftviz/img/gui/pause.png"));
		isPlaying = 1;
		coreGraph->setNodesFreezed(false);
		layout->play();
	}
}

void CoreWindow::noSelectClicked(bool checked)
{
	viewerWidget->getPickHandler()->setPickMode(Vwr::PickHandler::PickMode::NONE);
	singleSelect->setChecked(false);
	multiSelect->setChecked(false);
	center->setChecked(false);
	noSelect->setChecked(checked);
}

void CoreWindow::singleSelectClicked(bool checked)
{
	viewerWidget->getPickHandler()->setPickMode(Vwr::PickHandler::PickMode::SINGLE);
	noSelect->setChecked(false);
	multiSelect->setChecked(false);
	center->setChecked(false);
	singleSelect->setChecked(checked);
}

void CoreWindow::multiSelectClicked(bool checked)
{
	viewerWidget->getPickHandler()->setPickMode(Vwr::PickHandler::PickMode::MULTI);
	noSelect->setChecked(false);
	singleSelect->setChecked(false);
	center->setChecked(false);
	multiSelect->setChecked(checked);
}

void CoreWindow::centerView(bool checked)
{
	noSelect->setChecked(false);
	singleSelect->setChecked(false);
	multiSelect->setChecked(false);
	center->setChecked(checked);

	viewerWidget->getCameraManipulator()->setCenter(viewerWidget->getPickHandler()->getSelectionCenter(false));
}


void CoreWindow::addMetaNode()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL) {

		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

		QString metaNodeName = "metaNode";
		QString metaEdgeName = "metaEdge";

		osg::GraphicsContext * gc = viewerWidget->getCamera()->getGraphicsContext();
		osgViewer::GraphicsWindow * gw = dynamic_cast<osgViewer::GraphicsWindow *>(gc);

		int x, y, width, height;

		gw->getWindowRectangle(x, y, width, height);

		cout << width << " " << height << "\n";


		osg::ref_ptr<Data::Node> metaNode = NULL;
		QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();

		Network::Client * client = Network::Client::getInstance();
		if (!client->isConnected()) {

			QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selectedNodes->constBegin();
			metaNode = currentGraph->addNode(metaNodeName, currentGraph->getNodeMetaType(), position);
			while (i != selectedNodes->constEnd())
			{
				Data::Edge * e = currentGraph->addEdge(metaEdgeName, (*i), metaNode, currentGraph->getEdgeMetaType(), true);
				e->setCamera(viewerWidget->getCamera());
				++i;
			}
		} else {
			client->sendAddMetaNode(metaNodeName,selectedNodes,metaEdgeName,position);
		}

		Network::Server *server = Network::Server::getInstance();
		if (server->isListening() && metaNode != NULL) {
			server->sendAddMetaNode(metaNode,selectedNodes,metaEdgeName,position);
		}

		if (isPlaying)
			layout->play();
	}
}

void CoreWindow::fixNodes()
{
	viewerWidget->getPickHandler()->toggleSelectedNodesFixedState(true);
}

void CoreWindow::unFixNodes()
{
	viewerWidget->getPickHandler()->toggleSelectedNodesFixedState(false);

	if (isPlaying)
		layout->play();
}

void CoreWindow::mergeNodes()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL)
	{
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);
		QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();

		if(selectedNodes->count() > 0) {
			Network::Server * server = Network::Server::getInstance();
			Network::Client * client = Network::Client::getInstance();
			osg::ref_ptr<Data::Node> mergeNode = NULL;
			if (!client->isConnected()) {
				mergeNode = currentGraph->mergeNodes(selectedNodes, position);
			}
			if (server->isListening() && mergeNode != NULL) {
				server->sendMergeNodes(selectedNodes, position, mergeNode->getId());
			} else {
				client->sendMergeNodes(selectedNodes, position);
			}
		}
		else {
			qDebug() << "[QOSG::CoreWindow::mergeNodes] There are no nodes selected";
		}

		viewerWidget->getPickHandler()->unselectPickedEdges(0);
		viewerWidget->getPickHandler()->unselectPickedNodes(0);

		if (isPlaying)
			layout->play();
	}
}

void CoreWindow::separateNodes()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL)
	{
		QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();

		if(selectedNodes->count() > 0) {
			Network::Server * server = Network::Server::getInstance();
			Network::Client * client = Network::Client::getInstance();
			if (!client->isConnected()) {
				currentGraph->separateNodes(selectedNodes);
			}

			if (server->isListening()) {
				server->sendSeparateNodes(selectedNodes);
			} else if (client->isConnected()) {
				client->sendSeparateNodes(selectedNodes);
			}
		}
		else {
			qDebug() << "[QOSG::CoreWindow::separateNodes] There are no nodes selected";
		}

		if (isPlaying)
			layout->play();
	}
}

void CoreWindow::removeMetaNodes()
{
	QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selectedNodes->constBegin();

	while (i != selectedNodes->constEnd())
	{
		//treba este opravit - zatial kontrolujeme ci to nie je mergedNode len podla mena
		if ((*i)->getType()->isMeta() && (*i)->getName() != "mergedNode") {
			Network::Server * server = Network::Server::getInstance();
			Network::Client * client = Network::Client::getInstance();
			if (!client->isConnected()) {
				currentGraph->removeNode((*i));
			} else {
				client->sendRemoveNode((*i)->getId());
			}
			if (server->isListening()) {
				server->sendRemoveNode((*i)->getId());
			}

		}
		++i;
	}

	if (isPlaying)
		layout->play();
}

void CoreWindow::loadFile()
{

	QFileDialog dialog;
	dialog.setDirectory( "../share/3dsoftviz" );


    // Duransky start - vynulovanie vertigo rovin pri nacitani noveho grafu
    planes_Vertigo.clear();
    numberOfPlanes = 0;
    // Duransky end - vynulovanie vertigo rovin pri nacitani noveho grafu

	//treba overit
	layout->pause();
	coreGraph->setNodesFreezed(true);

	QString fileName =NULL;

	if(dialog.exec()){
		QStringList filenames = dialog.selectedFiles();
		fileName = filenames.at(0);
	}

	if (fileName != NULL) {
		Manager::GraphManager::getInstance()->loadGraph(fileName);

		viewerWidget->getCameraManipulator()->home();
	}

	//treba overit ci funguje
	if (isPlaying)
	{
		layout->play();
		coreGraph->setNodesFreezed(false);
	}


}

void CoreWindow::labelOnOff(bool)
{
	if (viewerWidget->getPickHandler()->getSelectionType() == Vwr::PickHandler::SelectionType::EDGE)
	{
		edgeLabelsVisible = !edgeLabelsVisible;
		coreGraph->setEdgeLabelsVisible(edgeLabelsVisible);
	}
	else if (viewerWidget->getPickHandler()->getSelectionType() == Vwr::PickHandler::SelectionType::NODE)
	{
		nodeLabelsVisible = !nodeLabelsVisible;
		coreGraph->setNodeLabelsVisible(nodeLabelsVisible);
	}
	else
	{
		bool state = edgeLabelsVisible & nodeLabelsVisible;

		nodeLabelsVisible = edgeLabelsVisible = !state;

		coreGraph->setEdgeLabelsVisible(!state);
		coreGraph->setNodeLabelsVisible(!state);
	}
}

void CoreWindow::sliderValueChanged(int value)
{
	layout->setAlphaValue((float)value * 0.001f);
}

//Volovar zac

void CoreWindow::RadialLayoutSizeChanged(int value)
{
    //notify radial layout that size was changed
    Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
    if (selectedRadialLayout != NULL)
        selectedRadialLayout->changeSize((float) value);
}

void CoreWindow::RadialLayoutAlphaChanged(int value)
{
    //notify radial layout that alpha channel was changed
    Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
    //qDebug()<<"Value: "<<value<<", selected: "<<selectedRadialLayout;
    if (selectedRadialLayout != NULL)
        selectedRadialLayout->setAlpha((float) value/500.0f);
}

void CoreWindow::RadialLayoutSetVisibleSpheres(int value)
{
    //notify radial layout that number of visibles spheres was changed
    Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
    if (selectedRadialLayout != NULL)
        selectedRadialLayout->setVisibleSpheres((float) value/100);
}

void CoreWindow::RadialLayoutSetForceScale(int value)
{
    //notify that multiplier of repulsive forces in radial layout was changed
    Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
    if (selectedRadialLayout != NULL)
        selectedRadialLayout->setForceScale((float) value);
}

void CoreWindow::RadialLayoutSetForceSphereScale(int value)
{
    //notify that multiplier of repulsive forces in radial layout on same layer was changed
    Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
    if (selectedRadialLayout != NULL)
        selectedRadialLayout->setForceSphereScale((float) value);
}

void CoreWindow::changeDrawMethod_RadialLayout()
{
    //notify that drawing method (WIREFRAME/SOLID) was changed
    Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
    if (selectedRadialLayout != NULL)
        selectedRadialLayout->changeRenderType();
}

void CoreWindow::changeMode_RadialLayout()
{
    //notify that mode of radial layout was changed (2D/3D)
    Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
    if (selectedRadialLayout != NULL)
        selectedRadialLayout->changeMode();
}

//Volovar koniec



void CoreWindow::colorPickerChanged(const QColor & color)
{
	this->color = color;
}

void CoreWindow::nodeTypeComboBoxChanged(int index)
{
	switch(index)
	{
	case 0:
		viewerWidget->getPickHandler()->setSelectionType(Vwr::PickHandler::SelectionType::ALL);
		label->setChecked(edgeLabelsVisible & nodeLabelsVisible);
		break;
	case 1:
		viewerWidget->getPickHandler()->setSelectionType(Vwr::PickHandler::SelectionType::NODE);
		label->setChecked(nodeLabelsVisible);
		break;
	case 2:
		viewerWidget->getPickHandler()->setSelectionType(Vwr::PickHandler::SelectionType::EDGE);
		label->setChecked(edgeLabelsVisible);
		break;
	default:
		qDebug() << "CoreWindow:nodeTypeComboBoxChanged do not suported index";
		break;

	}
}

void CoreWindow::applyColorClick()
{
	float alpha = (float)color.alphaF();
	float red = (float)color.redF();
	float green = (float)color.greenF();
	float blue = (float)color.blueF();

	QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator ni = selectedNodes->constBegin();

	Network::Server * server = Network::Server::getInstance();
	while (ni != selectedNodes->constEnd())
	{
		if (client->isConnected()) {
			client->sendNodeColor((*ni)->getId(), red, green, blue, alpha);
		} else {
			(*ni)->setColor(osg::Vec4(red, green, blue, alpha));
			server->sendNodeColor((*ni)->getId(), red, green, blue, alpha);
		}
		++ni;
	}

	QLinkedList<osg::ref_ptr<Data::Edge> > * selectedEdges = viewerWidget->getPickHandler()->getSelectedEdges();
	QLinkedList<osg::ref_ptr<Data::Edge> >::const_iterator ei = selectedEdges->constBegin();

	while (ei != selectedEdges->constEnd())
	{
		//ak je edge skryta, nebudeme jej menit farbu
		//(*ei)->getScale() != 0
		if(!(*ei)->getIsInvisible())
		{

			if (client->isConnected()) {
				client->sendNodeColor((*ei)->getId(), red, green, blue, alpha);
			} else {
				(*ei)->setEdgeColor(osg::Vec4(red, green, blue, alpha));
				server->sendNodeColor((*ei)->getId(), red, green, blue, alpha);
			}

		}
		++ei;
	}
}

void CoreWindow::applyLabelClick() {

	Network::Server * server = Network::Server::getInstance();

	QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator ni = selectedNodes->constBegin();
	QString newLabel = le_applyLabel->text();

	while (ni != selectedNodes->constEnd()) {
		if (client->isConnected()) {
			client->sendNodeLabel((*ni)->getId(), newLabel);
		} else {
			(*ni)->setName(newLabel);
			(*ni)->setLabelText(newLabel);
			(*ni)->reloadConfig();
			server->sendNodeLabel((*ni)->getId(), newLabel);
		}
		++ni;
	}
}

void CoreWindow::setRestriction_CylinderSurface()
{
	Data::Graph *currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL)
	{
		//osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

		//osg::ref_ptr<Data::Node> centerNode = currentGraph->addRestrictionNode (QString ("center"), position);
		//osg::ref_ptr<Data::Node> surfaceNode = currentGraph->addRestrictionNode (QString ("surface"), position + osg::Vec3f (10, 0, 0));

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;
		//restrictionNodes.push_back (centerNode);
		//restrictionNodes.push_back (surfaceNode);

		setRestrictionToAllNodes (
					QSharedPointer<Layout::ShapeGetter> (
						new Layout::ShapeGetter_CylinderSurface_ByCamera(viewerWidget, *b_SetRestriction_CylinderSurface_Slider)),
					currentGraph,
					QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
						new Layout::RestrictionRemovalHandler_RestrictionNodesRemover (
							*currentGraph,
							restrictionNodes
							)
						)
					);
	}
}

void CoreWindow::setRestriction_SphereSurface ()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL)
	{
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);
        if( qFuzzyCompare((float)position.length(),0.0f) ) return;
		osg::ref_ptr<Data::Node> centerNode;
		osg::ref_ptr<Data::Node> surfaceNode;

		QString name_centerNode = "center";
		QString name_sufraceNode = "surface";
		osg::Vec3 positionNode1 = position;
		osg::Vec3 positionNode2 = position + osg::Vec3f (10, 0, 0);

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;
		Network::Client * client = Network::Client::getInstance();

		if (!client->isConnected()) {

			centerNode = currentGraph->addRestrictionNode (name_centerNode, positionNode1);
			surfaceNode = currentGraph->addRestrictionNode (name_sufraceNode, positionNode2);

			restrictionNodes.push_back (centerNode);
			restrictionNodes.push_back (surfaceNode);

			setRestrictionToSelectedNodes (
						QSharedPointer<Layout::ShapeGetter> (
							new Layout::ShapeGetter_SphereSurface_ByTwoNodes (centerNode, surfaceNode)
							),
						currentGraph,
						QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
							new Layout::RestrictionRemovalHandler_RestrictionNodesRemover (
								*currentGraph,
								restrictionNodes
								)
							)
						);
		} else {
			client->sendSetRestriction(1,name_centerNode,positionNode1,name_sufraceNode,positionNode2, viewerWidget->getPickHandler()->getSelectedNodes());
		}

		Network::Server * server = Network::Server::getInstance();
		server->sendSetRestriction(1, centerNode, positionNode1, surfaceNode, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes());
	}
}

void CoreWindow::setRestriction_Sphere ()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL)
	{
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);
		osg::ref_ptr<Data::Node> centerNode;
		osg::ref_ptr<Data::Node> surfaceNode;

		QString name_centerNode = "center";
		QString name_sufraceNode = "surface";
		osg::Vec3 positionNode1 = position;
        osg::Vec3 positionNode2 = position + osg::Vec3f (10, 0, 0);

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;

		Network::Client * client = Network::Client::getInstance();

		if (!client->isConnected()) {

			centerNode = currentGraph->addRestrictionNode (name_centerNode, positionNode1);
			surfaceNode = currentGraph->addRestrictionNode (name_sufraceNode, positionNode2);

			restrictionNodes.push_back (centerNode);
			restrictionNodes.push_back (surfaceNode);

			setRestrictionToSelectedNodes (
						QSharedPointer<Layout::ShapeGetter> (
							new Layout::ShapeGetter_Sphere_ByTwoNodes (centerNode, surfaceNode)
							),
						currentGraph,
						QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
							new Layout::RestrictionRemovalHandler_RestrictionNodesRemover (
								*currentGraph,
								restrictionNodes
								)
							)
						);
		} else {
			client->sendSetRestriction(2,name_centerNode,positionNode1,name_sufraceNode, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes());
		}
		Network::Server * server = Network::Server::getInstance();
		server->sendSetRestriction(2, centerNode, positionNode1, surfaceNode, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes());
	}
}

void CoreWindow::setRestriction_ConeSurface()
{
	Data::Graph *currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL)
	{
		//osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

		//osg::ref_ptr<Data::Node> centerNode = currentGraph->addRestrictionNode (QString ("center"), position);
		//osg::ref_ptr<Data::Node> surfaceNode = currentGraph->addRestrictionNode (QString ("surface"), position + osg::Vec3f (10, 0, 0));

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;
		//restrictionNodes.push_back (centerNode);
		//restrictionNodes.push_back (surfaceNode);

		setRestrictionToAllNodes (
					QSharedPointer<Layout::ShapeGetter> (
						new Layout::ShapeGetter_ConeSurface_ByCamera(viewerWidget, *b_SetRestriction_ConeSurface_Slider)),
					currentGraph,
					QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
						new Layout::RestrictionRemovalHandler_RestrictionNodesRemover (
							*currentGraph,
							restrictionNodes
							)
						)
					);
	}
}

// Duransky start - Upravena funkcia na vytvorenie obmedzenia - roviny pre ucely vertigo zoomu
Layout::ShapeGetter_Plane_ByThreeNodes* CoreWindow::setRestriction_Plane_Vertigo (QLinkedList<osg::ref_ptr<Data::Node> > * nodesToRestrict, int nOfPlane)
{
    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

    if (currentGraph != NULL) {

        osg::ref_ptr<Data::Node> node1;
        osg::ref_ptr<Data::Node> node2;
        osg::ref_ptr<Data::Node> node3;

        QString name_node1 = "plane_node_1";
        QString name_node2 = "plane_node_2";
        QString name_node3 = "plane_node_3";

        osg::Vec3 rootPosition = osg::Vec3f (0.f, 0.f, 0.f);

        // pozicia bodu zavisi od poradoveho cisla roviny
        osg::Vec3 position = rootPosition + osg::Vec3f ((float)(nOfPlane - 1) * (float)vertigoPlanesDistance, 0.f, 0.f);

        osg::Vec3 positionNode1 = position + osg::Vec3f (0.f, -100.f, 0.f);
        osg::Vec3 positionNode2 = position + osg::Vec3f (0.f, 100.f, 200.f);
        osg::Vec3 positionNode3 = position + osg::Vec3f (0.f, 100.f, -200.f);

        Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;

        Network::Client * client = Network::Client::getInstance();

        if (!client->isConnected()) {

            node1 = currentGraph->addRestrictionNode (name_node1, positionNode1);
            node2 = currentGraph->addRestrictionNode (name_node2, positionNode2);
            node3 = currentGraph->addRestrictionNode (name_node3, positionNode3);
            restrictionNodes.push_back (node1);
            restrictionNodes.push_back (node2);
            restrictionNodes.push_back (node3);

            Layout::ShapeGetter_Plane_ByThreeNodes* plane = new Layout::ShapeGetter_Plane_ByThreeNodes (node1, node2, node3);

            setRestrictionToSelectedNodes (
                        QSharedPointer<Layout::ShapeGetter> (plane),currentGraph,
                        QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
                            new Layout::RestrictionRemovalHandler_RestrictionNodesRemover (
                                *currentGraph,restrictionNodes
                                )
                            ),
                        nodesToRestrict
                        );
            return plane;
        } else {
            // Tato cast pre sietovu komunikaciu nebola testovana
            client->sendSetRestriction(3,name_node1,positionNode1,name_node2, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes(),name_node3,&positionNode3);
        }
        Network::Server * server = Network::Server::getInstance();
        server->sendSetRestriction(3, node1, positionNode1, node2, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes(), node3, &positionNode3);
        return NULL;
    }
    return NULL;
}
// Duransky end - Upravena funkcia na vytvorenie obmedzenia - roviny pre ucely vertigo zoomu

void CoreWindow::setRestriction_Plane (QLinkedList<osg::ref_ptr<Data::Node> > * nodesToRestrict)
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL) {
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

		osg::ref_ptr<Data::Node> node1;
		osg::ref_ptr<Data::Node> node2;
		osg::ref_ptr<Data::Node> node3;

		QString name_node1 = "plane_node_1";
		QString name_node2 = "plane_node_2";
		QString name_node3 = "plane_node_3";

		osg::Vec3 positionNode1 = position;
		osg::Vec3 positionNode2 = position + osg::Vec3f (10, 0, 0);
		osg::Vec3 positionNode3 = position + osg::Vec3f (0, 10, 0);

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;

		Network::Client * client = Network::Client::getInstance();

		if (!client->isConnected()) {

			node1 = currentGraph->addRestrictionNode (name_node1, positionNode1);
			node2 = currentGraph->addRestrictionNode (name_node2, positionNode2);
			node3 = currentGraph->addRestrictionNode (name_node3, positionNode3);
			restrictionNodes.push_back (node1);
			restrictionNodes.push_back (node2);
			restrictionNodes.push_back (node3);

			setRestrictionToSelectedNodes (
						QSharedPointer<Layout::ShapeGetter> (
							new Layout::ShapeGetter_Plane_ByThreeNodes (node1, node2, node3)
							),
						currentGraph,
						QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
							new Layout::RestrictionRemovalHandler_RestrictionNodesRemover (
								*currentGraph,
								restrictionNodes
								)
							),
						nodesToRestrict
						);
		} else {
			client->sendSetRestriction(3,name_node1,positionNode1,name_node2, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes(),name_node3,&positionNode3);
		}
		Network::Server * server = Network::Server::getInstance();
		server->sendSetRestriction(3, node1, positionNode1, node2, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes(), node3, &positionNode3);
	}
}

void CoreWindow::setRestriction_SpherePlane(QLinkedList<osg::ref_ptr<Data::Node> > * nodesToRestrict){
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL) {

		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

		osg::ref_ptr<Data::Node> node1;
		osg::ref_ptr<Data::Node> node2;
		osg::ref_ptr<Data::Node> node3;

		QString name_node1 = "sphere_center_node";
		QString name_node2 = "sphere_surface_node";
		QString name_node3 = "plane_node_3";

		osg::Vec3 positionNode1 = position;
		osg::Vec3 positionNode2 = position + osg::Vec3f (10, 0, 0);
		osg::Vec3 positionNode3 = position + osg::Vec3f (0, 10, 0);

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;

		Network::Client * client = Network::Client::getInstance();

		if (!client->isConnected()) {

			node1 = currentGraph->addRestrictionNode (name_node1, positionNode1);
			node2 = currentGraph->addRestrictionNode (name_node2, positionNode2);
			node3 = currentGraph->addRestrictionNode (name_node3, positionNode3);
			restrictionNodes.push_back (node1);
			restrictionNodes.push_back (node2);
			restrictionNodes.push_back (node3);



			setRestrictionToSelectedNodes (
						QSharedPointer<Layout::ShapeGetter> (
							new Layout::ShapeGetter_SpherePlane_ByThreeNodes (node1, node2, node3)
							),
						currentGraph,
						QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
							new Layout::RestrictionRemovalHandler_RestrictionNodesRemover (
								*currentGraph,
								restrictionNodes
								)
							),
						nodesToRestrict
						);
		} else {
			client->sendSetRestriction(3,name_node1,positionNode1,name_node2, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes(),name_node3,&positionNode3);
		}
		Network::Server * server = Network::Server::getInstance();
		server->sendSetRestriction(3, node1, positionNode1, node2, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes(), node3, &positionNode3);
	}
}

void CoreWindow::setRestriction_Circle(QLinkedList<osg::ref_ptr<Data::Node> > * nodesToRestrict,
									   osg::ref_ptr<Data::Node> centerNode){
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL) {

		if (nodesToRestrict == NULL){
			nodesToRestrict = viewerWidget->getPickHandler()->getSelectedNodes();
		}

		osg::Vec3 position;
		if (centerNode == NULL)
			position = viewerWidget->getPickHandler()->getSelectionCenter(true);
		else
			position = centerNode->getTargetPosition();

		osg::ref_ptr<Data::Node> node1;
		osg::ref_ptr<Data::Node> node2;
		osg::ref_ptr<Data::Node> node3;

		QString name_node1 = "circle_center_node";
		QString name_node2 = "circle_edge_node";
		QString name_node3 = "circle_plane_node";

		osg::Vec3 positionNode1 = position;
		osg::Vec3 positionNode2 = position + osg::Vec3f (10, 0, 0);
		osg::Vec3 positionNode3 = position + osg::Vec3f (0, 20, 0);

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;

		Network::Client * client = Network::Client::getInstance();

		if (!client->isConnected()) {

			if (centerNode == NULL)
				node1 = currentGraph->addRestrictionNode (name_node1, positionNode1);
			else
				node1 = centerNode;
			node2 = currentGraph->addRestrictionNode (name_node2, positionNode2);
			node3 = currentGraph->addRestrictionNode (name_node3, positionNode3);
			node3->setInvisible();
			restrictionNodes.push_back (node1);
			restrictionNodes.push_back (node2);
			restrictionNodes.push_back (node3);

			QLinkedList<osg::ref_ptr<Data::Node> > nodes;
			nodes.append(node1);
			nodes.append(node2);
			nodes.append(node3);

			osg::ref_ptr<Data::Edge> radiusEdge = currentGraph->addEdge("pomEdge", node1, node2, currentGraph->getEdgeMetaType(), true);
			radiusEdge->setScale(0);
			radiusEdge->setSharedCoordinates(false, false, true);

			setRestrictionToSelectedNodes (
						QSharedPointer<Layout::ShapeGetter> (
							new Layout::ShapeGetter_Circle_ByThreeNodes (node1, node2, node3)
							),
						currentGraph,
						QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
							new Layout::RestrictionRemovalHandler_RestrictionNodesRemover (
								*currentGraph,
								restrictionNodes
								)
							),
						nodesToRestrict
						);
		} else {
			client->sendSetRestriction(3,name_node1,positionNode1,name_node2, positionNode2, nodesToRestrict,name_node3,&positionNode3);
		}
		Network::Server * server = Network::Server::getInstance();
		server->sendSetRestriction(3, node1, positionNode1, node2, positionNode2,nodesToRestrict, node3, &positionNode3);
	}
}

void CoreWindow::setRestriction_Cone(QLinkedList<osg::ref_ptr<Data::Node> > * nodesToRestrict,
									 osg::ref_ptr<Data::Node> parentNode){
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL) {
		if (parentNode == NULL){
			parentNode =  viewerWidget->getPickHandler()->getPickedNodeWithMaxEdgeCount();
			parentNode->setFixed(true);
		}

		if (nodesToRestrict == NULL ){
			nodesToRestrict = viewerWidget->getPickHandler()->getSelectedNodes();
		}

		nodesToRestrict->removeOne(parentNode);


		osg::Vec3 positionCenter = parentNode->getTargetPosition() + osg::Vec3f (0, 0, (-50));

		osg::ref_ptr<Data::Node> centernode = currentGraph->addRestrictionNode ("circle_center_node", positionCenter);

		osg::ref_ptr<Data::Edge> parentCircleEdge = currentGraph->addEdge("pomEdge", parentNode, centernode, currentGraph->getEdgeMetaType(), true);
		parentCircleEdge->setInvisible(true);
		parentCircleEdge->setSharedCoordinates(true, true, false);

		setRestriction_Circle(nodesToRestrict, centernode);
	}

}


void CoreWindow::setRestriction_ConeTree (){
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	if (currentGraph == NULL) return;
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();

	osg::ref_ptr<Data::Node> rootNode = viewerWidget->getPickHandler()->getPickedNodeWithMaxEdgeCount();
	if(rootNode == NULL) return;

	osg::Vec3 centerPosition = viewerWidget->getPickHandler()->getSelectionCenter(true);
	osg::Vec3 rootPosition = centerPosition + osg::Vec3f (0, 0, 100);
	rootNode->setTargetPosition(rootPosition);
	rootNode->setFixed(true);

	Data::GraphSpanningTree* spanningTree = currentGraph->getSpanningTree(rootNode->getId());
	QLinkedList<osg::ref_ptr<Data::Node> > pickedNodes;

	QList<qlonglong> groups = spanningTree->getAllGroups();
	QList<qlonglong>::iterator groupIt;
	for(groupIt=groups.begin(); groupIt!=groups.end();groupIt++){
		if ((*groupIt) == 0) continue;
		pickedNodes.clear();
		QList<qlonglong> nodes = spanningTree->getNodesInGroup(*groupIt);
		QList<qlonglong>::iterator nodeIt;
		for(nodeIt=nodes.begin(); nodeIt!=nodes.end();nodeIt++){
			pickedNodes.append(allNodes->value(*nodeIt));
		}
		osg::ref_ptr<Data::Node> parentNode = allNodes->value(*groupIt);
		setRestriction_Cone(&pickedNodes,parentNode);
	}

	int maxDepth = spanningTree->getMaxDepth();
	for (int depth=1; depth<=maxDepth;depth++){
		pickedNodes.clear();
		QList<qlonglong> groups = spanningTree->getGroupsInDepth(depth);

		QList<qlonglong>::iterator groupIt;
		for(groupIt=groups.begin(); groupIt!=groups.end();groupIt++){
			qlonglong nodeId = spanningTree->getRandomNodeInGroup(*groupIt);
			pickedNodes.append(allNodes->value(nodeId));

		}

		osg::Vec3 position = rootPosition + osg::Vec3f (0.f, 0.f, (-50.f) * (float)depth);
		osg::Vec3 positionNode1 = position;
		osg::Vec3 positionNode2 = position + osg::Vec3f (10.f, 0.f, 0.f);
		osg::Vec3 positionNode3 = position + osg::Vec3f (0.f, 10.f, 0.f);

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;

		osg::ref_ptr<Data::Node> node1 = currentGraph->addRestrictionNode ("plane_node_1", positionNode1);
		osg::ref_ptr<Data::Node> node2 = currentGraph->addRestrictionNode ("plane_node_2", positionNode2);
		osg::ref_ptr<Data::Node> node3 = currentGraph->addRestrictionNode ("plane_node_3", positionNode3);
		node1->setInvisible();
		node2->setInvisible();
		node3->setInvisible();
		restrictionNodes.push_back (node1);
		restrictionNodes.push_back (node2);
		restrictionNodes.push_back (node3);

		setRestrictionToShape(QSharedPointer<Layout::ShapeGetter> (
								  new Layout::ShapeGetter_Plane_ByThreeNodes (node1, node2, node3)
								  ),
							  currentGraph,
							  QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
								  new Layout::RestrictionRemovalHandler_RestrictionNodesRemover (
									  *currentGraph,
									  restrictionNodes
									  )
								  ),
							  pickedNodes);

	}
}

void CoreWindow::unsetRestriction () {
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	Network::Client * client = Network::Client::getInstance();
	if (!client->isConnected()) {
		if (currentGraph != NULL) {
			setRestrictionToSelectedNodes (
						QSharedPointer<Layout::ShapeGetter> (NULL),
						currentGraph,
						QSharedPointer<Layout::RestrictionRemovalHandler> (NULL)
						);
		}
	} else {
		client->sendUnSetRestriction(viewerWidget->getPickHandler()->getSelectedNodes());
	}

	Network::Server * server = Network::Server::getInstance();
	server->sendUnSetRestriction(viewerWidget->getPickHandler()->getSelectedNodes());
}

void CoreWindow::unsetRestrictionFromAll() {
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL) {
		setRestrictionToAllNodes (
					QSharedPointer<Layout::ShapeGetter> (NULL),
					currentGraph,
					QSharedPointer<Layout::RestrictionRemovalHandler> (NULL)
					);
        //volovar_zac
        Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
        if (selectedRadialLayout != NULL)
            selectedRadialLayout->unselectNodes();
        //volovar_kon
	}
}

void CoreWindow::setRestrictionToSelectedNodes (
		QSharedPointer<Layout::ShapeGetter> shapeGetter,
		Data::Graph * currentGraph,
		QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler,
		QLinkedList<osg::ref_ptr<Data::Node> > * nodesToRestrict
		) {
	QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = (nodesToRestrict == NULL) ? viewerWidget->getPickHandler()->getSelectedNodes() : nodesToRestrict;

	QSet<Data::Node *> nodes;
	for (QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator it = selectedNodes->constBegin (); it != selectedNodes->constEnd (); ++it) {
		nodes.insert (it->get ());
	}

	currentGraph->getRestrictionsManager().setRestrictions (nodes, shapeGetter);

	if ((! shapeGetter.isNull ()) && (! removalHandler.isNull ())) {
		currentGraph->getRestrictionsManager().setOrRunRestrictionRemovalHandler (shapeGetter, removalHandler);
	}

	if (isPlaying)
		layout->play();
}

void CoreWindow::setRestrictionToAllNodes (
		QSharedPointer<Layout::ShapeGetter> shapeGetter,
		Data::Graph * currentGraph,
		QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler
		) {
	QSet<Data::Node *> nodes;

	QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
	j = Manager::GraphManager::getInstance()->getActiveGraph()->getNodes()->begin();
	for (int i = 0; i < Manager::GraphManager::getInstance()->getActiveGraph()->getNodes()->count(); ++i,++j)
	{
		nodes.insert(j.value());
	}

	currentGraph->getRestrictionsManager ().setRestrictions (nodes, shapeGetter);

	if ((! shapeGetter.isNull ()) && (! removalHandler.isNull ())) {
		currentGraph->getRestrictionsManager ().setOrRunRestrictionRemovalHandler (shapeGetter, removalHandler);
	}

	if (isPlaying)
		layout->play();
}

/**Volovar zaciatok
 */
void CoreWindow::setRestriction_RadialLayout()
{
    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
    QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();
    osg::ref_ptr<Data::Node> rootNode = viewerWidget->getPickHandler()->getPickedNodeWithMinEdgeCount();
     osg::Vec3 rootPosition = viewerWidget->getPickHandler()->getSelectionCenter(true);
    if (currentGraph == NULL)
        return;
    Layout::RadialLayout *radialLayout = new Layout::RadialLayout(currentGraph, selectedNodes, 100, rootNode, rootPosition);
    radialLayout->select();
    viewerWidget->getCameraManipulator()->setCenter(rootPosition);
}
/*Volovar koniec
 */

void CoreWindow::setRestrictionToShape(
		QSharedPointer<Layout::ShapeGetter> shapeGetter,
		Data::Graph * currentGraph,
		QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler,
		QLinkedList<osg::ref_ptr<Data::Node> >  nodesOfShapeGettersToRestrict)
{

	currentGraph->getRestrictionsManager().setRestrictionToShape (&nodesOfShapeGettersToRestrict, shapeGetter);


	if ((! shapeGetter.isNull ()) && (! removalHandler.isNull ())) {
		currentGraph->getRestrictionsManager().setOrRunRestrictionRemovalHandler (shapeGetter, removalHandler);
	}

	if (isPlaying)
		layout->play();
}

bool CoreWindow::add_EdgeClick()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator ni = selectedNodes->constBegin();

	if (
			selectedNodes==NULL
			) {
		AppCore::Core::getInstance()->messageWindows->showMessageBox("Upozornenie","Ziadny uzol oznaceny",false);
		return false;
	}

	osg::ref_ptr<Data::Node> node1, node2;
	int i=0;

	while (ni != selectedNodes->constEnd())
	{
		osg::ref_ptr<Data::Node> existingNode = (* ni);
		++ni;i++;
	}
	if (
			i!=2
			) {
		AppCore::Core::getInstance()->messageWindows->showMessageBox("Upozornenie","Musite vybrat prave 2 vrcholy",false);
		return false;
	}
	ni = selectedNodes->constBegin();
	node2=(* ni);
	++ni;
	node1=(* ni);
	++ni;
	QMap<qlonglong, osg::ref_ptr<Data::Edge> > *mapa = currentGraph->getEdges();
	Data::Type* type = currentGraph->addType(Data::GraphLayout::META_EDGE_TYPE);
	for (QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator it = mapa->begin (); it != mapa->end (); ++it) {
		osg::ref_ptr<Data::Edge> existingEdge = it.value ();
		if (
				existingEdge->getSrcNode () ->getId () == node1 ->getId () &&
				existingEdge->getDstNode () ->getId () == node2 ->getId ()
				) {
			AppCore::Core::getInstance()->messageWindows->showMessageBox("Hrana najdena","Medzi vrcholmi nesmie byt hrana",false);
			return false;
		}
		if (
				existingEdge->getSrcNode () ->getId () == node2 ->getId () &&
				existingEdge->getDstNode () ->getId () == node1 ->getId ()
				) {
			AppCore::Core::getInstance()->messageWindows->showMessageBox("Hrana najdena","Medzi vrcholmi nesmie byt hrana",false);
			return false;
		}
	}

	osg::ref_ptr<Data::Edge> newEdge;
	if (!client->isConnected()) {
		newEdge = currentGraph->addEdge("GUI_edge", node1, node2, type, false);
		Network::Server * server = Network::Server::getInstance();
		server->sendNewEdge(newEdge);
	} else {
		client->sendNewEdge("GUI_edge", node1->getId(), node2->getId(), false);
	}
	if (isPlaying)
		layout->play();

	//QString nodename1 = QString(node1->getName());
	//QString nodename2 = QString(node2->getName());
	return true;
	//context.getGraph ().addEdge (QString (""), node1[1], node1[2], edgeType, true);

}

bool CoreWindow::add_NodeClick()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	Data::Type *edgeType = NULL;
	Data::Type *nodeType = NULL;

	if (currentGraph == NULL) {
		currentGraph= Manager::GraphManager::getInstance()->createNewGraph("NewGraph");
	}
	Importer::GraphOperations * operations = new Importer::GraphOperations(*currentGraph);
	operations->addDefaultTypes(edgeType, nodeType);

	osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

	osg::ref_ptr<Data::Node> newNode;
	if (!client->isConnected()) {
		newNode = currentGraph->addNode("newNode", nodeType , position);
		Network::Server * server = Network::Server::getInstance();
		server->sendNewNode(newNode);
	} else {
		client->sendNewNode("newNode", position);
	}

	if (isPlaying)
		layout->play();

	return true;
}

bool CoreWindow::removeClick()
{
	Network::Server * server = Network::Server::getInstance();

	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QLinkedList<osg::ref_ptr<Data::Edge> > * selectedEdges = viewerWidget->getPickHandler()->getSelectedEdges();

	while (selectedEdges->size () > 0) {
		osg::ref_ptr<Data::Edge> existingEdge1 = (* (selectedEdges->constBegin()));
		if (!client->isConnected()) {
			currentGraph->removeEdge(existingEdge1);
			server->sendRemoveEdge(existingEdge1->getId());
		} else {
			client->sendRemoveEdge(existingEdge1->getId());
		}
		selectedEdges->removeFirst ();
	}
	currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();

	while (selectedNodes->size () > 0) {
		osg::ref_ptr<Data::Node> existingNode1 = (* (selectedNodes->constBegin()));
		if (existingNode1->isRemovableByUser()) {
			if (!client->isConnected()) {
				currentGraph->removeNode(existingNode1);
				server->sendRemoveNode(existingNode1->getId());
			} else {
				client->sendRemoveNode(existingNode1->getId());
			}
		}
		selectedNodes->removeFirst ();
	}

	int NodesCount=currentGraph->getNodes()->size();
	cout<<NodesCount;
	if (isPlaying)
		layout->play();

	return true;
}

void CoreWindow::start_server()
{
	Network::Server *server = Network::Server::getInstance();

	if (!server -> isListening()) {

		server->setLayoutThread(layout);
		server->setCoreGraph(coreGraph);
		bool success = server->listen(QHostAddress::Any, 4200);
		if(!success) {
			qDebug() << "Could not listen on port 4200.";
		} else {
			le_client_name->setEnabled(false);
			le_server_addr->setEnabled(false);
			b_start_client->setEnabled(false);
			b_start_server->setText("End session");
		}

		qDebug() << "Server started";
	} else {
		server->stopServer();
		le_client_name->setEnabled(true);
		le_server_addr->setEnabled(true);
		b_start_client->setEnabled(true);
		b_start_server->setText("Host session");
		qDebug() << "Server stopped";
	}
}

void CoreWindow::start_client()
{
	if (!client -> isConnected()) {
		client -> setLayoutThread(layout);
		client -> setCoreGraph(coreGraph);
		b_start_client -> setText("Connecting...");
		b_start_client -> setEnabled(false);
		b_start_server -> setEnabled(false);
		client -> ServerConnect(le_client_name->text(), le_server_addr->text());
	} else {
		client -> disconnect();
	}
}


void CoreWindow::send_message()
{
	client->send_message(le_message->text());
}


void CoreWindow::create_facewindow()
{
#ifdef OPENCV_FOUND
	OpenCV::OpenCVCore::getInstance(NULL, this)->faceRecognition();
#endif
}


#ifdef OPENCV_FOUND
#ifdef OPENNI2_FOUND
#ifdef NITE2_FOUND
void CoreWindow::createKinectWindow(){

	Kinect::KinectCore::getInstance(NULL,this)->kinectRecognition();
}

void CoreWindow::calculateRansac()
{
	Kinect::Ransac *ransac= new Kinect::Ransac();
	ransac->calculate();
}

#endif
#endif
#endif

#ifdef SPEECHSDK_FOUND
void CoreWindow::startSpeech()
{
	if (this->mSpeechThr!=NULL && (b_start_speech->text()=="Stop Speech"))
	{
		this->mSpeechThr->cancel=true;
		if(!this->mSpeechThr->wait(5000))
		{
			this->mSpeechThr->terminate();
			this->mSpeechThr->wait();
		}
		delete(this->mSpeechThr);
		b_start_speech->setText("Start Speech");
		this->mSpeechThr=NULL;
		return;
	}
	this->mSpeechThr = new Speech::KinectSpeechThread();
	CoUninitialize();
	if (this->mSpeechThr->initializeSpeech()==1)
	{
		delete(this->mSpeechThr);
		this->mSpeechThr=NULL;
		return;
	}
	this->mSpeechThr->start();
	b_start_speech->setText("Stop Speech");
}
#endif



void CoreWindow::toggleSpyWatch()
{
	if (lw_users->count() == 0 || lw_users->currentItem() == NULL) {
		QMessageBox msgBox;
		QString message = lw_users->count() == 0 ? "No client connected" : "No client selected";
		msgBox.setText(message);
		msgBox.setIcon(QMessageBox::Information);
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		msgBox.exec();
		chb_spy->setChecked(false);
		chb_center->setChecked(false);
		return;
	}

	Network::Server * server = Network::Server::getInstance();
	bool is_server = server->isListening();

	QCheckBox *sender_chb = (QCheckBox*)sender();

	int id_user = lw_users->currentItem()->data(6).toInt();

	// ak bolo kliknute na "spy"
	if (sender_chb == chb_spy) {

		// ak je "spy" zakliknute
		if (chb_spy->isChecked()) {

			// ak je centrovanie aktivne, deaktivujem
			if (client->isCenteringUser() || server->isCenteringUser()) {
				if (is_server) {
					server->unCenterUser();
				} else {
					client->unCenterUser();
				}
			}

			// aktivujem spehovanie
			if (is_server) {
				server->spyUser(id_user);
			} else {
				client->spyUser(id_user);
			}
			chb_center->setChecked(false);
		}
		// ak je "spy" odkliknute
		else {

			// ak je spehovanie aktivne, deaktivujem
			if (client->isSpying() || server->isSpying()) {
				if (is_server) {
					server->unSpyUser();
				} else {
					client->unSpyUser();
				}
			}
		}
	}

	// ak bolo kliknute "center"
	if (sender_chb == chb_center) {

		// ak je "center" zakliknute
		if (chb_center->isChecked()) {

			// ak je spehovanie aktivne, deaktivujem
			if (client->isSpying() || server->isSpying()) {
				if (is_server) {
					server->unSpyUser();
				} else {
					client->unSpyUser();
				}
			}

			// aktivujem centrovanie
			if (is_server) {
				server->centerUser(id_user);
			} else {
				client->centerUser(id_user);
			}
			chb_spy->setChecked(false);
		}

		// ak je "center" odkliknute
		else {

			// ak je centrovanie aktivne, deaktivujem
			if (client->isCenteringUser() || server->isCenteringUser()) {
				if (is_server) {
					server->unCenterUser();
				} else {
					client->unCenterUser();
				}
			}
		}
	}
}

void CoreWindow::toggleAttention() {
	if (chb_attention->isChecked()) {
		Network::Server * server = Network::Server::getInstance();
		if (server->isListening()) {
			server->sendAttractAttention(true);
		} else {
			client->sendAttractAttention(true);
		}
	} else {
		Network::Server * server = Network::Server::getInstance();
		if (server->isListening()) {
			server->sendAttractAttention(false);
		} else {
			client->sendAttractAttention(false);
		}
	}
}

void CoreWindow::setAvatarScale(int scale) {
	client->setAvatarScale(scale);
	Network::Server::getInstance()->setAvatarScale(scale);
}

// Duransky start - Akcia pri prepnuti checkboxu "Vertigo zoom"
void CoreWindow::toggleVertigo() {
    // ak je "Vertigo zoom" zakliknute
    if (chb_vertigo->isChecked()) {
        // nastavi kameru do vertigo modu
        viewerWidget->getCameraManipulator()->setVertigoMode(true);
    }else{
        // ak je "Vertigo zoom" odkliknute
        viewerWidget->getCameraManipulator()->setVertigoMode(false);
        // resetuje projekcnu maticu
        viewerWidget->getCameraManipulator()->resetProjectionMatrixToDefault();
    }
}
// Duransky end - Akcia pri prepnuti checkboxu "Vertigo zoom"

// Duransky start - Funkcia na vytvorenie daneho poctu vertigo rovin
void CoreWindow::create_Vertigo_Planes(int numberOfPlanes, int nOfDepthsInOnePlane, Data::GraphSpanningTree* spanningTree, int maxDepth, QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes ){

   QLinkedList<osg::ref_ptr<Data::Node> > pickedNodes;

   // vymazanie starych uzlov rovin - nie je spravne - namiesto odstranenia uzlov sa uzly zneviditelnia
   QLinkedList<Layout::ShapeGetter_Plane_ByThreeNodes*>::const_iterator it = planes_Vertigo.constBegin();
   for(it; it != planes_Vertigo.constEnd(); ++it){

       QSet<Data::Node *> nodesOfPlane = (*it)->getNodesOfShape();
       QSetIterator<Data::Node *> i(nodesOfPlane);
       while (i.hasNext()){
           Data::Node* node = i.next();
           node->setInvisible();
           // - vyriesit odstranovanie uzlov spravne
       }
   }

   planes_Vertigo.clear();

   int nOfDepths = 0;
   int nOfPlane = 1;

   // rozdelenie uzlov do rovin podla hlbky
   for (int depth = 0; depth <= maxDepth; depth++){

       ++nOfDepths;

       QList<qlonglong> groups = spanningTree->getGroupsInDepth(depth);

       QList<qlonglong>::iterator groupIt;
       for(groupIt=groups.begin(); groupIt!=groups.end();groupIt++){

           // vyber vsetkych skupin a uzlov v skupine danej hlbky
           QList<qlonglong> nodes = spanningTree->getNodesInGroup(*groupIt);
           QList<qlonglong>::iterator nodeIt;

           for(nodeIt=nodes.begin(); nodeIt!=nodes.end();nodeIt++){
               // nastavi uzlu cislo vertigo roviny, na ktorej sa nachadza
               allNodes->value(*nodeIt)->setNumberOfVertigoPlane(nOfPlane);
               pickedNodes.append(allNodes->value(*nodeIt));
           }
       }

       // obmedzenie uzlov podla poctu hlbok, ktore sa maju obmedzit na jednu rovinu
       if((depth != (nOfDepthsInOnePlane * numberOfPlanes - 1)) && nOfDepths == nOfDepthsInOnePlane){

           Layout::ShapeGetter_Plane_ByThreeNodes * plane = setRestriction_Plane_Vertigo(&pickedNodes,nOfPlane);
           planes_Vertigo.append(plane);

           nOfDepths = 0;
           ++nOfPlane;
           pickedNodes.clear();
       }
   }

   //zbytok uzlov sa obmedzi na poslednu rovinu
   Layout::ShapeGetter_Plane_ByThreeNodes * plane = setRestriction_Plane_Vertigo(&pickedNodes,nOfPlane);
   planes_Vertigo.append(plane);

}
// Duransky end - Funkcia na vytvorenie daneho poctu vertigo rovin

// Duransky start - Funkcia na pridanie dvoch vertigo rovin
void CoreWindow::add_PlanesClick()
{
    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

    // ak nebol nacitany graf, tak sa nic nestane
    if(currentGraph == NULL){
        return;
    }

    // vsetky uzly grafu
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();

    // uzly grafu vlozene do linked listu pre jednoduche iterovanie
    QLinkedList<osg::ref_ptr<Data::Node> > nodesList;
    QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = allNodes->constBegin();
    while (i != allNodes->constEnd()) {
        nodesList.append(i.value());
        ++i;
    }

    // vyber root node pre kostru grafu
    int maxEdges=0;
    osg::ref_ptr<Data::Node> rootNode;
    QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator itNode = nodesList.constBegin();
    for (itNode; itNode != nodesList.constEnd(); itNode++) {
        int actEdges = itNode->get()->getEdges()->size();
        if (actEdges>maxEdges){
            rootNode= itNode->get();
            maxEdges=actEdges;
        }
    }

    // vytvorenie kostry stromu z root node
    Data::GraphSpanningTree* spanningTree = currentGraph->getSpanningTree(rootNode->getId());

    // maximalna hlbka kostry grafu
    int maxDepth = spanningTree->getMaxDepth();

    // pocet hlbok na jednu rovinu
    int nOfDepthsInOnePlane = (maxDepth + 1) / (numberOfPlanes + 2);

    // ak nevieme rozdelit jemnejsie rozdelit kostru grafu, tak nic neurobime
    if(nOfDepthsInOnePlane == 0){
        return;
    }

    // pridame dve roviny
    numberOfPlanes += 2;

    // vytvorime roviny s ich zmenenym poctom
    create_Vertigo_Planes(numberOfPlanes, nOfDepthsInOnePlane, spanningTree, maxDepth, allNodes);

}
// Duransky end - Funkcia na pridanie dvoch vertigo rovin

// Duransky start - Funkcia na odobranie dvoch vertigo rovin
void CoreWindow::remove_PlanesClick() {

    if(numberOfPlanes == 0){
        return;
    }

    Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

    // ak nebol nacitany graf, tak sa nic nestane
    if(currentGraph == NULL){
        return;
    }

    // odstranenie obmedzeni vsetkych uzlov
    if(numberOfPlanes == 2){

         // vsetky uzly grafu
        QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();

        // reset hodnot urcujucich ktorej rovine patri uzol
        QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = allNodes->constBegin();
        while (i != allNodes->constEnd()) {
            i.value()->setNumberOfVertigoPlane(i.value()->getId());
            ++i;
        }

        //vymazanie starych uzlov rovin - setInvisible
        QLinkedList<Layout::ShapeGetter_Plane_ByThreeNodes*>::const_iterator it = planes_Vertigo.constBegin();
        for(it; it != planes_Vertigo.constEnd(); ++it){

            QSet<Data::Node *> nodesOfPlane = (*it)->getNodesOfShape();
            QSetIterator<Data::Node *> i(nodesOfPlane);
            while (i.hasNext()){
                Data::Node* node = i.next();
                node->setInvisible();
                //node->Referenced.deleteUsingDeleteHandler();
                /*osg::ref_ptr<Data::Node> nodeRefPtr = node;
                node->setRemovableByUser(true);
                currentGraph->removeNode(nodeRefPtr);*/
            }
        }

        planes_Vertigo.clear();

        // uvolni vsetky uzly z obmedzovacov
        unsetRestrictionFromAll();

        numberOfPlanes = 0;
        return;
    }

    // ak nebol nacitany graf, tak sa nic nestane
    if(currentGraph == NULL){
        return;
    }

    // vsetky uzly grafu
    QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();

    // uzly grafu vlozene do linked listu pre jednoduche iterovanie
    QLinkedList<osg::ref_ptr<Data::Node> > nodesList;
    QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = allNodes->constBegin();
    while (i != allNodes->constEnd()) {
        nodesList.append(i.value());
        ++i;
    }

    // vyber root node pre kostru grafu
    int maxEdges=0;
    osg::ref_ptr<Data::Node> rootNode;
    QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator itNode = nodesList.constBegin();
    for ( itNode; itNode != nodesList.constEnd(); itNode++) {
        int actEdges = itNode->get()->getEdges()->size();
        if ( actEdges>maxEdges){
            rootNode= itNode->get();
            maxEdges=actEdges;
        }
    }

    // vytvorenie kostry stromu z root node
    Data::GraphSpanningTree* spanningTree = currentGraph->getSpanningTree(rootNode->getId());
    //QLinkedList<osg::ref_ptr<Data::Node> > pickedNodes;

    // maximalna hlbka kostry grafu
    int maxDepth = spanningTree->getMaxDepth();

    // pocet hlbok na jednu rovinu
    int nOfDepthsInOnePlane = (maxDepth + 1) / (numberOfPlanes - 2);

    // odoberieme dve roviny
    numberOfPlanes -= 2;

    // vytvorime roviny s ich zmenenym poctom
    create_Vertigo_Planes(numberOfPlanes, nOfDepthsInOnePlane, spanningTree, maxDepth, allNodes);

}
// Duransky end - Funkcia na odobranie dvoch vertigo rovin

// Duransky start - Funkcie na zmenu vzajomnej vzdialenosti vertigo rovin
void CoreWindow::change_Vertigo_Planes_Distance(int value){

    int nOfPlane = 0;
    QLinkedList<Layout::ShapeGetter_Plane_ByThreeNodes*>::const_iterator it = planes_Vertigo.constBegin();
    for(it; it != planes_Vertigo.constEnd(); ++it , ++nOfPlane){

        // ziskanie troch uzlov, ktore urcuju rovinu - obmedzenie
        QSet<Data::Node *> nodesOfPlane = (*it)->getNodesOfShape();
        QSetIterator<Data::Node *> i(nodesOfPlane);

        // ohranicenie minimalnej a maximalnej vzdialenosti dvoch rovin
        if((value > 0 && nOfPlane == 1 && i.peekNext()->getTargetPosition().x() >= vertigoPlanesMaxDistance) ||
           (value < 0 && nOfPlane == 1 && i.peekNext()->getTargetPosition().x() <= vertigoPlanesMinDistance)){
            return;
        }

        // pridanie zmeny vzdialenosti ku kazdemu uzlu, ktory urcuje rovinu - obmedzenie
        while (i.hasNext()){
            Data::Node* node = i.next();
            osg::Vec3f oldPosition = node->getTargetPosition();
            node->setTargetPosition(oldPosition + osg::Vec3f ((float)(value * nOfPlane), 0.f, 0.f));
        }
    }
    //zmena vzdialenosti medzi rovinami globalne
    vertigoPlanesDistance += value;
}

void CoreWindow::add_DistanceClick(){
    // zvysenie vzdialenosti medzi rovinami
    change_Vertigo_Planes_Distance(deltaVertigoDistance);
}

void CoreWindow::subtract_DistanceClick(){
    // znizenie vzdialenosti medzi rovinami
    change_Vertigo_Planes_Distance(-deltaVertigoDistance);
}
// Duransky end - Funkcie na zmenu vzajomnej vzdialenosti vertigo rovin

// Duransky start - Funkcia na zmenu hodnoty nasobica odpudivych sil dvoch uzlov nachadzajucich sa na rovnakej rovine
void CoreWindow::repulsive_Forces_ValueChanged() {
    layout->getAlg()->setRepulsiveForceVertigo(change_Forces->value());
}
// Duransky end - Funkcia na zmenu hodnoty nasobica odpudivych sil dvoch uzlov nachadzajucich sa na rovnakej rovine

Vwr::CameraManipulator* CoreWindow::getCameraManipulator() {
    return viewerWidget->getCameraManipulator();
}

QOSG::ViewerQT * CoreWindow::GetViewerQt()
{
	return viewerWidget;
}

void CoreWindow::closeEvent(QCloseEvent *event)
{

#ifdef OPENCV_FOUND
	delete OpenCV::OpenCVCore::getInstance(NULL, this);
#endif
	//QApplication::closeAllWindows();   // ????
}

void QOSG::CoreWindow::moveMouseAruco(double positionX,double positionY,bool isClick, Qt::MouseButton button )
{
	this->viewerWidget->moveMouseAruco(positionX,positionY,isClick,this->x(),this->y(),button);
}
