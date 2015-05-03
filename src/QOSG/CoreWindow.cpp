#include "QOSG/CoreWindow.h"

#include "QOSG/ViewerQT.h"
#include "QOSG/OptionsWindow.h"
#include "QOSG/LoadGraphWindow.h"
#include "QOSG/MessageWindows.h"

#include "Network/Server.h"
#include "Network/Client.h"

#include "Viewer/CoreGraph.h"
#include "Viewer/CameraManipulator.h"
#include "Viewer/PickHandler.h"

#include "Manager/Manager.h"

#include "Model/DB.h"
#include "Model/GraphLayoutDAO.h"

#include "Data/GraphLayout.h"
#include "Data/GraphSpanningTree.h"

#include "Layout/LayoutThread.h"
#include "Layout/FRAlgorithm.h"
#include "Layout/ShapeGetter_CylinderSurface_ByCamera.h"
#include "Layout/ShapeGetter_SphereSurface_ByTwoNodes.h"
#include "Layout/ShapeGetter_Sphere_ByTwoNodes.h"
#include "Layout/ShapeGetter_ConeSurface_ByCamera.h"
#include "Layout/ShapeGetter_Plane_ByThreeNodes.h"
#include "Layout/RadialLayout.h"
#include "Layout/ShapeGetter_Cube.h"

#include "Importer/GraphOperations.h"
#include "Util/Cleaner.h"
#include "Core/Core.h"

#include "QDebug"

#include "LuaGraph/LuaGraph.h"
#include "LuaInterface/LuaInterface.h"
#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/FullHyperGraphVisualizer.h"
#include "LuaGraph/HyperGraphVisualizer.h"
#include "LuaGraph/SimpleGraphVisualizer.h"

#include "Diluculum/LuaState.hpp"
#include <LuaGraph/LuaGraphTreeModel.h>

#include <iostream>
#include <osg/ref_ptr>

#ifdef OPENCV_FOUND
#include "OpenCV/OpenCVCore.h"
#endif

#ifdef OPENCV_FOUND
#ifdef OPENNI2_FOUND
#include "Kinect/KinectCore.h"
#include "Kinect/RansacSurface/Ransac.h"
#endif
#endif

namespace QOSG {

CoreWindow::CoreWindow( QWidget* parent, Vwr::CoreGraph* coreGraph, QApplication* app, Layout::LayoutThread* thread ) : QMainWindow( parent )
{
	//inicializacia premennych
	isPlaying = true;
	isEBPlaying = false;
	application = app;
	layout = thread;

	client = new Network::Client( this );
	new Network::Server( this );

	//vytvorenie menu a toolbar-ov
	createActions();
	createMenus();
	createLeftToolBar();
	createMetricsToolBar();

	viewerWidget = new ViewerQT( this, 0, 0, 0, coreGraph );
	viewerWidget->setSceneData( coreGraph->getScene() );

	setCentralWidget( viewerWidget );

	statusBar()->showMessage( "Ready" );

	dock = new QDockWidget( this );
	dock->setAllowedAreas( Qt::TopDockWidgetArea );

	lineEdit = new QLineEdit();

	//vytvorenie sql vstupu
	QWidget* widget = new QWidget();
	QHBoxLayout* layoutt = new QHBoxLayout();
	layoutt->addWidget( lineEdit );
	widget->setLayout( layoutt );
	dock->setWidget( widget );
	addDockWidget( Qt::TopDockWidgetArea, dock );
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

	connect( lineEdit,SIGNAL( returnPressed() ),this,SLOT( sqlQuery() ) );


	// connect checkbox for interchanging between rotation of camera or rotation of graph
	QObject::connect( chb_camera_rot, SIGNAL( clicked( bool ) ),
					  viewerWidget->getCameraManipulator(), SLOT( setCameraCanRot( bool ) ) );

	Lua::LuaInterface::getInstance()->executeFile( "main.lua" );
	viewerWidget->getPickHandler()->setSelectionObserver( this );

}

void CoreWindow::createActions()
{
	quit = new QAction( "Quit", this );
	connect( quit, SIGNAL( triggered() ), application, SLOT( quit() ) );

	options = new QAction( "Options", this );
	connect( options,SIGNAL( triggered() ),this,SLOT( showOptions() ) );

	load = new QAction( QIcon( "../share/3dsoftviz/img/gui/open.png" ),"&Load graph from file", this );
	connect( load, SIGNAL( triggered() ), this, SLOT( loadFile() ) );

	loadGraph = new QAction( QIcon( "../share/3dsoftviz/img/gui/loadFromDB.png" ),"&Load graph from database", this );
	connect( loadGraph, SIGNAL( triggered() ), this, SLOT( showLoadGraph() ) );

	saveGraph = new QAction( QIcon( "../share/3dsoftviz/img/gui/saveToDB.png" ),"&Save graph", this );
	connect( saveGraph, SIGNAL( triggered() ), this, SLOT( saveGraphToDB() ) );

	saveLayout = new QAction( QIcon( "../share/3dsoftviz/img/gui/saveToDB.png" ),"&Save layout", this );
	connect( saveLayout, SIGNAL( triggered() ), this, SLOT( saveLayoutToDB() ) );

	about = new QAction( "About", this );

	play = new QPushButton();
	play->setIcon( QIcon( "../share/3dsoftviz/img/gui/pause.png" ) );
	play->setToolTip( "&Play" );
	play->setFocusPolicy( Qt::NoFocus );
	connect( play, SIGNAL( clicked() ), this, SLOT( playPause() ) );

	addMeta = new QPushButton();
	addMeta->setIcon( QIcon( "../share/3dsoftviz/img/gui/meta.png" ) );
	addMeta->setToolTip( "&Add meta node" );
	addMeta->setFocusPolicy( Qt::NoFocus );
	connect( addMeta, SIGNAL( clicked() ), this, SLOT( addMetaNode() ) );

	removeMeta = new QPushButton();
	removeMeta->setIcon( QIcon( "../share/3dsoftviz/img/gui/removemeta.png" ) );
	removeMeta->setToolTip( "&Remove meta nodes" );
	removeMeta->setFocusPolicy( Qt::NoFocus );
	connect( removeMeta, SIGNAL( clicked() ), this, SLOT( removeMetaNodes() ) );

	fix = new QPushButton();
	fix->setIcon( QIcon( "../share/3dsoftviz/img/gui/fix.png" ) );
	fix->setToolTip( "&Fix nodes" );
	fix->setFocusPolicy( Qt::NoFocus );
	connect( fix, SIGNAL( clicked() ), this, SLOT( fixNodes() ) );

	unFix = new QPushButton();
	unFix->setIcon( QIcon( "../share/3dsoftviz/img/gui/unfix.png" ) );
	unFix->setToolTip( "&Unfix nodes" );
	unFix->setFocusPolicy( Qt::NoFocus );
	connect( unFix, SIGNAL( clicked() ), this, SLOT( unFixNodes() ) );

	merge = new QPushButton();
	merge->setIcon( QIcon( "../share/3dsoftviz/img/gui/merge.png" ) );
	merge->setToolTip( "&Merge nodes together" );
	merge->setFocusPolicy( Qt::NoFocus );
	connect( merge, SIGNAL( clicked() ), this, SLOT( mergeNodes() ) );

	separate = new QPushButton();
	separate->setIcon( QIcon( "../share/3dsoftviz/img/gui/separate.png" ) );
	separate->setToolTip( "&Separate merged nodes" );
	separate->setFocusPolicy( Qt::NoFocus );
	connect( separate, SIGNAL( clicked() ), this, SLOT( separateNodes() ) );

	label = new QPushButton();
	label->setIcon( QIcon( "../share/3dsoftviz/img/gui/label.png" ) );
	label->setToolTip( "&Turn on/off labels" );
	label->setCheckable( true );
	label->setFocusPolicy( Qt::NoFocus );
	connect( label, SIGNAL( clicked( bool ) ), this, SLOT( labelOnOff( bool ) ) );

	applyColor = new QPushButton();
	applyColor->setText( "Apply color" );
	applyColor->setToolTip( "Apply selected color" );
	applyColor->setFocusPolicy( Qt::NoFocus );
	connect( applyColor,SIGNAL( clicked() ),this,SLOT( applyColorClick() ) );

	applyLabel = new QPushButton();
	applyLabel->setText( "Apply label" );
	applyLabel->setToolTip( "Apply selected label" );
	applyLabel->setFocusPolicy( Qt::NoFocus );
	connect( applyLabel,SIGNAL( clicked() ),this,SLOT( applyLabelClick() ) );

	le_applyLabel = new QLineEdit;

	//add edge
	add_Edge = new QPushButton();
	add_Edge->setText( "Add Edge" );
	add_Edge->setToolTip( "Create new edge between two selected Nodes" );
	add_Edge->setFocusPolicy( Qt::NoFocus );
	connect( add_Edge,SIGNAL( clicked() ),this,SLOT( add_EdgeClick() ) );

	//add Node
	add_Node = new QPushButton();
	add_Node->setText( "Add Node" );
	add_Node->setToolTip( "Create node" );
	add_Node->setFocusPolicy( Qt::NoFocus );
	connect( add_Node, SIGNAL( clicked() ), this, SLOT( add_NodeClick() ) );

	//remove
	remove_all = new QPushButton();
	remove_all->setText( "Remove" );
	remove_all->setToolTip( "Remove nodes and edges" );
	remove_all->setFocusPolicy( Qt::NoFocus );
	connect( remove_all, SIGNAL( clicked() ), this, SLOT( removeClick() ) );

	// <Change> Nagy+Gloger
	loadFunctionCallButton = new QPushButton();
	loadFunctionCallButton->setText( "Load function calls" );
	loadFunctionCallButton->setToolTip( "Load function calls" );
	loadFunctionCallButton->setFocusPolicy( Qt::NoFocus );
	connect( loadFunctionCallButton, SIGNAL( clicked() ), this, SLOT( loadFunctionCall() ) );

	browsersGroupingButton = new QPushButton();
	browsersGroupingButton->setIcon( QIcon( "../share/3dsoftviz/img/gui/grouping.png" ) );
	browsersGroupingButton->setToolTip( "Toggle browsers (webViews) grouping" );
	browsersGroupingButton->setCheckable( true );
	browsersGroupingButton->setFocusPolicy( Qt::NoFocus );
	connect( browsersGroupingButton, SIGNAL( clicked( bool ) ), this, SLOT( browsersGroupingClicked( bool ) ) );

	filterNodesEdit = new QLineEdit();
	filterEdgesEdit = new QLineEdit();
	connect( filterNodesEdit, SIGNAL( returnPressed() ), this, SLOT( filterGraph() ) );
	connect( filterEdgesEdit, SIGNAL( returnPressed() ), this, SLOT( filterGraph() ) );

	luaGraphTreeView = new QTreeView();
	// <end change> Nagy+Gloger

	//mody - ziadny vyber, vyber jedneho, multi vyber centrovanie
	noSelect = new QPushButton();
	noSelect->setIcon( QIcon( "../share/3dsoftviz/img/gui/noselect.png" ) );
	noSelect->setToolTip( "&No-select mode" );
	noSelect->setCheckable( true );
	noSelect->setFocusPolicy( Qt::NoFocus );
	connect( noSelect, SIGNAL( clicked( bool ) ), this, SLOT( noSelectClicked( bool ) ) );

	singleSelect = new QPushButton();
	singleSelect->setIcon( QIcon( "../share/3dsoftviz/img/gui/singleselect.png" ) );
	singleSelect->setToolTip( "&Single-select mode" );
	singleSelect->setCheckable( true );
	singleSelect->setFocusPolicy( Qt::NoFocus );
	connect( singleSelect, SIGNAL( clicked( bool ) ), this, SLOT( singleSelectClicked( bool ) ) );

	multiSelect = new QPushButton();
	multiSelect->setIcon( QIcon( "../share/3dsoftviz/img/gui/multiselect.png" ) );
	multiSelect->setToolTip( "&Multi-select mode" );
	multiSelect->setCheckable( true );
	multiSelect->setFocusPolicy( Qt::NoFocus );
	connect( multiSelect, SIGNAL( clicked( bool ) ), this, SLOT( multiSelectClicked( bool ) ) );

	center = new QPushButton();
	center->setIcon( QIcon( "../share/3dsoftviz/img/gui/center.png" ) );
	center->setToolTip( "&Center view" );
	center->setFocusPolicy( Qt::NoFocus );
	connect( center, SIGNAL( clicked( bool ) ), this, SLOT( centerView( bool ) ) );

	// layout restrictions
	b_SetRestriction_SphereSurface = new QPushButton();
	b_SetRestriction_SphereSurface->setIcon( QIcon( "../share/3dsoftviz/img/gui/restriction_sphere_surface.png" ) );
	b_SetRestriction_SphereSurface->setToolTip( "&Set restriction - sphere surface" );
	b_SetRestriction_SphereSurface->setFocusPolicy( Qt::NoFocus );
	connect( b_SetRestriction_SphereSurface, SIGNAL( clicked() ), this, SLOT( setRestriction_SphereSurface() ) );

	b_SetRestriction_Sphere = new QPushButton();
	b_SetRestriction_Sphere->setIcon( QIcon( "../share/3dsoftviz/img/gui/restriction_sphere.png" ) );
	b_SetRestriction_Sphere->setToolTip( "&Set restriction - sphere" );
	b_SetRestriction_Sphere->setFocusPolicy( Qt::NoFocus );
	connect( b_SetRestriction_Sphere, SIGNAL( clicked() ), this, SLOT( setRestriction_Sphere() ) );

	b_SetRestriction_Plane = new QPushButton();
	b_SetRestriction_Plane->setIcon( QIcon( "../share/3dsoftviz/img/gui/restriction_plane.png" ) );
	b_SetRestriction_Plane->setToolTip( "&Set restriction - plane" );
	b_SetRestriction_Plane->setFocusPolicy( Qt::NoFocus );
	connect( b_SetRestriction_Plane, SIGNAL( clicked() ), this, SLOT( setRestriction_Plane() ) );

	b_SetRestriction_SpherePlane = new QPushButton();
	b_SetRestriction_SpherePlane->setIcon( QIcon( "../share/3dsoftviz/img/gui/restriction_sphereplane.png" ) );
	b_SetRestriction_SpherePlane->setToolTip( "&Set restriction - sphere and plane" );
	b_SetRestriction_SpherePlane->setFocusPolicy( Qt::NoFocus );
	connect( b_SetRestriction_SpherePlane, SIGNAL( clicked() ), this, SLOT( setRestriction_SpherePlane() ) );

	b_SetRestriction_Circle = new QPushButton();
	b_SetRestriction_Circle->setIcon( QIcon( "../share/3dsoftviz/img/gui/restriction_circle.png" ) );
	b_SetRestriction_Circle->setToolTip( "&Set restriction - circle" );
	b_SetRestriction_Circle->setFocusPolicy( Qt::NoFocus );
	connect( b_SetRestriction_Circle, SIGNAL( clicked() ), this, SLOT( setRestriction_Circle() ) );

	b_SetRestriction_Cone = new QPushButton();
	b_SetRestriction_Cone->setIcon( QIcon( "../share/3dsoftviz/img/gui/restriction_cone.png" ) );
	b_SetRestriction_Cone->setToolTip( "&Set restriction - cone" );
	b_SetRestriction_Cone->setFocusPolicy( Qt::NoFocus );
	connect( b_SetRestriction_Cone, SIGNAL( clicked() ), this, SLOT( setRestriction_Cone() ) );

	b_SetRestriction_ConeTree = new QPushButton();
	b_SetRestriction_ConeTree->setIcon( QIcon( "../share/3dsoftviz/img/gui/restriction_conetree.png" ) );
	b_SetRestriction_ConeTree->setToolTip( "&Set restriction - cone tree" );
	b_SetRestriction_ConeTree->setFocusPolicy( Qt::NoFocus );
	connect( b_SetRestriction_ConeTree, SIGNAL( clicked() ), this, SLOT( setRestriction_ConeTree() ) );

	b_UnsetRestriction = new QPushButton();
	b_UnsetRestriction->setIcon( QIcon( "../share/3dsoftviz/img/gui/restriction_unset.png" ) );
	b_UnsetRestriction->setToolTip( "&Unset restriction" );
	b_UnsetRestriction->setFocusPolicy( Qt::NoFocus );
	connect( b_UnsetRestriction, SIGNAL( clicked() ), this, SLOT( unsetRestriction() ) );

	b_SetRestriction_CylinderSurface = new QPushButton();
	b_SetRestriction_CylinderSurface->setIcon( QIcon( "../share/3dsoftviz/img/gui/restriction_cylinder_surface.png" ) );
	b_SetRestriction_CylinderSurface->setToolTip( "&Set restriction - cylinder surface" );
	b_SetRestriction_CylinderSurface->setFocusPolicy( Qt::NoFocus );
	connect( b_SetRestriction_CylinderSurface, SIGNAL( clicked() ), this, SLOT( setRestriction_CylinderSurface() ) );

	b_SetRestriction_CylinderSurface_SpinBox = new QSpinBox();
	b_SetRestriction_CylinderSurface_SpinBox->setToolTip( "&Modify base radius of the restriction" );
	b_SetRestriction_CylinderSurface_SpinBox->setFocusPolicy( Qt::NoFocus );
	b_SetRestriction_CylinderSurface_SpinBox->setValue( 25 );

	//volovar_zac
	b_SetRestriction_RadialLayout_Slider = new QSlider( Qt::Horizontal );
	b_SetRestriction_RadialLayout_Slider->setToolTip( "&Change radius of Radial layout" );
	b_SetRestriction_RadialLayout_Slider->setRange( 0, 300 );
	b_SetRestriction_RadialLayout_Slider->setSingleStep( 10 );
	b_SetRestriction_RadialLayout_Slider->setFocusPolicy( Qt::ClickFocus );
	b_SetRestriction_RadialLayout_Slider->setValue( 100 );

	b_SetAlpha_RadialLayout_Slider = new QSlider( Qt::Horizontal );
	b_SetAlpha_RadialLayout_Slider->setToolTip( "&Change alpha of Radial layout" );
	b_SetAlpha_RadialLayout_Slider->setRange( 0, 100 );
	b_SetAlpha_RadialLayout_Slider->setSingleStep( 1 );
	b_SetAlpha_RadialLayout_Slider->setFocusPolicy( Qt::ClickFocus );
	b_SetAlpha_RadialLayout_Slider->setValue( 10 );

	b_SetVisibleSpheres_RadialLayout_Slider = new QSlider( Qt::Horizontal );
	b_SetVisibleSpheres_RadialLayout_Slider->setToolTip( "&Change number of visible spheres in Radial layout" );
	b_SetVisibleSpheres_RadialLayout_Slider->setRange( 0, 100 );
	b_SetVisibleSpheres_RadialLayout_Slider->setSingleStep( 1 );
	b_SetVisibleSpheres_RadialLayout_Slider->setFocusPolicy( Qt::ClickFocus );
	b_SetVisibleSpheres_RadialLayout_Slider->setValue( 100 );


	b_SetForceScale_RadialLayout_Slider = new QSlider( Qt::Horizontal );
	b_SetForceScale_RadialLayout_Slider->setToolTip( "&Change force between two nodes in Radial layout" );
	b_SetForceScale_RadialLayout_Slider->setRange( 1, 5000 );
	b_SetForceScale_RadialLayout_Slider->setSingleStep( 1 );
	b_SetForceScale_RadialLayout_Slider->setFocusPolicy( Qt::ClickFocus );
	b_SetForceScale_RadialLayout_Slider->setValue( 1000 );

	b_SetForceSphereScale_RadialLayout_Slider = new QSlider( Qt::Horizontal );
	b_SetForceSphereScale_RadialLayout_Slider->setToolTip( "&Change force between two nodes in same sphere in Radial layout" );
	b_SetForceSphereScale_RadialLayout_Slider->setRange( 1, 5000 );
	b_SetForceSphereScale_RadialLayout_Slider->setSingleStep( 1 );
	b_SetForceSphereScale_RadialLayout_Slider->setFocusPolicy( Qt::ClickFocus );
	b_SetForceSphereScale_RadialLayout_Slider->setValue( 1000 );
	//volovar_kon

	b_SetRestriction_ConeSurface = new QPushButton();
	b_SetRestriction_ConeSurface->setIcon( QIcon( "../share/3dsoftviz/img/gui/restriction_cone_surface.png" ) );
	b_SetRestriction_ConeSurface->setToolTip( "&Set restriction - cone surface" );
	b_SetRestriction_ConeSurface->setFocusPolicy( Qt::NoFocus );
	connect( b_SetRestriction_ConeSurface, SIGNAL( clicked() ), this, SLOT( setRestriction_ConeSurface() ) );

	b_SetRestriction_ConeSurface_SpinBox = new QSpinBox();
	b_SetRestriction_ConeSurface_SpinBox->setToolTip( "&Modify base radius of the restriction" );
	b_SetRestriction_ConeSurface_SpinBox->setFocusPolicy( Qt::NoFocus );
	b_SetRestriction_ConeSurface_SpinBox->setValue( 25 );

	//volovar_zac
	b_SetRestriction_RadialLayout = new QPushButton();
	b_SetRestriction_RadialLayout->setIcon( QIcon( "../share/3dsoftviz/img/gui/restriction_radial_layout.png" ) );
	b_SetRestriction_RadialLayout->setToolTip( "&Set restriction - radial Layout" );
	b_SetRestriction_RadialLayout->setFocusPolicy( Qt::NoFocus );
	connect( b_SetRestriction_RadialLayout, SIGNAL( clicked() ), this, SLOT( setRestriction_RadialLayout() ) );

	b_drawMethod_RadialLayout = new QPushButton();
	b_drawMethod_RadialLayout->setIcon( QIcon( "../share/3dsoftviz/img/gui/drawMethod_radial_layout.png" ) );
	b_drawMethod_RadialLayout->setToolTip( "&Set restriction - radial Layout" );
	b_drawMethod_RadialLayout->setFocusPolicy( Qt::NoFocus );
	connect( b_drawMethod_RadialLayout, SIGNAL( clicked() ), this, SLOT( changeDrawMethod_RadialLayout() ) );

	b_mode_RadialLayout = new QPushButton();
	b_mode_RadialLayout->setIcon( QIcon( "../share/3dsoftviz/img/gui/mode_radial_layout.png" ) );
	b_mode_RadialLayout->setToolTip( "&Set restriction - radial Layout" );
	b_mode_RadialLayout->setFocusPolicy( Qt::NoFocus );
	connect( b_mode_RadialLayout, SIGNAL( clicked() ), this, SLOT( changeMode_RadialLayout() ) );

	//volovar_kon

	b_UnsetRestrictionFromAll = new QPushButton();
	b_UnsetRestrictionFromAll->setIcon( QIcon( "../share/3dsoftviz/img/gui/restriction_unset.png" ) );
	b_UnsetRestrictionFromAll->setToolTip( "&Unset restriction from all nodes" );
	b_UnsetRestrictionFromAll->setFocusPolicy( Qt::NoFocus );
	connect( b_UnsetRestrictionFromAll, SIGNAL( clicked() ), this, SLOT( unsetRestrictionFromAll() ) );

	b_StartEdgeBundling = new QPushButton();
	b_StartEdgeBundling->setText( "Edge Bundling" );
	b_StartEdgeBundling->setToolTip( "&Start edge bundling" );
	b_StartEdgeBundling->setFocusPolicy( Qt::NoFocus );
	connect( b_StartEdgeBundling, SIGNAL( clicked() ), this, SLOT( startEdgeBundling() ) );

	b_EdgeBundling_SpinBox = new QSpinBox();
	b_EdgeBundling_SpinBox->setToolTip( "&Modify count of edge parts" );
	b_EdgeBundling_SpinBox->setFocusPolicy( Qt::NoFocus );
	b_EdgeBundling_SpinBox->setValue( 3 );

	edgeBundlingSlider = new QSlider( Qt::Horizontal,this );
	edgeBundlingSlider->setTickPosition( QSlider::TicksAbove );
	edgeBundlingSlider->setTickInterval( 1 );
	edgeBundlingSlider->setValue( 50 );
	edgeBundlingSlider->setFocusPolicy( Qt::NoFocus );
	connect( edgeBundlingSlider,SIGNAL( valueChanged( int ) ),this,SLOT( edgeBundlingSliderValueChanged( int ) ) );

	nodeTypeComboBox = new QComboBox();
	nodeTypeComboBox->insertItems( 0,( QStringList() << "Square" << "Sphere" ) );
	nodeTypeComboBox->setFocusPolicy( Qt::NoFocus );
	connect( nodeTypeComboBox,SIGNAL( currentIndexChanged( int ) ),this,SLOT( nodeTypeComboBoxChanged( int ) ) );

	edgeTypeComboBox = new QComboBox();
	edgeTypeComboBox->insertItems( 0,( QStringList() << "Quad" << "Cylinder" << "Line" << "Curve" ) );
	edgeTypeComboBox->setFocusPolicy( Qt::NoFocus );
	connect( edgeTypeComboBox,SIGNAL( currentIndexChanged( int ) ),this,SLOT( edgeTypeComboBoxChanged( int ) ) );

	b_start_server = new QPushButton();
	b_start_server->setText( "Host session" );
	connect( b_start_server, SIGNAL( clicked() ), this, SLOT( start_server() ) );

	b_start_client = new QPushButton();
	b_start_client->setText( "Connect to session" );
	connect( b_start_client, SIGNAL( clicked() ), this, SLOT( start_client() ) );

	b_send_message = new QPushButton();
	b_send_message->setText( "Send" );
	connect( b_send_message, SIGNAL( clicked() ), this, SLOT( send_message() ) );

	chb_center = new QCheckBox( "&Center" );
	connect( chb_center, SIGNAL( clicked() ), this, SLOT( toggleSpyWatch() ) );

	chb_spy = new QCheckBox( "&Spy" );
	connect( chb_spy, SIGNAL( clicked() ), this, SLOT( toggleSpyWatch() ) );

	chb_attention = new QCheckBox( "S&hout" );
	connect( chb_attention, SIGNAL( clicked() ), this, SLOT( toggleAttention() ) );

	le_client_name = new QLineEdit( "Nick" );
	le_server_addr = new QLineEdit( "localhost" );
	le_message= new QLineEdit( "Message" );

	lw_users = new QListWidget();
	lw_users->setSelectionMode( QListWidget::SingleSelection );
	lw_users->setSortingEnabled( true );
	lw_users->setMaximumHeight( 100 );

	sl_avatarScale = new QSlider( Qt::Horizontal,this );
	sl_avatarScale->setTickPosition( QSlider::TicksAbove );
	sl_avatarScale->setRange( 1,20 );
	sl_avatarScale->setPageStep( 1 );
	sl_avatarScale->setValue( 1 );
	sl_avatarScale->setFocusPolicy( Qt::NoFocus );
	connect( sl_avatarScale,SIGNAL( valueChanged( int ) ),this,SLOT( setAvatarScale( int ) ) );

	// Duransky start - Nastavenie widgetov

	chb_vertigo = new QCheckBox( "Vertigo zoom" );
	connect( chb_vertigo, SIGNAL( clicked() ), this, SLOT( toggleVertigo() ) );

	//Add distance
	add_Distance = new QPushButton();
	add_Distance->setText( "Add distance" );
	add_Distance->setToolTip( "Adds distance between planes" );
	add_Distance->setFocusPolicy( Qt::NoFocus );
	connect( add_Distance,SIGNAL( clicked() ),this,SLOT( add_DistanceClick() ) );

	//Subtract distance
	subtract_Distance = new QPushButton();
	subtract_Distance->setText( "Substract distance" );
	subtract_Distance->setToolTip( "Subtracts distance between planes" );
	subtract_Distance->setFocusPolicy( Qt::NoFocus );
	connect( subtract_Distance,SIGNAL( clicked() ),this,SLOT( subtract_DistanceClick() ) );

	//Add planes
	add_Planes = new QPushButton();
	add_Planes->setText( "Add Planes" );
	add_Planes->setToolTip( "Adds two planes for the vertigo zoom" );
	add_Planes->setFocusPolicy( Qt::NoFocus );
	connect( add_Planes,SIGNAL( clicked() ),this,SLOT( add_PlanesClick() ) );

	//Remove planes
	remove_Planes = new QPushButton();
	remove_Planes->setText( "Remove Planes" );
	remove_Planes->setToolTip( "Removes two planes for the vertigo zoom" );
	remove_Planes->setFocusPolicy( Qt::NoFocus );
	connect( remove_Planes,SIGNAL( clicked() ),this,SLOT( remove_PlanesClick() ) );

	//Change repulsive forces between the node on the same plane
	change_Forces = new QSpinBox;
	change_Forces->setRange( 1,1000 );
	change_Forces->setValue( 1 );
	change_Forces->setToolTip( "Changes the repulsive forces between nodes restricted in one plane" );
	change_Forces->setFocusPolicy( Qt::NoFocus );
	connect( change_Forces,SIGNAL( valueChanged( int ) ),this,SLOT( repulsive_Forces_ValueChanged() ) );

	// Duransky end - Nastavenie widgetov

	// Britvik start
	chb_clustersOpacity = new QCheckBox( "auto" );
	connect( chb_clustersOpacity, SIGNAL( clicked( bool ) ), this, SLOT( clustersOpacityCheckboxValueChanged( bool ) ) );

	chb_clusterSelectedOpacity = new QCheckBox( "selected" );
	connect( chb_clusterSelectedOpacity, SIGNAL( clicked( bool ) ), this, SLOT( clusterSelectedOpacityCheckboxValueChanged( bool ) ) );

	l_clustersOpacity = new QLabel( "Opacity:" );
	l_clustersShapes = new QLabel( "Cluster shapes:" );

	b_clustersOpacity_Slider = new QSlider( Qt::Horizontal );
	b_clustersOpacity_Slider->setToolTip( "Set opacity for clusters" );
	b_clustersOpacity_Slider->setFocusPolicy( Qt::NoFocus );
	b_clustersOpacity_Slider->setTickPosition( QSlider::TicksAbove );
	b_clustersOpacity_Slider->setRange( 0,10 );
	b_clustersOpacity_Slider->setPageStep( 1 );
	connect( b_clustersOpacity_Slider, SIGNAL( valueChanged( int ) ), this, SLOT( clustersOpacitySliderValueChanged( int ) ) );

	l_clusters1Min = new QLabel( "0" );
	l_clusters1Max = new QLabel( "0" );

	l_clusters1Min->setFixedWidth( 30 );
	l_clusters1Min->setAlignment( Qt::AlignCenter );
	l_clusters1Max->setFixedWidth( 30 );
	l_clusters1Max->setAlignment( Qt::AlignCenter );

	b_clustersShapeBoundary_Slider = new QSlider( Qt::Horizontal );
	b_clustersShapeBoundary_Slider->setToolTip( "Change cluster shapes based on number of clustered nodes inside them" );
	b_clustersShapeBoundary_Slider->setFocusPolicy( Qt::NoFocus );
	connect( b_clustersShapeBoundary_Slider, SIGNAL( valueChanged( int ) ), this, SLOT( clustersShapeBoundarySliderValueChanged( int ) ) );

	cb_clusteringAlgorithm = new QComboBox();
	cb_clusteringAlgorithm->insertItems( 0,( QStringList() << "Adjacency" << "Leafs" << "Neighbours" ) );
	cb_clusteringAlgorithm->setFocusPolicy( Qt::NoFocus );
	connect( cb_clusteringAlgorithm,SIGNAL( currentIndexChanged( int ) ),this,SLOT( clusteringAlgorithmChanged( int ) ) );

	le_clusteringDepth = new QLineEdit();
	le_clusteringDepth->setText( "0" );
	le_clusteringDepth->setAlignment( Qt::AlignCenter );
	le_clusteringDepth->setFixedWidth( 30 );
	le_clusteringDepth->setValidator( new QIntValidator( 0, 10, this ) );
	connect( le_clusteringDepth, SIGNAL( textChanged( const QString& ) ), this, SLOT( clusteringDepthChanged( const QString& ) ) );

	b_cluster_nodes = new QPushButton();
	b_cluster_nodes->setText( "Cluster nodes" );
	connect( b_cluster_nodes, SIGNAL( clicked() ), this, SLOT( cluster_nodes() ) );

	b_restartLayouting = new QPushButton();
	b_restartLayouting->setText( "Restart Layout" );
	connect( b_restartLayouting, SIGNAL( clicked() ), this, SLOT( restartLayouting() ) );

	clusteringProgressBar = new QProgressDialog( "", "", 0, 10, this, Qt::Dialog );
	clusteringProgressBar->setWindowTitle( "Clustering" );
	clusteringProgressBar->setCancelButtonText( "Abort" );
	Qt::WindowFlags flags = clusteringProgressBar->windowFlags();
	flags = flags & ( ~Qt::WindowContextHelpButtonHint );
	clusteringProgressBar->setWindowFlags( flags );
	clusteringProgressBar->setModal( true );
	clusteringProgressBar->setMinimumDuration( 1000 );

	b_SetRestriction_Cube_Selected = new QPushButton();
	b_SetRestriction_Cube_Selected->setText( "Restrict" );
	b_SetRestriction_Cube_Selected->setFocusPolicy( Qt::NoFocus );
	connect( b_SetRestriction_Cube_Selected, SIGNAL( clicked() ), this, SLOT( setRestriction_Cube_Selected() ) );

	l_repulsiveForceInsideCluster = new QLabel( "Repulsive force" );
	l_repulsiveForceInsideCluster->hide();

	sb_repulsiveForceInsideCluster = new QDoubleSpinBox();
	sb_repulsiveForceInsideCluster->setToolTip( "Modify repulsive force inside cluster" );
	sb_repulsiveForceInsideCluster->setMinimum( 0 );
	sb_repulsiveForceInsideCluster->setMaximum( 500 );
	connect( sb_repulsiveForceInsideCluster, SIGNAL( valueChanged( double ) ), this, SLOT( repulsiveForceInsideClusterValueChanged( double ) ) );

	sb_repulsiveForceInsideCluster->hide();
	b_SetRestriction_Cube_Selected->hide();
	b_restartLayouting->hide();

	line1 = createLine();
	line2 = createLine();
	line3 = createLine();
	// hide
	setVisibleClusterSection( false );
}

void CoreWindow::setVisibleClusterSection( bool visible )
{
	l_clustersOpacity->setVisible( visible );
	chb_clustersOpacity->setVisible( visible );
	chb_clusterSelectedOpacity->setVisible( visible );
	line1->setVisible( visible );
	b_clustersOpacity_Slider->setVisible( visible );
	line2->setVisible( visible );
	l_clustersShapes->setVisible( visible );
	l_clusters1Min->setVisible( visible );
	l_clusters1Max->setVisible( visible );
	b_clustersShapeBoundary_Slider->setVisible( visible );
	line3->setVisible( visible );
	// Britvik end

}

void CoreWindow::createMenus()
{
	file = menuBar()->addMenu( "File" );
	file->addAction( load );
	file->addAction( loadGraph );
	file->addSeparator();
	file->addAction( saveGraph );
	file->addAction( saveLayout );
	file->addSeparator();
	file->addAction( quit );

	edit = menuBar()->addMenu( "Settings" );
	edit->addAction( options );

	help = menuBar()->addMenu( "Help" );
	help->addAction( about );
}

QtColorPicker* CoreWindow::createColorPicker()
{
	QtColorPicker* colorPicker = new QtColorPicker();
	colorPicker->setStandardColors();
	connect( colorPicker,SIGNAL( colorChanged( const QColor& ) ),this,SLOT( colorPickerChanged( const QColor& ) ) );

	return colorPicker;
}

QWidget* CoreWindow::createGraphTab( QFrame* line )
{
	QtColorPicker* colorPicker = createColorPicker();
	createGraphSlider();
	createSelectionComboBox();

	QWidget* wGraph = new QWidget();

	QFormLayout* lGraph = new QFormLayout( wGraph );
	lGraph->setContentsMargins( 1,1,1,1 );
	lGraph->setSpacing( 2 );
	lGraph->setAlignment( Qt::AlignHCenter );

	noSelect->setMinimumWidth( 68 );
	singleSelect->setMaximumWidth( 68 );
	lGraph->addRow( noSelect,singleSelect );
	//multiSelect->setMinimumWidth(68);
	lGraph->addRow( multiSelect,center );
	multiSelect->setMinimumWidth( 68 );
	center->setMaximumWidth( 68 );
	lGraph->addRow( selectionTypeComboBox );
	selectionTypeComboBox->setMaximumWidth( 136 );
	line = createLine();
	lGraph->addRow( line );
	addMeta->setMinimumWidth( 68 );
	removeMeta->setMaximumWidth( 68 );
	lGraph->addRow( addMeta,removeMeta );
	fix->setMinimumWidth( 68 );
	unFix->setMaximumWidth( 68 );
	lGraph->addRow( fix,unFix );
	line = createLine();
	lGraph->addRow( line );
	add_Edge->setMaximumWidth( 136 );
	lGraph->addRow( add_Edge );
	add_Node->setMaximumWidth( 136 );
	lGraph->addRow( add_Node );
	remove_all->setMaximumWidth( 136 );
	lGraph->addRow( remove_all );
	line = createLine();
	lGraph->addRow( line );
	colorPicker->setMaximumWidth( 136 );
	applyColor->setMaximumWidth( 136 );
	lGraph->addRow( colorPicker );
	lGraph->addRow( applyColor );
	line = createLine();
	lGraph->addRow( line );
	le_applyLabel->setMaximumWidth( 136 );
	lGraph->addRow( le_applyLabel );
	applyLabel->setMaximumWidth( 136 );
	lGraph->addRow( applyLabel );
	label->setMaximumWidth( 136 );
	lGraph->addRow( label );
	line = createLine();
	lGraph->addRow( line );
	play->setMaximumWidth( 136 );
	lGraph->addRow( play );
	slider->setMaximumWidth( 136 );
	lGraph->addRow( slider );
	line = createLine();
	lGraph->addRow( line );

	nodeTypeComboBox->setMaximumWidth( 136 );
	lGraph->addRow( nodeTypeComboBox );
	edgeTypeComboBox->setMaximumWidth( 136 );
	lGraph->addRow( edgeTypeComboBox );

	wGraph->setLayout( lGraph );

	return wGraph;
}

QFormLayout* CoreWindow::createGroupLayout()
{
	QFormLayout* vbox = new QFormLayout;
	vbox->setContentsMargins( 1,1,1,1 );
	return vbox;
}

QGroupBox* CoreWindow::createGroupBox( QString name )
{
	QGroupBox* group = new QGroupBox( name );
	return group;
}

QGroupBox* CoreWindow::sphereAndPlaneGroup()
{
	QGroupBox* group = createGroupBox( tr( "Sphere and plane" ) );
	QFormLayout* layoutGroup = createGroupLayout();
	b_SetRestriction_SphereSurface->setMinimumWidth( 68 );
	layoutGroup->addRow( b_SetRestriction_SphereSurface,b_SetRestriction_Sphere );
	b_SetRestriction_Plane->setMinimumWidth( 68 );
	layoutGroup->addRow( b_SetRestriction_Plane,b_SetRestriction_SpherePlane );
	group->setLayout( layoutGroup );

	return group;
}

QGroupBox* CoreWindow::circleAndConeGroup()
{
	QGroupBox* group = createGroupBox( tr( "Circle and cone" ) );
	QFormLayout* layoutGroup = createGroupLayout();
	b_SetRestriction_Circle->setMinimumWidth( 68 );
	layoutGroup->addRow( b_SetRestriction_Circle,b_SetRestriction_Cone );
	b_SetRestriction_ConeTree->setMinimumWidth( 68 );
	layoutGroup->addRow( b_SetRestriction_ConeTree,b_UnsetRestriction );
	group->setLayout( layoutGroup );

	return group;
}

QGroupBox* CoreWindow::cylinderAndConeSurfaceGroup()
{
	QGroupBox* group = createGroupBox( tr( "Cylinder and cone surface" ) );
	QFormLayout* layoutGroup = createGroupLayout();
	b_SetRestriction_CylinderSurface->setMinimumWidth( 68 );
	layoutGroup->addRow( b_SetRestriction_CylinderSurface,b_SetRestriction_CylinderSurface_SpinBox );
	b_SetRestriction_ConeSurface->setMinimumWidth( 68 );
	layoutGroup->addRow( b_SetRestriction_ConeSurface, b_SetRestriction_ConeSurface_SpinBox );
	group->setLayout( layoutGroup );

	return group ;
}

QGroupBox* CoreWindow::radialLayoutGroup()
{
	QGroupBox* group = createGroupBox( tr( "Radial layout" ) );
	QFormLayout* layoutGroup = createGroupLayout();
	//volovar_zac
	b_SetRestriction_RadialLayout->setMinimumWidth( 68 );
	layoutGroup->addRow( b_SetRestriction_RadialLayout,b_drawMethod_RadialLayout );
	layoutGroup->addRow( b_mode_RadialLayout );
	layoutGroup->addRow( b_SetRestriction_RadialLayout_Slider );
	connect( b_SetRestriction_RadialLayout_Slider,SIGNAL( valueChanged( int ) ),this,SLOT( RadialLayoutSizeChanged( int ) ) );
	layoutGroup->addRow( b_SetAlpha_RadialLayout_Slider );
	connect( b_SetAlpha_RadialLayout_Slider,SIGNAL( valueChanged( int ) ),this,SLOT( RadialLayoutAlphaChanged( int ) ) );
	layoutGroup->addRow( b_SetVisibleSpheres_RadialLayout_Slider );
	connect( b_SetVisibleSpheres_RadialLayout_Slider,SIGNAL( valueChanged( int ) ),this,SLOT( RadialLayoutSetVisibleSpheres( int ) ) );
	layoutGroup->addRow( b_SetForceScale_RadialLayout_Slider );
	connect( b_SetForceScale_RadialLayout_Slider,SIGNAL( valueChanged( int ) ),this,SLOT( RadialLayoutSetForceScale( int ) ) );
	layoutGroup->addRow( b_SetForceSphereScale_RadialLayout_Slider );
	connect( b_SetForceSphereScale_RadialLayout_Slider,SIGNAL( valueChanged( int ) ),this,SLOT( RadialLayoutSetForceSphereScale( int ) ) );
	//volovar_kon
	group->setLayout( layoutGroup );

	return group;
}

QWidget* CoreWindow::createConstraintsTab( QFrame* line )
{
	QWidget* wConstraints = new QWidget();
	QFormLayout* lConstraints = new QFormLayout( wConstraints );
	lConstraints->setContentsMargins( 1,1,1,1 );
	lConstraints->setSpacing( 2 );
	QGroupBox* group = NULL;

	group = sphereAndPlaneGroup();
	lConstraints->addRow( group );

	group = circleAndConeGroup();
	lConstraints->addRow( group );

	group = cylinderAndConeSurfaceGroup();
	lConstraints->addRow( group );

	group = radialLayoutGroup();
	lConstraints->addRow( group );

	line = createLine();
	lConstraints->addRow( line );
	chb_vertigo->setMaximumWidth( 136 );
	lConstraints->addRow( chb_vertigo );
	add_Distance->setMaximumWidth( 136 );
	lConstraints->addRow( add_Distance );
	subtract_Distance->setMaximumWidth( 136 );
	lConstraints->addRow( subtract_Distance );
	add_Planes->setMaximumWidth( 136 );
	lConstraints->addRow( add_Planes );
	remove_Planes->setMaximumWidth( 136 );
	lConstraints->addRow( remove_Planes );
	change_Forces->setMaximumWidth( 136 );
	lConstraints->addRow( change_Forces );

	line = createLine();
	lConstraints->addRow( line );
	b_UnsetRestrictionFromAll->setMaximumWidth( 136 );
	lConstraints->addRow( b_UnsetRestrictionFromAll );

	wConstraints->setLayout( lConstraints );

	return wConstraints;
}

QWidget* CoreWindow::createConnectionsTab( QFrame* line )
{
	QWidget* wManage = new QWidget();
	QFormLayout* lManage = new QFormLayout( wManage );
	lManage->setContentsMargins( 1,1,1,1 );
	lManage->setSpacing( 2 );

	lManage->addRow( new QLabel( "Nick:" ) );
	le_client_name->setMaximumWidth( 136 );
	lManage->addRow( le_client_name );
	b_start_server->setMaximumWidth( 136 );
	lManage->addRow( b_start_server );
	lManage->addRow( new QLabel( "Host:" ) );
	le_server_addr->setMaximumWidth( 136 );
	lManage->addRow( le_server_addr );
	b_start_client->setMaximumWidth( 136 );
	lManage->addRow( b_start_client );
	line = createLine();
	lManage->addRow( line );
	lManage->addRow( new QLabel( "Collaborators: " ) );
	lManage->addRow( lw_users );
	lw_users->setMaximumWidth( 136 );
	lManage->addRow( chb_spy );
	lManage->addRow( chb_center );
	lManage->addRow( chb_attention );
	line = createLine();
	lManage->addRow( line );
	lManage->addRow( new QLabel( "Avatar scale" ) );
	sl_avatarScale->setMaximumWidth( 136 );
	lManage->addRow( sl_avatarScale );

	wManage->setLayout( lManage );

	return wManage;
}

QWidget* CoreWindow::createClusteringTab( QFrame* line )
{
	QWidget* wClustering = new QWidget();
	QFormLayout* lClustering = new QFormLayout( wClustering );
	lClustering->setContentsMargins( 1,1,1,1 );
	lClustering->setSpacing( 2 );

	merge->setMinimumWidth( 68 );
	separate->setMaximumWidth( 68 );
	lClustering->addRow( merge,separate );
	cb_clusteringAlgorithm->setMaximumWidth( 136 );
	lClustering->addRow( cb_clusteringAlgorithm );
	lClustering->addRow( new QLabel( "Depth: " ), le_clusteringDepth );
	b_cluster_nodes->setMaximumWidth( 136 );
	lClustering->addRow( b_cluster_nodes );
	l_clustersOpacity->setMaximumWidth( 136 );
	lClustering->addRow( l_clustersOpacity );
	chb_clustersOpacity->setMaximumWidth( 136 );
	lClustering->addRow( chb_clustersOpacity );
	chb_clusterSelectedOpacity->setMaximumWidth( 136 );
	lClustering->addRow( chb_clusterSelectedOpacity );
	lClustering->addRow( line1 );
	b_clustersOpacity_Slider->setMaximumWidth( 136 );
	lClustering->addRow( b_clustersOpacity_Slider );
	lClustering->addRow( line2 );
	l_clustersShapes->setMaximumWidth( 136 );
	lClustering->addRow( l_clustersShapes );
	lClustering->addRow( l_clusters1Min );
	b_clustersShapeBoundary_Slider->setMaximumWidth( 136 );
	lClustering->addRow( b_clustersShapeBoundary_Slider );
	lClustering->addRow( l_clusters1Max );
	b_SetRestriction_Cube_Selected->setMaximumWidth( 136 );
	lClustering->addRow( b_SetRestriction_Cube_Selected );
	lClustering->addRow( line3 );
	b_restartLayouting->setMaximumWidth( 136 );
	lClustering->addRow( b_restartLayouting );
	l_repulsiveForceInsideCluster->setMaximumWidth( 136 );
	lClustering->addRow( l_repulsiveForceInsideCluster );
	sb_repulsiveForceInsideCluster->setMaximumWidth( 136 );
	lClustering->addRow( sb_repulsiveForceInsideCluster );

	line = createLine();
	lClustering->addRow( line );
	b_StartEdgeBundling->setMaximumWidth( 136 );
	lClustering->addRow( b_StartEdgeBundling, b_EdgeBundling_SpinBox );
	edgeBundlingSlider->setMaximumWidth( 136 );
	lClustering->addRow( edgeBundlingSlider );
	line = createLine();

	wClustering->setLayout( lClustering );

	return wClustering;
}

QWidget* CoreWindow::createMoreFeaturesTab( QFrame* line )
{
	QWidget* wMore = new QWidget();
	QFormLayout* lMore = new QFormLayout( wMore );
	lMore->setContentsMargins( 1,1,1,1 );
	lMore->setSpacing( 2 );

#ifdef OPENCV_FOUND
	b_start_face = new QPushButton( tr( "Start camera" ) );
	lMore->addRow( new QLabel( tr( "Face & Marker detection" ) ) );
	b_start_face->setMaximumWidth( 136 );
	lMore->addRow( b_start_face );
	connect( b_start_face, SIGNAL( clicked() ), this, SLOT( create_facewindow() ) );
#endif
	chb_camera_rot = new QCheckBox( tr( "Camera rotation" ) );
	chb_camera_rot->setChecked( true );
	chb_camera_rot->setMaximumWidth( 136 );
	lMore->addRow( chb_camera_rot );
	// dont rotate camera if video background
	if ( Util::ApplicationConfig::get()->getValue( "Viewer.SkyBox.Noise" ).toInt() == 2 ) {
		chb_camera_rot->setChecked( false );
	}
	else {
		chb_camera_rot->setChecked( true );
	}
#ifdef OPENCV_FOUND

#ifdef OPENNI2_FOUND
	line = createLine();
	lMore->addRow( line );
	lMore->addRow( new QLabel( tr( "Kinect" ) ) );
	b_start_kinect = new QPushButton();
	b_start_kinect->setText( "Start kinect" );
	b_start_kinect->setMaximumWidth( 136 );
	lMore->addRow( b_start_kinect );
	connect( b_start_kinect, SIGNAL( clicked() ), this, SLOT( createKinectWindow() ) );
	b_start_ransac = new QPushButton();
	b_start_ransac->setText( "Start calculate surface" );
	b_start_ransac->setMaximumWidth( 136 );
	lMore->addRow( b_start_ransac );
	connect( b_start_ransac, SIGNAL( clicked() ), this, SLOT( calculateRansac() ) );
#endif
#endif

#ifdef SPEECHSDK_FOUND
	line = createLine();
	lMore->addRow( line );
	lMore->addRow( new QLabel( tr( "Speech" ) ) );
	b_start_speech = new QPushButton();
	b_start_speech->setText( "Start Speech" );
	b_start_speech->setMaximumWidth( 136 );
	lMore->addRow( b_start_speech );
	connect( b_start_speech, SIGNAL( clicked() ), this, SLOT( startSpeech() ) );
#endif

#ifdef FGLOVE_FOUND
	line = createLine();
	lMore->addRow( line );
	lMore->addRow( new QLabel( tr( "5DT Gloves" ) ) );
	b_start_gloves = new QPushButton();
	b_start_gloves->setText( "Start Gloves" );
	b_start_gloves->setMaximumWidth( 136 );
	lMore->addRow( b_start_gloves );
	connect( b_start_gloves, SIGNAL( clicked() ), this, SLOT( startGlovesRecognition() ) );
#endif

	wMore->setLayout( lMore );

	return wMore;
}

void CoreWindow::createGraphSlider()
{
	slider = new QSlider( Qt::Horizontal,this );
	slider->setTickPosition( QSlider::TicksAbove );
	slider->setTickInterval( 5 );
	slider->setValue( 5 );
	slider->setFocusPolicy( Qt::NoFocus );
	connect( slider,SIGNAL( valueChanged( int ) ),this,SLOT( sliderValueChanged( int ) ) );
}

void CoreWindow::createSelectionComboBox()
{
	selectionTypeComboBox = new QComboBox();
	selectionTypeComboBox->insertItems( 0,( QStringList() << "All" << "Node" << "Edge" << "Cluster" ) );
	selectionTypeComboBox->setFocusPolicy( Qt::NoFocus );
	connect( selectionTypeComboBox,SIGNAL( currentIndexChanged( int ) ),this,SLOT( selectionTypeComboBoxChanged( int ) ) );
}

void CoreWindow::createLeftToolBar()
{
	QFrame* line = NULL;

	QWidget* wGraph = createGraphTab( line );

	QWidget* wConstraints = createConstraintsTab( line );

	QWidget* wManage = createConnectionsTab( line );

	QWidget* wClustering = createClusteringTab( line );

	QWidget* wMore = createMoreFeaturesTab( line );

	toolBox = new QToolBox();
	toolBox->setSizePolicy( QSizePolicy( QSizePolicy::Maximum, QSizePolicy::Ignored ) );
	toolBox->setMinimumWidth( 163 );
	toolBox->addItem( wGraph, tr( "Graph" ) );
	toolBox->addItem( wConstraints, tr( "Constraints" ) );
	toolBox->addItem( wClustering, tr( "Clustering" ) );
	toolBox->addItem( wManage, tr( "Connections" ) );
	toolBox->addItem( wMore, tr( "More features" ) );
	toolBar = new QToolBar( "Tools",this );

	QFrame* frame = createHorizontalFrame();
	frame->layout()->addWidget( toolBox );
	toolBar->addWidget( frame );

	addToolBar( Qt::LeftToolBarArea,toolBar );
	toolBar->setMaximumWidth( 200 );
	toolBar->setMovable( false );

	/*toolBar->addWidget(le_message);
	toolBar->addWidget(b_send_message); */
}

QFrame* CoreWindow::createLine()
{
	QFrame* line = new QFrame();
	line->setFrameShape( QFrame::HLine );
	line->setFrameShadow( QFrame::Sunken );
	line->setMinimumHeight( 15 );
	line->setMaximumWidth( 138 );

	return line;
}

QFrame* CoreWindow::createHorizontalFrame()
{
	QFrame* frame = new QFrame();
	QHBoxLayout* layout = new QHBoxLayout();
	frame->setLayout( layout );
	layout->setMargin( 0 );
	layout->setSpacing( 0 );

	return frame;
}

void CoreWindow::addSQLInput()
{
	if ( dock->isVisible() ) {
		dock->hide();
	}
	else {
		dock->show();
	}
}

void CoreWindow::showOptions()
{
	OptionsWindow* options = new OptionsWindow( coreGraph, viewerWidget );
	options->show();
}

void CoreWindow::showLoadGraph()
{
	LoadGraphWindow* loadGraph = new LoadGraphWindow( this );
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
	QString layout_name = QInputDialog::getText( this, tr( "New layout name" ),
						  tr( "Layout name:" ),
						  QLineEdit::Normal, "", &ok );


	// save layout
	if ( ok && !layout_name.isEmpty() ) {
		Manager::GraphManager::getInstance()->saveActiveLayoutToDB( layout_name );

	}
	else {
		qDebug() << "[QOSG::CoreWindow::saveLayoutToDB()] Input dialog canceled";
	}
}

void CoreWindow::sqlQuery()
{
	std::cout << lineEdit->text().toStdString() << endl;
}

void CoreWindow::playPause()
{
	if ( isPlaying ) {
		play->setIcon( QIcon( "../share/3dsoftviz/img/gui/play.png" ) );
		isPlaying = 0;
		layout->pause();
		coreGraph->setNodesFreezed( true );
	}
	else {
		play->setIcon( QIcon( "../share/3dsoftviz/img/gui/pause.png" ) );
		isPlaying = 1;
		coreGraph->setNodesFreezed( false );
		layout->play();
	}
}

void CoreWindow::noSelectClicked( bool checked )
{
	viewerWidget->getPickHandler()->setPickMode( Vwr::PickHandler::PickMode::NONE );
	singleSelect->setChecked( false );
	multiSelect->setChecked( false );
	center->setChecked( false );
	noSelect->setChecked( checked );
}

void CoreWindow::singleSelectClicked( bool checked )
{
	viewerWidget->getPickHandler()->setPickMode( Vwr::PickHandler::PickMode::SINGLE );
	noSelect->setChecked( false );
	multiSelect->setChecked( false );
	center->setChecked( false );
	singleSelect->setChecked( checked );
}

void CoreWindow::multiSelectClicked( bool checked )
{
	viewerWidget->getPickHandler()->setPickMode( Vwr::PickHandler::PickMode::MULTI );
	noSelect->setChecked( false );
	singleSelect->setChecked( false );
	center->setChecked( false );
	multiSelect->setChecked( checked );
}

void CoreWindow::centerView( bool checked )
{
	noSelect->setChecked( false );
	singleSelect->setChecked( false );
	multiSelect->setChecked( false );
	center->setChecked( checked );

	viewerWidget->getCameraManipulator()->setCenterSmoothly( viewerWidget->getPickHandler()->getSelectionCenterNnE() );
}


void CoreWindow::addMetaNode()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {

		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter( true );

		QString metaNodeName = "metaNode";
		QString metaEdgeName = "metaEdge";

		osg::GraphicsContext* gc = viewerWidget->getCamera()->getGraphicsContext();
		osgViewer::GraphicsWindow* gw = dynamic_cast<osgViewer::GraphicsWindow*>( gc );

		int x, y, width, height;

		gw->getWindowRectangle( x, y, width, height );

		std::cout << width << " " << height << "\n";


		osg::ref_ptr<Data::Node> metaNode = NULL;
		QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();

		Network::Client* client = Network::Client::getInstance();
		if ( !client->isConnected() ) {

			QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selectedNodes->constBegin();
			metaNode = currentGraph->addNode( metaNodeName, currentGraph->getNodeMetaType(), position );
			while ( i != selectedNodes->constEnd() ) {
				Data::Edge* e = currentGraph->addEdge( metaEdgeName, ( *i ), metaNode, currentGraph->getEdgeMetaType(), true );
				e->setCamera( viewerWidget->getCamera() );
				++i;
			}
		}
		else {
			client->sendAddMetaNode( metaNodeName,selectedNodes,metaEdgeName,position );
		}

		Network::Server* server = Network::Server::getInstance();
		if ( server->isListening() && metaNode != NULL ) {
			server->sendAddMetaNode( metaNode,selectedNodes,metaEdgeName,position );
		}

		if ( isPlaying ) {
			layout->play();
		}
	}
}

void CoreWindow::fixNodes()
{
	viewerWidget->getPickHandler()->toggleSelectedNodesFixedState( true );
}

void CoreWindow::unFixNodes()
{
	viewerWidget->getPickHandler()->toggleSelectedNodesFixedState( false );

	if ( isPlaying ) {
		layout->play();
	}
}

void CoreWindow::mergeNodes()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter( true );
		QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();

		if ( selectedNodes->count() > 0 ) {
			Network::Server* server = Network::Server::getInstance();
			Network::Client* client = Network::Client::getInstance();
			osg::ref_ptr<Data::Node> mergeNode = NULL;
			if ( !client->isConnected() ) {
				mergeNode = currentGraph->mergeNodes( selectedNodes, position );
			}
			if ( server->isListening() && mergeNode != NULL ) {
				server->sendMergeNodes( selectedNodes, position, mergeNode->getId() );
			}
			else {
				client->sendMergeNodes( selectedNodes, position );
			}
		}
		else {
			qDebug() << "[QOSG::CoreWindow::mergeNodes] There are no nodes selected";
		}

		viewerWidget->getPickHandler()->unselectPickedEdges( 0 );
		viewerWidget->getPickHandler()->unselectPickedNodes( 0 );

		if ( isPlaying ) {
			layout->play();
		}
	}
}

void CoreWindow::separateNodes()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {
		QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();

		if ( selectedNodes->count() > 0 ) {
			Network::Server* server = Network::Server::getInstance();
			Network::Client* client = Network::Client::getInstance();
			if ( !client->isConnected() ) {
				currentGraph->separateNodes( selectedNodes );
			}

			if ( server->isListening() ) {
				server->sendSeparateNodes( selectedNodes );
			}
			else if ( client->isConnected() ) {
				client->sendSeparateNodes( selectedNodes );
			}
		}
		else {
			qDebug() << "[QOSG::CoreWindow::separateNodes] There are no nodes selected";
		}

		if ( isPlaying ) {
			layout->play();
		}
	}
}

void CoreWindow::removeMetaNodes()
{
	QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selectedNodes->constBegin();

	while ( i != selectedNodes->constEnd() ) {
		//treba este opravit - zatial kontrolujeme ci to nie je mergedNode len podla mena
		if ( ( *i )->getType()->isMeta() && ( *i )->getName() != "mergedNode" ) {
			Network::Server* server = Network::Server::getInstance();
			Network::Client* client = Network::Client::getInstance();
			if ( !client->isConnected() ) {
				currentGraph->removeNode( ( *i ) );
			}
			else {
				client->sendRemoveNode( ( *i )->getId() );
			}
			if ( server->isListening() ) {
				server->sendRemoveNode( ( *i )->getId() );
			}

		}
		++i;
	}

	if ( isPlaying ) {
		layout->play();
	}
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
	layout->pauseAllAlg();
	coreGraph->setNodesFreezed( true );

	QString fileName =NULL;

	if ( dialog.exec() ) {
		QStringList filenames = dialog.selectedFiles();
		fileName = filenames.at( 0 );
	}

	if ( fileName != NULL ) {
		Manager::GraphManager::getInstance()->loadGraph( fileName );

		viewerWidget->getCameraManipulator()->home();
	}

	//treba overit ci funguje
	if ( isPlaying ) {
		layout->play();
		coreGraph->setNodesFreezed( false );
	}

}

void CoreWindow::labelOnOff( bool )
{
	if ( viewerWidget->getPickHandler()->getSelectionType() == Vwr::PickHandler::SelectionType::EDGE ) {
		edgeLabelsVisible = !edgeLabelsVisible;
		coreGraph->setEdgeLabelsVisible( edgeLabelsVisible );
	}
	else if ( viewerWidget->getPickHandler()->getSelectionType() == Vwr::PickHandler::SelectionType::NODE ) {
		nodeLabelsVisible = !nodeLabelsVisible;
		coreGraph->setNodeLabelsVisible( nodeLabelsVisible );
	}
	else {
		bool state = edgeLabelsVisible & nodeLabelsVisible;

		nodeLabelsVisible = edgeLabelsVisible = !state;

		coreGraph->setEdgeLabelsVisible( !state );
		coreGraph->setNodeLabelsVisible( !state );
	}
}

void CoreWindow::sliderValueChanged( int value )
{
	layout->setAlphaValue( static_cast<float>( value ) * 0.001f );
}

//Volovar zac

void CoreWindow::RadialLayoutSizeChanged( int value )
{
	//notify radial layout that size was changed
	Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
	if ( selectedRadialLayout != NULL ) {
		selectedRadialLayout->changeSize( static_cast<float>( value ) );
	}
}

void CoreWindow::RadialLayoutAlphaChanged( int value )
{
	//notify radial layout that alpha channel was changed
	Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
	//qDebug()<<"Value: "<<value<<", selected: "<<selectedRadialLayout;
	if ( selectedRadialLayout != NULL ) {
		selectedRadialLayout->setAlpha( static_cast<float>( value )/500.0f );
	}
}

void CoreWindow::RadialLayoutSetVisibleSpheres( int value )
{
	//notify radial layout that number of visibles spheres was changed
	Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
	if ( selectedRadialLayout != NULL ) {
		selectedRadialLayout->setVisibleSpheres( static_cast<float>( value )/100 );
	}
}

void CoreWindow::RadialLayoutSetForceScale( int value )
{
	//notify that multiplier of repulsive forces in radial layout was changed
	Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
	if ( selectedRadialLayout != NULL ) {
		selectedRadialLayout->setForceScale( static_cast<float>( value ) );
	}
}

void CoreWindow::RadialLayoutSetForceSphereScale( int value )
{
	//notify that multiplier of repulsive forces in radial layout on same layer was changed
	Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
	if ( selectedRadialLayout != NULL ) {
		selectedRadialLayout->setForceSphereScale( static_cast<float>( value ) );
	}
}

void CoreWindow::changeDrawMethod_RadialLayout()
{
	//notify that drawing method (WIREFRAME/SOLID) was changed
	Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
	if ( selectedRadialLayout != NULL ) {
		selectedRadialLayout->changeRenderType();
	}
}

void CoreWindow::changeMode_RadialLayout()
{
	//notify that mode of radial layout was changed (2D/3D)
	Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
	if ( selectedRadialLayout != NULL ) {
		selectedRadialLayout->changeMode();
	}
}

//Volovar koniec


void CoreWindow::colorPickerChanged( const QColor& color )
{
	this->color = color;
}

void CoreWindow::selectionTypeComboBoxChanged( int index )
{
	switch ( index ) {
		case 0:
			viewerWidget->getPickHandler()->setSelectionType( Vwr::PickHandler::SelectionType::ALL );
			label->setChecked( edgeLabelsVisible & nodeLabelsVisible );
			break;
		case 1:
			viewerWidget->getPickHandler()->setSelectionType( Vwr::PickHandler::SelectionType::NODE );
			label->setChecked( nodeLabelsVisible );
			break;
		case 2:
			viewerWidget->getPickHandler()->setSelectionType( Vwr::PickHandler::SelectionType::EDGE );
			label->setChecked( edgeLabelsVisible );
			break;
		case 3:
			viewerWidget->getPickHandler()->setSelectionType( Vwr::PickHandler::SelectionType::CLUSTER );
			label->setChecked( edgeLabelsVisible & nodeLabelsVisible );
			break;
		default:
			qDebug() << "CoreWindow:selectionTypeComboBoxChanged do not suported index";
			break;

	}
}

void CoreWindow::nodeTypeComboBoxChanged( int index )
{
	switch ( index ) {
		case 0:
			coreGraph->setNodeVisual( Data::Node::INDEX_SQUARE );
			break;
		case 1:
			coreGraph->setNodeVisual( Data::Node::INDEX_SPHERE );
			break;
		default:
			qDebug() << "CoreWindow:nodeTypeComboBoxChanged do not suported index";
			break;

	}
}

void CoreWindow::edgeTypeComboBoxChanged( int index )
{
	switch ( index ) {
		case 0:
			coreGraph->setEdgeVisual( Data::Edge::INDEX_QUAD );
			break;
		case 1:
			coreGraph->setEdgeVisual( Data::Edge::INDEX_CYLINDER );
			break;
		case 2:
			coreGraph->setEdgeVisual( Data::Edge::INDEX_LINE );
			break;
		case 3:
			coreGraph->setEdgeVisual( Data::Edge::INDEX_CURVE );
			break;
		default:
			qDebug() << "CoreWindow:edgeTypeComboBoxChanged do not suported index";
			break;

	}
}

void CoreWindow::applyColorClick()
{
	float alpha = static_cast<float>( color.alphaF() );
	float red = static_cast<float>( color.redF() );
	float green = static_cast<float>( color.greenF() );
	float blue = static_cast<float>( color.blueF() );

	QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator ni = selectedNodes->constBegin();

	Network::Server* server = Network::Server::getInstance();
	while ( ni != selectedNodes->constEnd() ) {
		if ( client->isConnected() ) {
			client->sendNodeColor( ( *ni )->getId(), red, green, blue, alpha );
		}
		else {
			( *ni )->setColor( osg::Vec4( red, green, blue, alpha ) );
			server->sendNodeColor( ( *ni )->getId(), red, green, blue, alpha );
		}
		++ni;
	}

	QLinkedList<osg::ref_ptr<Data::Edge> >* selectedEdges = viewerWidget->getPickHandler()->getSelectedEdges();
	QLinkedList<osg::ref_ptr<Data::Edge> >::const_iterator ei = selectedEdges->constBegin();

	while ( ei != selectedEdges->constEnd() ) {
		//ak je edge skryta, nebudeme jej menit farbu
		//(*ei)->getScale() != 0
		if ( !( *ei )->getIsInvisible() ) {

			if ( client->isConnected() ) {
				client->sendNodeColor( ( *ei )->getId(), red, green, blue, alpha );
			}
			else {
				( *ei )->setEdgeColor( osg::Vec4( red, green, blue, alpha ) );
				server->sendNodeColor( ( *ei )->getId(), red, green, blue, alpha );
			}

		}
		++ei;
	}
}

void CoreWindow::applyLabelClick()
{

	Network::Server* server = Network::Server::getInstance();

	QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator ni = selectedNodes->constBegin();
	QString newLabel = le_applyLabel->text();

	while ( ni != selectedNodes->constEnd() ) {
		if ( client->isConnected() ) {
			client->sendNodeLabel( ( *ni )->getId(), newLabel );
		}
		else {
			( *ni )->setName( newLabel );
			( *ni )->setLabelText( newLabel );
			( *ni )->reloadConfig();
			server->sendNodeLabel( ( *ni )->getId(), newLabel );
		}
		++ni;
	}
}

void CoreWindow::setRestriction_CylinderSurface()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {
		//osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

		//osg::ref_ptr<Data::Node> centerNode = currentGraph->addRestrictionNode (QString ("center"), position);
		//osg::ref_ptr<Data::Node> surfaceNode = currentGraph->addRestrictionNode (QString ("surface"), position + osg::Vec3f (10, 0, 0));

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;
		//restrictionNodes.push_back (centerNode);
		//restrictionNodes.push_back (surfaceNode);

		setRestrictionToAllNodes(
			QSharedPointer<Layout::ShapeGetter> (
				new Layout::ShapeGetter_CylinderSurface_ByCamera( viewerWidget, *b_SetRestriction_CylinderSurface_SpinBox ) ),
			currentGraph,
			QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
				new Layout::RestrictionRemovalHandler_RestrictionNodesRemover(
					*currentGraph,
					restrictionNodes
				)
			)
		);
	}
}

void CoreWindow::setRestriction_SphereSurface()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter( true );
		if ( qFuzzyCompare( static_cast<float>( position.length() ),0.0f ) ) {
			return;
		}
		osg::ref_ptr<Data::Node> centerNode;
		osg::ref_ptr<Data::Node> surfaceNode;

		QString name_centerNode = "center";
		QString name_sufraceNode = "surface";
		osg::Vec3 positionNode1 = position;
		osg::Vec3 positionNode2 = position + osg::Vec3f( 10, 0, 0 );

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;
		Network::Client* client = Network::Client::getInstance();

		if ( !client->isConnected() ) {

			centerNode = currentGraph->addRestrictionNode( name_centerNode, positionNode1 );
			surfaceNode = currentGraph->addRestrictionNode( name_sufraceNode, positionNode2 );

			restrictionNodes.push_back( centerNode );
			restrictionNodes.push_back( surfaceNode );

			setRestrictionToSelectedNodes(
				QSharedPointer<Layout::ShapeGetter> (
					new Layout::ShapeGetter_SphereSurface_ByTwoNodes( centerNode, surfaceNode )
				),
				currentGraph,
				QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
					new Layout::RestrictionRemovalHandler_RestrictionNodesRemover(
						*currentGraph,
						restrictionNodes
					)
				)
			);
		}
		else {
			client->sendSetRestriction( 1,name_centerNode,positionNode1,name_sufraceNode,positionNode2, viewerWidget->getPickHandler()->getSelectedNodes() );
		}

		Network::Server* server = Network::Server::getInstance();
		server->sendSetRestriction( 1, centerNode, positionNode1, surfaceNode, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes() );
	}
}

void CoreWindow::setRestriction_Sphere()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter( true );
		osg::ref_ptr<Data::Node> centerNode;
		osg::ref_ptr<Data::Node> surfaceNode;

		QString name_centerNode = "center";
		QString name_sufraceNode = "surface";
		osg::Vec3 positionNode1 = position;
		osg::Vec3 positionNode2 = position + osg::Vec3f( 10, 0, 0 );

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;

		Network::Client* client = Network::Client::getInstance();

		if ( !client->isConnected() ) {

			centerNode = currentGraph->addRestrictionNode( name_centerNode, positionNode1 );
			surfaceNode = currentGraph->addRestrictionNode( name_sufraceNode, positionNode2 );

			restrictionNodes.push_back( centerNode );
			restrictionNodes.push_back( surfaceNode );

			setRestrictionToSelectedNodes(
				QSharedPointer<Layout::ShapeGetter> (
					new Layout::ShapeGetter_Sphere_ByTwoNodes( centerNode, surfaceNode )
				),
				currentGraph,
				QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
					new Layout::RestrictionRemovalHandler_RestrictionNodesRemover(
						*currentGraph,
						restrictionNodes
					)
				)
			);
		}
		else {
			client->sendSetRestriction( 2,name_centerNode,positionNode1,name_sufraceNode, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes() );
		}
		Network::Server* server = Network::Server::getInstance();
		server->sendSetRestriction( 2, centerNode, positionNode1, surfaceNode, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes() );
	}
}

void CoreWindow::setRestriction_ConeSurface()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {
		//osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

		//osg::ref_ptr<Data::Node> centerNode = currentGraph->addRestrictionNode (QString ("center"), position);
		//osg::ref_ptr<Data::Node> surfaceNode = currentGraph->addRestrictionNode (QString ("surface"), position + osg::Vec3f (10, 0, 0));

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;
		//restrictionNodes.push_back (centerNode);
		//restrictionNodes.push_back (surfaceNode);

		setRestrictionToAllNodes(
			QSharedPointer<Layout::ShapeGetter> (
				new Layout::ShapeGetter_ConeSurface_ByCamera( viewerWidget, *b_SetRestriction_ConeSurface_SpinBox ) ),
			currentGraph,
			QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
				new Layout::RestrictionRemovalHandler_RestrictionNodesRemover(
					*currentGraph,
					restrictionNodes
				)
			)
		);
	}
}

// Duransky start - Upravena funkcia na vytvorenie obmedzenia - roviny pre ucely vertigo zoomu
Layout::ShapeGetter_Plane_ByThreeNodes* CoreWindow::setRestriction_Plane_Vertigo( QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict, int nOfPlane )
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {

		osg::ref_ptr<Data::Node> node1;
		osg::ref_ptr<Data::Node> node2;
		osg::ref_ptr<Data::Node> node3;

		QString name_node1 = "plane_node_1";
		QString name_node2 = "plane_node_2";
		QString name_node3 = "plane_node_3";

		osg::Vec3 rootPosition = osg::Vec3f( 0.f, 0.f, 0.f );

		// pozicia bodu zavisi od poradoveho cisla roviny
		osg::Vec3 position = rootPosition + osg::Vec3f( static_cast<float>( nOfPlane - 1 ) * static_cast<float>( vertigoPlanesDistance ), 0.f, 0.f );

		osg::Vec3 positionNode1 = position + osg::Vec3f( 0.f, -100.f, 0.f );
		osg::Vec3 positionNode2 = position + osg::Vec3f( 0.f, 100.f, 200.f );
		osg::Vec3 positionNode3 = position + osg::Vec3f( 0.f, 100.f, -200.f );

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;

		Network::Client* client = Network::Client::getInstance();

		if ( !client->isConnected() ) {

			node1 = currentGraph->addRestrictionNode( name_node1, positionNode1 );
			node2 = currentGraph->addRestrictionNode( name_node2, positionNode2 );
			node3 = currentGraph->addRestrictionNode( name_node3, positionNode3 );
			restrictionNodes.push_back( node1 );
			restrictionNodes.push_back( node2 );
			restrictionNodes.push_back( node3 );

			Layout::ShapeGetter_Plane_ByThreeNodes* plane = new Layout::ShapeGetter_Plane_ByThreeNodes( node1, node2, node3 );

			setRestrictionToSelectedNodes(
				QSharedPointer<Layout::ShapeGetter> ( plane ),currentGraph,
				QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
					new Layout::RestrictionRemovalHandler_RestrictionNodesRemover(
						*currentGraph,restrictionNodes
					)
				),
				nodesToRestrict
			);
			return plane;
		}
		else {
			// Tato cast pre sietovu komunikaciu nebola testovana
			client->sendSetRestriction( 3,name_node1,positionNode1,name_node2, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes(),name_node3,&positionNode3 );
		}
		Network::Server* server = Network::Server::getInstance();
		server->sendSetRestriction( 3, node1, positionNode1, node2, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes(), node3, &positionNode3 );
		return NULL;
	}
	return NULL;
}
// Duransky end - Upravena funkcia na vytvorenie obmedzenia - roviny pre ucely vertigo zoomu

void CoreWindow::setRestriction_Plane( QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict )
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter( true );

		osg::ref_ptr<Data::Node> node1;
		osg::ref_ptr<Data::Node> node2;
		osg::ref_ptr<Data::Node> node3;

		QString name_node1 = "plane_node_1";
		QString name_node2 = "plane_node_2";
		QString name_node3 = "plane_node_3";

		osg::Vec3 positionNode1 = position;
		osg::Vec3 positionNode2 = position + osg::Vec3f( 10, 0, 0 );
		osg::Vec3 positionNode3 = position + osg::Vec3f( 0, 10, 0 );

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;

		Network::Client* client = Network::Client::getInstance();

		if ( !client->isConnected() ) {

			node1 = currentGraph->addRestrictionNode( name_node1, positionNode1 );
			node2 = currentGraph->addRestrictionNode( name_node2, positionNode2 );
			node3 = currentGraph->addRestrictionNode( name_node3, positionNode3 );
			restrictionNodes.push_back( node1 );
			restrictionNodes.push_back( node2 );
			restrictionNodes.push_back( node3 );

			setRestrictionToSelectedNodes(
				QSharedPointer<Layout::ShapeGetter> (
					new Layout::ShapeGetter_Plane_ByThreeNodes( node1, node2, node3 )
				),
				currentGraph,
				QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
					new Layout::RestrictionRemovalHandler_RestrictionNodesRemover(
						*currentGraph,
						restrictionNodes
					)
				),
				nodesToRestrict
			);
		}
		else {
			client->sendSetRestriction( 3,name_node1,positionNode1,name_node2, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes(),name_node3,&positionNode3 );
		}
		Network::Server* server = Network::Server::getInstance();
		server->sendSetRestriction( 3, node1, positionNode1, node2, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes(), node3, &positionNode3 );
	}
}

void CoreWindow::setRestriction_SpherePlane( QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict )
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {

		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter( true );

		osg::ref_ptr<Data::Node> node1;
		osg::ref_ptr<Data::Node> node2;
		osg::ref_ptr<Data::Node> node3;

		QString name_node1 = "sphere_center_node";
		QString name_node2 = "sphere_surface_node";
		QString name_node3 = "plane_node_3";

		osg::Vec3 positionNode1 = position;
		osg::Vec3 positionNode2 = position + osg::Vec3f( 10, 0, 0 );
		osg::Vec3 positionNode3 = position + osg::Vec3f( 0, 10, 0 );

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;

		Network::Client* client = Network::Client::getInstance();

		if ( !client->isConnected() ) {

			node1 = currentGraph->addRestrictionNode( name_node1, positionNode1 );
			node2 = currentGraph->addRestrictionNode( name_node2, positionNode2 );
			node3 = currentGraph->addRestrictionNode( name_node3, positionNode3 );
			restrictionNodes.push_back( node1 );
			restrictionNodes.push_back( node2 );
			restrictionNodes.push_back( node3 );



			setRestrictionToSelectedNodes(
				QSharedPointer<Layout::ShapeGetter> (
					new Layout::ShapeGetter_SpherePlane_ByThreeNodes( node1, node2, node3 )
				),
				currentGraph,
				QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
					new Layout::RestrictionRemovalHandler_RestrictionNodesRemover(
						*currentGraph,
						restrictionNodes
					)
				),
				nodesToRestrict
			);
		}
		else {
			client->sendSetRestriction( 3,name_node1,positionNode1,name_node2, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes(),name_node3,&positionNode3 );
		}
		Network::Server* server = Network::Server::getInstance();
		server->sendSetRestriction( 3, node1, positionNode1, node2, positionNode2, viewerWidget->getPickHandler()->getSelectedNodes(), node3, &positionNode3 );
	}
}

void CoreWindow::setRestriction_Circle( QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict,
										osg::ref_ptr<Data::Node> centerNode )
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {

		if ( nodesToRestrict == NULL ) {
			nodesToRestrict = viewerWidget->getPickHandler()->getSelectedNodes();
		}

		osg::Vec3 position;
		if ( centerNode == NULL ) {
			position = viewerWidget->getPickHandler()->getSelectionCenter( true );
		}
		else {
			position = centerNode->getTargetPosition();
		}

		osg::ref_ptr<Data::Node> node1;
		osg::ref_ptr<Data::Node> node2;
		osg::ref_ptr<Data::Node> node3;

		QString name_node1 = "circle_center_node";
		QString name_node2 = "circle_edge_node";
		QString name_node3 = "circle_plane_node";

		osg::Vec3 positionNode1 = position;
		osg::Vec3 positionNode2 = position + osg::Vec3f( 10, 0, 0 );
		osg::Vec3 positionNode3 = position + osg::Vec3f( 0, 20, 0 );

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;

		Network::Client* client = Network::Client::getInstance();

		if ( !client->isConnected() ) {

			if ( centerNode == NULL ) {
				node1 = currentGraph->addRestrictionNode( name_node1, positionNode1 );
			}
			else {
				node1 = centerNode;
			}
			node2 = currentGraph->addRestrictionNode( name_node2, positionNode2 );
			node3 = currentGraph->addRestrictionNode( name_node3, positionNode3 );
			node3->setInvisible( true );
			restrictionNodes.push_back( node1 );
			restrictionNodes.push_back( node2 );
			restrictionNodes.push_back( node3 );

			QLinkedList<osg::ref_ptr<Data::Node> > nodes;
			nodes.append( node1 );
			nodes.append( node2 );
			nodes.append( node3 );

			osg::ref_ptr<Data::Edge> radiusEdge = currentGraph->addEdge( "pomEdge", node1, node2, currentGraph->getEdgeMetaType(), true );
			radiusEdge->setScale( 0 );
			radiusEdge->setSharedCoordinates( false, false, true );

			setRestrictionToSelectedNodes(
				QSharedPointer<Layout::ShapeGetter> (
					new Layout::ShapeGetter_Circle_ByThreeNodes( node1, node2, node3 )
				),
				currentGraph,
				QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
					new Layout::RestrictionRemovalHandler_RestrictionNodesRemover(
						*currentGraph,
						restrictionNodes
					)
				),
				nodesToRestrict
			);
		}
		else {
			client->sendSetRestriction( 3,name_node1,positionNode1,name_node2, positionNode2, nodesToRestrict,name_node3,&positionNode3 );
		}
		Network::Server* server = Network::Server::getInstance();
		server->sendSetRestriction( 3, node1, positionNode1, node2, positionNode2,nodesToRestrict, node3, &positionNode3 );
	}
}

void CoreWindow::setRestriction_Cone( QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict,
									  osg::ref_ptr<Data::Node> parentNode )
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {
		if ( parentNode == NULL ) {
			parentNode =  viewerWidget->getPickHandler()->getPickedNodeWithMaxEdgeCount();
			parentNode->setFixed( true );
		}

		if ( nodesToRestrict == NULL ) {
			nodesToRestrict = viewerWidget->getPickHandler()->getSelectedNodes();
		}

		nodesToRestrict->removeOne( parentNode );


		osg::Vec3 positionCenter = parentNode->getTargetPosition() + osg::Vec3f( 0, 0, ( -50 ) );

		osg::ref_ptr<Data::Node> centernode = currentGraph->addRestrictionNode( "circle_center_node", positionCenter );

		osg::ref_ptr<Data::Edge> parentCircleEdge = currentGraph->addEdge( "pomEdge", parentNode, centernode, currentGraph->getEdgeMetaType(), true );
		parentCircleEdge->setInvisible( true );
		parentCircleEdge->setSharedCoordinates( true, true, false );

		setRestriction_Circle( nodesToRestrict, centernode );
	}

}


void CoreWindow::setRestriction_ConeTree()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	if ( currentGraph == NULL ) {
		return;
	}
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();

	osg::ref_ptr<Data::Node> rootNode = viewerWidget->getPickHandler()->getPickedNodeWithMaxEdgeCount();
	if ( rootNode == NULL ) {
		return;
	}

	osg::Vec3 centerPosition = viewerWidget->getPickHandler()->getSelectionCenter( true );
	osg::Vec3 rootPosition = centerPosition + osg::Vec3f( 0, 0, 100 );
	rootNode->setTargetPosition( rootPosition );
	rootNode->setFixed( true );

	Data::GraphSpanningTree* spanningTree = currentGraph->getSpanningTree( rootNode->getId() );
	QLinkedList<osg::ref_ptr<Data::Node> > pickedNodes;

	QList<qlonglong> groups = spanningTree->getAllGroups();
	QList<qlonglong>::iterator groupIt;
	for ( groupIt=groups.begin(); groupIt!=groups.end(); ++groupIt ) {
		if ( ( *groupIt ) == 0 ) {
			continue;
		}
		pickedNodes.clear();
		QList<qlonglong> nodes = spanningTree->getNodesInGroup( *groupIt );
		QList<qlonglong>::iterator nodeIt;
		for ( nodeIt=nodes.begin(); nodeIt!=nodes.end(); ++nodeIt ) {
			pickedNodes.append( allNodes->value( *nodeIt ) );
		}
		osg::ref_ptr<Data::Node> parentNode = allNodes->value( *groupIt );
		setRestriction_Cone( &pickedNodes,parentNode );
	}

	int maxDepth = spanningTree->getMaxDepth();
	for ( int depth=1; depth<=maxDepth; depth++ ) {
		pickedNodes.clear();
		QList<qlonglong> groups = spanningTree->getGroupsInDepth( depth );

		QList<qlonglong>::iterator groupIt;
		for ( groupIt=groups.begin(); groupIt!=groups.end(); ++groupIt ) {
			qlonglong nodeId = spanningTree->getRandomNodeInGroup( *groupIt );
			pickedNodes.append( allNodes->value( nodeId ) );

		}

		osg::Vec3 position = rootPosition + osg::Vec3f( 0.f, 0.f, ( -50.f ) * static_cast<float>( depth ) );
		osg::Vec3 positionNode1 = position;
		osg::Vec3 positionNode2 = position + osg::Vec3f( 10.f, 0.f, 0.f );
		osg::Vec3 positionNode3 = position + osg::Vec3f( 0.f, 10.f, 0.f );

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;

		osg::ref_ptr<Data::Node> node1 = currentGraph->addRestrictionNode( "plane_node_1", positionNode1 );
		osg::ref_ptr<Data::Node> node2 = currentGraph->addRestrictionNode( "plane_node_2", positionNode2 );
		osg::ref_ptr<Data::Node> node3 = currentGraph->addRestrictionNode( "plane_node_3", positionNode3 );
		node1->setInvisible( true );
		node2->setInvisible( true );
		node3->setInvisible( true );
		restrictionNodes.push_back( node1 );
		restrictionNodes.push_back( node2 );
		restrictionNodes.push_back( node3 );

		setRestrictionToShape( QSharedPointer<Layout::ShapeGetter> (
								   new Layout::ShapeGetter_Plane_ByThreeNodes( node1, node2, node3 )
							   ),
							   currentGraph,
							   QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
								   new Layout::RestrictionRemovalHandler_RestrictionNodesRemover(
									   *currentGraph,
									   restrictionNodes
								   )
							   ),
							   pickedNodes );

	}
}

void CoreWindow::unsetRestriction()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	Network::Client* client = Network::Client::getInstance();
	if ( !client->isConnected() ) {
		if ( currentGraph != NULL ) {
			setRestrictionToSelectedNodes(
				QSharedPointer<Layout::ShapeGetter> ( NULL ),
				currentGraph,
				QSharedPointer<Layout::RestrictionRemovalHandler> ( NULL )
			);
		}
	}
	else {
		client->sendUnSetRestriction( viewerWidget->getPickHandler()->getSelectedNodes() );
	}

	Network::Server* server = Network::Server::getInstance();
	server->sendUnSetRestriction( viewerWidget->getPickHandler()->getSelectedNodes() );
}

void CoreWindow::unsetRestrictionFromAll()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {
		setRestrictionToAllNodes(
			QSharedPointer<Layout::ShapeGetter> ( NULL ),
			currentGraph,
			QSharedPointer<Layout::RestrictionRemovalHandler> ( NULL )
		);
		//volovar_zac
		Layout::RadialLayout* selectedRadialLayout = Layout::RadialLayout::getSelectedRadialLayout();
		if ( selectedRadialLayout != NULL ) {
			selectedRadialLayout->unselectNodes();
		}
		//volovar_kon
	}
}

void CoreWindow::startEdgeBundling()
{
	//stop EB
	if ( isEBPlaying ) {
		layout->stopEdgeBundling();
		isEBPlaying = false;

		if ( isPlaying ) {
			play->setIcon( QIcon( "../share/3dsoftviz/img/gui/play.png" ) );
			isPlaying = 0;
			layout->pause();
			coreGraph->setNodesFreezed( true );
		}

		Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

		if ( currentGraph != NULL ) {
			currentGraph->restoreSplittedEdges();
		}
	}
	//start EB
	else {
		if ( isPlaying ) {
			isPlaying = 0;
			layout->pause();
		}

		Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

		if ( currentGraph != NULL ) {

			//select all nodes and fix them
			QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator iNode = currentGraph->getNodes()->begin();
			while ( iNode != currentGraph->getNodes()->end() ) {
				viewerWidget->getPickHandler()->addPickedNode( *iNode );
				iNode++;
			}
			fixNodes();

			//split edges
			int splitCount = static_cast<int>( b_EdgeBundling_SpinBox->value() );
			currentGraph->splitAllEdges( splitCount );

		}

		if ( !isPlaying ) {
			play->setIcon( QIcon( "../share/3dsoftviz/img/gui/pause.png" ) );
			isPlaying = 1;
			coreGraph->setNodesFreezed( false );
			layout->play();
		}

		layout->playEdgeBundling();
		isEBPlaying = true;
	}
}

void CoreWindow::edgeBundlingSliderValueChanged( int value )
{
	layout->setAlphaEdgeBundlingValue( static_cast<float>( value ) );
}

void CoreWindow::setRestrictionToSelectedNodes(
	QSharedPointer<Layout::ShapeGetter> shapeGetter,
	Data::Graph* currentGraph,
	QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler,
	QLinkedList<osg::ref_ptr<Data::Node> >* nodesToRestrict
)
{
	QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = ( nodesToRestrict == NULL ) ? viewerWidget->getPickHandler()->getSelectedNodes() : nodesToRestrict;

	QSet<Data::Node*> nodes;
	for ( QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator it = selectedNodes->constBegin(); it != selectedNodes->constEnd(); ++it ) {
		nodes.insert( it->get() );
	}

	currentGraph->getRestrictionsManager().setRestrictions( nodes, shapeGetter );

	if ( ( ! shapeGetter.isNull() ) && ( ! removalHandler.isNull() ) ) {
		currentGraph->getRestrictionsManager().setOrRunRestrictionRemovalHandler( shapeGetter, removalHandler );
	}

	if ( isPlaying ) {
		layout->play();
	}
}

void CoreWindow::setRestrictionToAllNodes(
	QSharedPointer<Layout::ShapeGetter> shapeGetter,
	Data::Graph* currentGraph,
	QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler
)
{
	QSet<Data::Node*> nodes;

	QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
	j = Manager::GraphManager::getInstance()->getActiveGraph()->getNodes()->begin();
	for ( int i = 0; i < Manager::GraphManager::getInstance()->getActiveGraph()->getNodes()->count(); ++i,++j ) {
		nodes.insert( j.value() );
	}

	currentGraph->getRestrictionsManager().setRestrictions( nodes, shapeGetter );

	if ( ( ! shapeGetter.isNull() ) && ( ! removalHandler.isNull() ) ) {
		currentGraph->getRestrictionsManager().setOrRunRestrictionRemovalHandler( shapeGetter, removalHandler );
	}

	if ( isPlaying ) {
		layout->play();
	}
}

/**Volovar zaciatok
 */
void CoreWindow::setRestriction_RadialLayout()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();
	osg::ref_ptr<Data::Node> rootNode = viewerWidget->getPickHandler()->getPickedNodeWithMinEdgeCount();
	osg::Vec3 rootPosition = viewerWidget->getPickHandler()->getSelectionCenter( true );
	if ( currentGraph == NULL ) {
		return;
	}
	Layout::RadialLayout* radialLayout = new Layout::RadialLayout( currentGraph, selectedNodes, 100, rootNode, rootPosition );
	radialLayout->select();
	viewerWidget->getCameraManipulator()->setCenter( rootPosition );
}
/*Volovar koniec
 */

void CoreWindow::setRestrictionToShape(
	QSharedPointer<Layout::ShapeGetter> shapeGetter,
	Data::Graph* currentGraph,
	QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler,
	QLinkedList<osg::ref_ptr<Data::Node> >  nodesOfShapeGettersToRestrict )
{

	currentGraph->getRestrictionsManager().setRestrictionToShape( &nodesOfShapeGettersToRestrict, shapeGetter );


	if ( ( ! shapeGetter.isNull() ) && ( ! removalHandler.isNull() ) ) {
		currentGraph->getRestrictionsManager().setOrRunRestrictionRemovalHandler( shapeGetter, removalHandler );
	}

	if ( isPlaying ) {
		layout->play();
	}
}

bool CoreWindow::add_EdgeClick()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator ni = selectedNodes->constBegin();

	if (
		selectedNodes==NULL
	) {
		AppCore::Core::getInstance()->messageWindows->showMessageBox( "Upozornenie","Ziadny uzol oznaceny",false );
		return false;
	}

	osg::ref_ptr<Data::Node> node1, node2;
	int i=0;

	while ( ni != selectedNodes->constEnd() ) {
		osg::ref_ptr<Data::Node> existingNode = ( * ni );
		++ni;
		i++;
	}
	if (
		i!=2
	) {
		AppCore::Core::getInstance()->messageWindows->showMessageBox( "Upozornenie","Musite vybrat prave 2 vrcholy",false );
		return false;
	}
	ni = selectedNodes->constBegin();
	node2=( * ni );
	++ni;
	node1=( * ni );
	++ni;
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >* mapa = currentGraph->getEdges();
	Data::Type* type = currentGraph->addType( Data::GraphLayout::META_EDGE_TYPE );
	for ( QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator it = mapa->begin(); it != mapa->end(); ++it ) {
		osg::ref_ptr<Data::Edge> existingEdge = it.value();
		if (
			existingEdge->getSrcNode() ->getId() == node1 ->getId() &&
			existingEdge->getDstNode() ->getId() == node2 ->getId()
		) {
			AppCore::Core::getInstance()->messageWindows->showMessageBox( "Hrana najdena","Medzi vrcholmi nesmie byt hrana",false );
			return false;
		}
		if (
			existingEdge->getSrcNode() ->getId() == node2 ->getId() &&
			existingEdge->getDstNode() ->getId() == node1 ->getId()
		) {
			AppCore::Core::getInstance()->messageWindows->showMessageBox( "Hrana najdena","Medzi vrcholmi nesmie byt hrana",false );
			return false;
		}
	}

	osg::ref_ptr<Data::Edge> newEdge;
	if ( !client->isConnected() ) {
		newEdge = currentGraph->addEdge( "GUI_edge", node1, node2, type, false );
		Network::Server* server = Network::Server::getInstance();
		server->sendNewEdge( newEdge );
	}
	else {
		client->sendNewEdge( "GUI_edge", node1->getId(), node2->getId(), false );
	}
	if ( isPlaying ) {
		layout->play();
	}

	//QString nodename1 = QString(node1->getName());
	//QString nodename2 = QString(node2->getName());
	return true;
	//context.getGraph ().addEdge (QString (""), node1[1], node1[2], edgeType, true);

}

bool CoreWindow::add_NodeClick()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	Data::Type* edgeType = NULL;
	Data::Type* nodeType = NULL;

	if ( currentGraph == NULL ) {
		currentGraph= Manager::GraphManager::getInstance()->createNewGraph( "NewGraph" );
	}
	Importer::GraphOperations* operations = new Importer::GraphOperations( *currentGraph );
	operations->addDefaultTypes( edgeType, nodeType );

	osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter( true );

	osg::ref_ptr<Data::Node> newNode;
	if ( !client->isConnected() ) {
		newNode = currentGraph->addNode( "newNode", nodeType , position );
		Network::Server* server = Network::Server::getInstance();
		server->sendNewNode( newNode );
	}
	else {
		client->sendNewNode( "newNode", position );
	}

	if ( isPlaying ) {
		layout->play();
	}

	return true;
}

bool CoreWindow::removeClick()
{
	Network::Server* server = Network::Server::getInstance();

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QLinkedList<osg::ref_ptr<Data::Edge> >* selectedEdges = viewerWidget->getPickHandler()->getSelectedEdges();

	while ( selectedEdges->size() > 0 ) {
		osg::ref_ptr<Data::Edge> existingEdge1 = ( * ( selectedEdges->constBegin() ) );
		if ( !client->isConnected() ) {
			currentGraph->removeEdge( existingEdge1 );
			server->sendRemoveEdge( existingEdge1->getId() );
		}
		else {
			client->sendRemoveEdge( existingEdge1->getId() );
		}
		selectedEdges->removeFirst();
	}
	currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QLinkedList<osg::ref_ptr<Data::Node> >* selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();

	while ( selectedNodes->size() > 0 ) {
		osg::ref_ptr<Data::Node> existingNode1 = ( * ( selectedNodes->constBegin() ) );
		if ( existingNode1->isRemovableByUser() ) {
			if ( !client->isConnected() ) {
				currentGraph->removeNode( existingNode1 );
				server->sendRemoveNode( existingNode1->getId() );
			}
			else {
				client->sendRemoveNode( existingNode1->getId() );
			}
		}
		selectedNodes->removeFirst();
	}

	int NodesCount=currentGraph->getNodes()->size();
	std::cout<<NodesCount;
	if ( isPlaying ) {
		layout->play();
	}

	return true;
}

void CoreWindow::start_server()
{
	Network::Server* server = Network::Server::getInstance();

	if ( !server -> isListening() ) {

		server->setLayoutThread( layout );
		server->setCoreGraph( coreGraph );
		bool success = server->listen( QHostAddress::Any, 4200 );
		if ( !success ) {
			qDebug() << "Could not listen on port 4200.";
		}
		else {
			le_client_name->setEnabled( false );
			le_server_addr->setEnabled( false );
			b_start_client->setEnabled( false );
			b_start_server->setText( "End session" );
		}

		qDebug() << "Server started";
	}
	else {
		server->stopServer();
		le_client_name->setEnabled( true );
		le_server_addr->setEnabled( true );
		b_start_client->setEnabled( true );
		b_start_server->setText( "Host session" );
		qDebug() << "Server stopped";
	}
}

void CoreWindow::start_client()
{
	if ( !client -> isConnected() ) {
		client -> setLayoutThread( layout );
		client -> setCoreGraph( coreGraph );
		b_start_client -> setText( "Connecting..." );
		b_start_client -> setEnabled( false );
		b_start_server -> setEnabled( false );
		client -> ServerConnect( le_client_name->text(), le_server_addr->text() );
	}
	else {
		client -> disconnect();
	}
}

void CoreWindow::clusteringAlgorithmChanged( int index )
{
	switch ( index ) {
		case 0:
			Clustering::Clusterer::getInstance().setAlgorithmType( Clustering::Clusterer::AlgorithmType::ADJACENCY );
			break;
		case 1:
			Clustering::Clusterer::getInstance().setAlgorithmType( Clustering::Clusterer::AlgorithmType::LEAF );
			break;
		case 2:
			Clustering::Clusterer::getInstance().setAlgorithmType( Clustering::Clusterer::AlgorithmType::NEIGHBOUR );
			break;
		default:
			qDebug() << "CoreWindow:clusteringAlgorithmChanged : not suported index " << index;
			break;
	}
}

void CoreWindow::clusteringDepthChanged( const QString& value )
{
	Clustering::Clusterer::getInstance().setClusteringDepth( value.toInt() );
}

void CoreWindow::clustersOpacityCheckboxValueChanged( bool checked )
{
	b_clustersOpacity_Slider->setEnabled( !checked );
	chb_clusterSelectedOpacity->setEnabled( !checked );
	coreGraph->setClustersOpacityAutomatic( checked );
}

void CoreWindow::clusterSelectedOpacityCheckboxValueChanged( bool checked )
{
	coreGraph->setClustersOpacitySelected( checked );
}

void CoreWindow::clustersOpacitySliderValueChanged( int value )
{
	coreGraph->setClustersOpacity( static_cast<double>( value ) / 10 );
}

void CoreWindow::clustersShapeBoundarySliderValueChanged( int value )
{
	coreGraph->setClustersShapeBoundary( value );
}

void CoreWindow::repulsiveForceInsideClusterValueChanged( double value )
{
	QLinkedList<osg::ref_ptr<Data::Cluster> > clusters = viewerWidget->getPickHandler()->getPickedClusters();
	QLinkedList<osg::ref_ptr<Data::Cluster> >::iterator i;
	for ( i = clusters.begin(); i != clusters.end(); ++i ) {
		osg::ref_ptr<Data::Cluster> cluster = *i;
		cluster->setRepulsiveForceInside( value );
	}
}

void CoreWindow::cluster_nodes()
{
	if ( isPlaying ) {
		playPause();
	}

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph == NULL ) {
		AppCore::Core::getInstance()->messageWindows->showMessageBox( "Upozornenie","Ziadny graf nie je nacitany. Nie je co zhlukovat.",false );
		return;
	}

	Clustering::Clusterer::getInstance().cluster( currentGraph, clusteringProgressBar );

	coreGraph->createClusterGroup( Clustering::Clusterer::getInstance().getClusters() );

	int maxNodes = Clustering::Clusterer::getInstance().getMaxCountOfNodesInClusters();
	qDebug() << "***** maxNodes = " << maxNodes;

	l_clusters1Max->setText( QString::number( maxNodes ) );

	b_clustersShapeBoundary_Slider->setValue( maxNodes/2 );
	b_clustersShapeBoundary_Slider->setMaximum( maxNodes );

	chb_clustersOpacity->setChecked( false );
	b_clustersOpacity_Slider->setValue( 5 );

	coreGraph->setCameraManipulator( getCameraManipulator() );

	// show
	setVisibleClusterSection( true );

	selectionTypeComboBox->setCurrentIndex( 3 ); // selectionType zmen na CLUSTER

	//AppCore::Core::getInstance(NULL)->cg->reload(currentGraph);

	if ( !isPlaying ) {
		playPause();
	}
}

void CoreWindow::restartLayouting()
{
	float scale = Util::ApplicationConfig::get()->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();

	QLinkedList<osg::ref_ptr<Data::Cluster> > clusters = viewerWidget->getPickHandler()->getPickedClusters();
	QLinkedList<osg::ref_ptr<Data::Cluster> >::iterator i;
	for ( i = clusters.begin(); i != clusters.end(); ++i ) {
		osg::ref_ptr<Data::Cluster> cluster = *i;
		Layout::ShapeGetter_Cube* shapeGetter = cluster->getShapeGetter();

		if ( shapeGetter != NULL ) {
			QSet<Data::Node*> allClusteredNodes = cluster->getALLClusteredNodes();
			for ( QSet<Data::Node*>::const_iterator n = allClusteredNodes.constBegin(); n != allClusteredNodes.constEnd(); ++n ) {
				Data::Node* node = ( *n );
				node->setCurrentPosition( shapeGetter->getCenterNode()->getTargetPosition() * scale );
			}
		}
	}

	coreGraph->setNodesFreezed( true );
	coreGraph->setNodesFreezed( false );
}

// TODO - toto by sa mohlo robit uz pri oznaceni zhluku a nie explicitne cez button
void CoreWindow::setRestriction_Cube_Selected()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QLinkedList<osg::ref_ptr<Data::Cluster> > clusters = viewerWidget->getPickHandler()->getPickedClusters();
	if ( currentGraph != NULL && !clusters.empty() ) {
		//todo iter
		osg::ref_ptr<Data::Cluster> cluster = clusters.first();
		if ( cluster == NULL ) {
			return;
		}

		osg::ref_ptr<Data::Node> centerNode;
		osg::ref_ptr<Data::Node> surfaceNodeX;
		osg::ref_ptr<Data::Node> surfaceNodeY;
		osg::ref_ptr<Data::Node> surfaceNodeZ;

		float scale = Util::ApplicationConfig::get()->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();

		QString name_centerNode = "center";
		QString name_sufraceNodeX = "surfaceX";
		QString name_sufraceNodeY = "surfaceY";
		QString name_sufraceNodeZ = "surfaceZ";
		osg::Vec3 positionNode1 = cluster->getCube()->getMidpoint() / scale;
		osg::Vec3 positionNode2 = positionNode1 + osg::Vec3f( static_cast<float>( cluster->getCube()->getRadius() ) / scale, 0, 0 );
		osg::Vec3 positionNode3 = positionNode1 + osg::Vec3f( 0, static_cast<float>( cluster->getCube()->getRadius() ) / scale, 0 );
		osg::Vec3 positionNode4 = positionNode1 + osg::Vec3f( 0, 0, static_cast<float>( cluster->getCube()->getRadius() ) / scale );

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;

		centerNode = currentGraph->addRestrictionNode( name_centerNode, positionNode1 );
		surfaceNodeX = currentGraph->addRestrictionNode( name_sufraceNodeX, positionNode2 );
		surfaceNodeY = currentGraph->addRestrictionNode( name_sufraceNodeY, positionNode3 );
		surfaceNodeZ = currentGraph->addRestrictionNode( name_sufraceNodeZ, positionNode4 );

		restrictionNodes.push_back( centerNode );
		restrictionNodes.push_back( surfaceNodeX );
		restrictionNodes.push_back( surfaceNodeY );
		restrictionNodes.push_back( surfaceNodeZ );

		Layout::ShapeGetter_Cube* cube = new Layout::ShapeGetter_Cube( centerNode, surfaceNodeX, surfaceNodeY, surfaceNodeZ );

		// schovaj kocku obmedzovaca a aj nody ktorymi je reprezentovany - pretoze samotny tvar clusteru predstavuje obmedzovac
		centerNode->setInvisible( true );
		surfaceNodeX->setInvisible( true );
		surfaceNodeY->setInvisible( true );
		surfaceNodeZ->setInvisible( true );
		cube->setInvisible( true );

		QSharedPointer<Layout::ShapeGetter> shapeGetter = QSharedPointer<Layout::ShapeGetter> ( cube );
		QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler = QSharedPointer<Layout::RestrictionRemovalHandler_RestrictionNodesRemover> (
					new Layout::RestrictionRemovalHandler_RestrictionNodesRemover(
						*currentGraph,
						restrictionNodes
					)
				);

		cluster->registerShapeGetter( cube );

		QSet<Data::Node*> nodes = cluster->getALLClusteredNodes();

		currentGraph->getRestrictionsManager().setRestrictions( nodes, shapeGetter );

		if ( ( ! shapeGetter.isNull() ) && ( ! removalHandler.isNull() ) ) {
			currentGraph->getRestrictionsManager().setOrRunRestrictionRemovalHandler( shapeGetter, removalHandler );
		}

		if ( isPlaying ) {
			layout->play();
		}
	}
}

void CoreWindow::send_message()
{
	client->send_message( le_message->text() );
}

void CoreWindow::create_facewindow()
{
#ifdef OPENCV_FOUND
	OpenCV::OpenCVCore::getInstance( NULL, this )->faceRecognition();
#endif
}

#ifdef OPENCV_FOUND
#ifdef OPENNI2_FOUND
void CoreWindow::createKinectWindow()
{

	Kinect::KinectCore::getInstance( NULL,this )->kinectRecognition();
}

void CoreWindow::calculateRansac()
{
	Kinect::Ransac* ransac= new Kinect::Ransac();
	ransac->calculate();
}
#endif
#endif

#ifdef SPEECHSDK_FOUND
void CoreWindow::startSpeech()
{
	if ( this->mSpeechThr!=NULL && ( b_start_speech->text()=="Stop Speech" ) ) {
		this->mSpeechThr->cancel=true;
		if ( !this->mSpeechThr->wait( 5000 ) ) {
			this->mSpeechThr->terminate();
			this->mSpeechThr->wait();
		}
		delete( this->mSpeechThr );
		b_start_speech->setText( "Start Speech" );
		this->mSpeechThr=NULL;
		return;
	}
	this->mSpeechThr = new Speech::KinectSpeechThread();
	CoUninitialize();
	if ( this->mSpeechThr->initializeSpeech()==1 ) {
		delete( this->mSpeechThr );
		this->mSpeechThr=NULL;
		return;
	}
	this->mSpeechThr->start();
	b_start_speech->setText( "Stop Speech" );
}
#endif



void CoreWindow::toggleSpyWatch()
{
	if ( lw_users->count() == 0 || lw_users->currentItem() == NULL ) {
		QMessageBox msgBox;
		QString message = lw_users->count() == 0 ? "No client connected" : "No client selected";
		msgBox.setText( message );
		msgBox.setIcon( QMessageBox::Information );
		msgBox.setStandardButtons( QMessageBox::Ok );
		msgBox.setDefaultButton( QMessageBox::Ok );
		msgBox.exec();
		chb_spy->setChecked( false );
		chb_center->setChecked( false );
		return;
	}

	Network::Server* server = Network::Server::getInstance();
	bool is_server = server->isListening();

	QCheckBox* sender_chb = reinterpret_cast<QCheckBox*>( sender() );

	int id_user = lw_users->currentItem()->data( 6 ).toInt();

	// ak bolo kliknute na "spy"
	if ( sender_chb == chb_spy ) {

		// ak je "spy" zakliknute
		if ( chb_spy->isChecked() ) {

			// ak je centrovanie aktivne, deaktivujem
			if ( client->isCenteringUser() || server->isCenteringUser() ) {
				if ( is_server ) {
					server->unCenterUser();
				}
				else {
					client->unCenterUser();
				}
			}

			// aktivujem spehovanie
			if ( is_server ) {
				server->spyUser( id_user );
			}
			else {
				client->spyUser( id_user );
			}
			chb_center->setChecked( false );
		}
		// ak je "spy" odkliknute
		else {

			// ak je spehovanie aktivne, deaktivujem
			if ( client->isSpying() || server->isSpying() ) {
				if ( is_server ) {
					server->unSpyUser();
				}
				else {
					client->unSpyUser();
				}
			}
		}
	}

	// ak bolo kliknute "center"
	if ( sender_chb == chb_center ) {

		// ak je "center" zakliknute
		if ( chb_center->isChecked() ) {

			// ak je spehovanie aktivne, deaktivujem
			if ( client->isSpying() || server->isSpying() ) {
				if ( is_server ) {
					server->unSpyUser();
				}
				else {
					client->unSpyUser();
				}
			}

			// aktivujem centrovanie
			if ( is_server ) {
				server->centerUser( id_user );
			}
			else {
				client->centerUser( id_user );
			}
			chb_spy->setChecked( false );
		}

		// ak je "center" odkliknute
		else {

			// ak je centrovanie aktivne, deaktivujem
			if ( client->isCenteringUser() || server->isCenteringUser() ) {
				if ( is_server ) {
					server->unCenterUser();
				}
				else {
					client->unCenterUser();
				}
			}
		}
	}
}

void CoreWindow::toggleAttention()
{
	if ( chb_attention->isChecked() ) {
		Network::Server* server = Network::Server::getInstance();
		if ( server->isListening() ) {
			server->sendAttractAttention( true );
		}
		else {
			client->sendAttractAttention( true );
		}
	}
	else {
		Network::Server* server = Network::Server::getInstance();
		if ( server->isListening() ) {
			server->sendAttractAttention( false );
		}
		else {
			client->sendAttractAttention( false );
		}
	}
}

void CoreWindow::setAvatarScale( int scale )
{
	client->setAvatarScale( scale );
	Network::Server::getInstance()->setAvatarScale( scale );
}

// Duransky start - Akcia pri prepnuti checkboxu "Vertigo zoom"
void CoreWindow::toggleVertigo()
{
	// ak je "Vertigo zoom" zakliknute
	if ( chb_vertigo->isChecked() ) {
		// nastavi kameru do vertigo modu
		viewerWidget->getCameraManipulator()->setVertigoMode( true );
	}
	else {
		// ak je "Vertigo zoom" odkliknute
		viewerWidget->getCameraManipulator()->setVertigoMode( false );
		// resetuje projekcnu maticu
		viewerWidget->getCameraManipulator()->resetProjectionMatrixToDefault();
	}
}
// Duransky end - Akcia pri prepnuti checkboxu "Vertigo zoom"

// Duransky start - Funkcia na vytvorenie daneho poctu vertigo rovin
void CoreWindow::create_Vertigo_Planes( int numberOfPlanes, int nOfDepthsInOnePlane, Data::GraphSpanningTree* spanningTree, int maxDepth, QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes )
{

	QLinkedList<osg::ref_ptr<Data::Node> > pickedNodes;

	// vymazanie starych uzlov rovin - nie je spravne - namiesto odstranenia uzlov sa uzly zneviditelnia
	QLinkedList<Layout::ShapeGetter_Plane_ByThreeNodes*>::const_iterator it = planes_Vertigo.constBegin();
	for ( it; it != planes_Vertigo.constEnd(); ++it ) {

		QSet<Data::Node*> nodesOfPlane = ( *it )->getNodesOfShape();
		QSetIterator<Data::Node*> i( nodesOfPlane );
		while ( i.hasNext() ) {
			Data::Node* node = i.next();
			node->setInvisible( true );
			// - vyriesit odstranovanie uzlov spravne
		}
	}

	planes_Vertigo.clear();

	int nOfDepths = 0;
	int nOfPlane = 1;

	// rozdelenie uzlov do rovin podla hlbky
	for ( int depth = 0; depth <= maxDepth; depth++ ) {

		++nOfDepths;

		QList<qlonglong> groups = spanningTree->getGroupsInDepth( depth );

		QList<qlonglong>::iterator groupIt;
		for ( groupIt=groups.begin(); groupIt!=groups.end(); ++groupIt ) {

			// vyber vsetkych skupin a uzlov v skupine danej hlbky
			QList<qlonglong> nodes = spanningTree->getNodesInGroup( *groupIt );
			QList<qlonglong>::iterator nodeIt;

			for ( nodeIt=nodes.begin(); nodeIt!=nodes.end(); ++nodeIt ) {
				// nastavi uzlu cislo vertigo roviny, na ktorej sa nachadza
				allNodes->value( *nodeIt )->setNumberOfVertigoPlane( nOfPlane );
				pickedNodes.append( allNodes->value( *nodeIt ) );
			}
		}

		// obmedzenie uzlov podla poctu hlbok, ktore sa maju obmedzit na jednu rovinu
		if ( ( depth != ( nOfDepthsInOnePlane * numberOfPlanes - 1 ) ) && nOfDepths == nOfDepthsInOnePlane ) {

			Layout::ShapeGetter_Plane_ByThreeNodes* plane = setRestriction_Plane_Vertigo( &pickedNodes,nOfPlane );
			planes_Vertigo.append( plane );

			nOfDepths = 0;
			++nOfPlane;
			pickedNodes.clear();
		}
	}

	//zbytok uzlov sa obmedzi na poslednu rovinu
	Layout::ShapeGetter_Plane_ByThreeNodes* plane = setRestriction_Plane_Vertigo( &pickedNodes,nOfPlane );
	planes_Vertigo.append( plane );

}
// Duransky end - Funkcia na vytvorenie daneho poctu vertigo rovin

// Duransky start - Funkcia na pridanie dvoch vertigo rovin
void CoreWindow::add_PlanesClick()
{
	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	// ak nebol nacitany graf, tak sa nic nestane
	if ( currentGraph == NULL ) {
		return;
	}

	// vsetky uzly grafu
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();

	// uzly grafu vlozene do linked listu pre jednoduche iterovanie
	QLinkedList<osg::ref_ptr<Data::Node> > nodesList;
	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = allNodes->constBegin();
	while ( i != allNodes->constEnd() ) {
		nodesList.append( i.value() );
		++i;
	}

	// vyber root node pre kostru grafu
	int maxEdges=0;
	osg::ref_ptr<Data::Node> rootNode;
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator itNode = nodesList.constBegin();
	for ( itNode; itNode != nodesList.constEnd(); ++itNode ) {
		int actEdges = itNode->get()->getEdges()->size();
		if ( actEdges>maxEdges ) {
			rootNode= itNode->get();
			maxEdges=actEdges;
		}
	}

	// vytvorenie kostry stromu z root node
	Data::GraphSpanningTree* spanningTree = currentGraph->getSpanningTree( rootNode->getId() );

	// maximalna hlbka kostry grafu
	int maxDepth = spanningTree->getMaxDepth();

	// pocet hlbok na jednu rovinu
	int nOfDepthsInOnePlane = ( maxDepth + 1 ) / ( numberOfPlanes + 2 );

	// ak nevieme rozdelit jemnejsie rozdelit kostru grafu, tak nic neurobime
	if ( nOfDepthsInOnePlane == 0 ) {
		return;
	}

	// pridame dve roviny
	numberOfPlanes += 2;

	// vytvorime roviny s ich zmenenym poctom
	create_Vertigo_Planes( numberOfPlanes, nOfDepthsInOnePlane, spanningTree, maxDepth, allNodes );

}
// Duransky end - Funkcia na pridanie dvoch vertigo rovin

// Duransky start - Funkcia na odobranie dvoch vertigo rovin
void CoreWindow::remove_PlanesClick()
{

	if ( numberOfPlanes == 0 ) {
		return;
	}

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	// ak nebol nacitany graf, tak sa nic nestane
	if ( currentGraph == NULL ) {
		return;
	}

	// odstranenie obmedzeni vsetkych uzlov
	if ( numberOfPlanes == 2 ) {

		// vsetky uzly grafu
		QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();

		// reset hodnot urcujucich ktorej rovine patri uzol
		QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = allNodes->constBegin();
		while ( i != allNodes->constEnd() ) {
			i.value()->setNumberOfVertigoPlane( i.value()->getId() );
			++i;
		}

		//vymazanie starych uzlov rovin - setInvisible
		QLinkedList<Layout::ShapeGetter_Plane_ByThreeNodes*>::const_iterator it = planes_Vertigo.constBegin();
		for ( it; it != planes_Vertigo.constEnd(); ++it ) {

			QSet<Data::Node*> nodesOfPlane = ( *it )->getNodesOfShape();
			QSetIterator<Data::Node*> i( nodesOfPlane );
			while ( i.hasNext() ) {
				Data::Node* node = i.next();
				node->setInvisible( true );
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
	if ( currentGraph == NULL ) {
		return;
	}

	// vsetky uzly grafu
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = currentGraph->getNodes();

	// uzly grafu vlozene do linked listu pre jednoduche iterovanie
	QLinkedList<osg::ref_ptr<Data::Node> > nodesList;
	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = allNodes->constBegin();
	while ( i != allNodes->constEnd() ) {
		nodesList.append( i.value() );
		++i;
	}

	// vyber root node pre kostru grafu
	int maxEdges=0;
	osg::ref_ptr<Data::Node> rootNode;
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator itNode = nodesList.constBegin();
	for ( itNode; itNode != nodesList.constEnd(); ++itNode ) {
		int actEdges = itNode->get()->getEdges()->size();
		if ( actEdges>maxEdges ) {
			rootNode= itNode->get();
			maxEdges=actEdges;
		}
	}

	// vytvorenie kostry stromu z root node
	Data::GraphSpanningTree* spanningTree = currentGraph->getSpanningTree( rootNode->getId() );
	//QLinkedList<osg::ref_ptr<Data::Node> > pickedNodes;

	// maximalna hlbka kostry grafu
	int maxDepth = spanningTree->getMaxDepth();

	// pocet hlbok na jednu rovinu
	int nOfDepthsInOnePlane = ( maxDepth + 1 ) / ( numberOfPlanes - 2 );

	// odoberieme dve roviny
	numberOfPlanes -= 2;

	// vytvorime roviny s ich zmenenym poctom
	create_Vertigo_Planes( numberOfPlanes, nOfDepthsInOnePlane, spanningTree, maxDepth, allNodes );

}
// Duransky end - Funkcia na odobranie dvoch vertigo rovin

// Duransky start - Funkcie na zmenu vzajomnej vzdialenosti vertigo rovin
void CoreWindow::change_Vertigo_Planes_Distance( int value )
{

	int nOfPlane = 0;
	QLinkedList<Layout::ShapeGetter_Plane_ByThreeNodes*>::const_iterator it = planes_Vertigo.constBegin();
	for ( it; it != planes_Vertigo.constEnd(); ++it , ++nOfPlane ) {

		// ziskanie troch uzlov, ktore urcuju rovinu - obmedzenie
		QSet<Data::Node*> nodesOfPlane = ( *it )->getNodesOfShape();
		QSetIterator<Data::Node*> i( nodesOfPlane );

		// ohranicenie minimalnej a maximalnej vzdialenosti dvoch rovin
		if ( ( value > 0 && nOfPlane == 1 && i.peekNext()->getTargetPosition().x() >= vertigoPlanesMaxDistance ) ||
				( value < 0 && nOfPlane == 1 && i.peekNext()->getTargetPosition().x() <= vertigoPlanesMinDistance ) ) {
			return;
		}

		// pridanie zmeny vzdialenosti ku kazdemu uzlu, ktory urcuje rovinu - obmedzenie
		while ( i.hasNext() ) {
			Data::Node* node = i.next();
			osg::Vec3f oldPosition = node->getTargetPosition();
			node->setTargetPosition( oldPosition + osg::Vec3f( static_cast<float>( value * nOfPlane ), 0.f, 0.f ) );
		}
	}
	//zmena vzdialenosti medzi rovinami globalne
	vertigoPlanesDistance += value;
}

void CoreWindow::add_DistanceClick()
{
	// zvysenie vzdialenosti medzi rovinami
	change_Vertigo_Planes_Distance( deltaVertigoDistance );
}

void CoreWindow::subtract_DistanceClick()
{
	// znizenie vzdialenosti medzi rovinami
	change_Vertigo_Planes_Distance( -deltaVertigoDistance );
}
// Duransky end - Funkcie na zmenu vzajomnej vzdialenosti vertigo rovin

// Duransky start - Funkcia na zmenu hodnoty nasobica odpudivych sil dvoch uzlov nachadzajucich sa na rovnakej rovine
void CoreWindow::repulsive_Forces_ValueChanged()
{
	layout->getAlg()->setRepulsiveForceVertigo( change_Forces->value() );
}
// Duransky end - Funkcia na zmenu hodnoty nasobica odpudivych sil dvoch uzlov nachadzajucich sa na rovnakej rovine

Vwr::CameraManipulator* CoreWindow::getCameraManipulator()
{
	return viewerWidget->getCameraManipulator();
}

QOSG::ViewerQT* CoreWindow::GetViewerQt()
{
	return viewerWidget;
}

void CoreWindow::closeEvent( QCloseEvent* event )
{

#ifdef OPENCV_FOUND
	delete OpenCV::OpenCVCore::getInstance( NULL, this );
#endif
	//QApplication::closeAllWindows();   // ????
}

void QOSG::CoreWindow::moveMouseAruco( double positionX,double positionY,bool isClick, Qt::MouseButton button )
{
	this->viewerWidget->moveMouseAruco( positionX,positionY,isClick,this->x(),this->y(),button );
}

void CoreWindow::setRepulsiveForceInsideCluster( double repulsiveForceInsideCluster )
{
	sb_repulsiveForceInsideCluster->setValue( repulsiveForceInsideCluster );
	l_repulsiveForceInsideCluster->show();
	sb_repulsiveForceInsideCluster->show();
	b_SetRestriction_Cube_Selected->show();
	b_restartLayouting->show();
}

void CoreWindow::hideRepulsiveForceSpinBox()
{
	l_repulsiveForceInsideCluster->hide();
	sb_repulsiveForceInsideCluster->hide();
	b_SetRestriction_Cube_Selected->hide();
	b_restartLayouting->hide();
}

#ifdef FGLOVE_FOUND
void CoreWindow::startGlovesRecognition()
{

	// terminating fgloveThread
	if ( this->mGloveThr!=NULL && ( b_start_gloves->text()=="Stop Gloves" ) ) {
		this->mGloveThr->terminate();
		delete( this->mGloveThr );

		b_start_gloves->setText( "Start Gloves" );
		this->mGloveThr=NULL;
		return;
	}

	// starting fgloveThread
	this->mGloveThr = new Fglove::FgloveThread();
	this->mGloveThr->start();
	b_start_gloves->setText( "Stop Gloves" );

}

#endif

void CoreWindow::createMetricsToolBar()
{
	toolBar = new QToolBar( "Metrics visualizations",this );

	// <Change> Gloger start: added horizontal frame to support browser (webView) grouping toggling
	QFrame* frame = createHorizontalFrame();
	frame->layout()->addWidget( loadFunctionCallButton );
	frame->layout()->addWidget( browsersGroupingButton );
	toolBar->addWidget( frame );
	// Gloger end

	toolBar->addWidget( luaGraphTreeView );
	toolBar->setMinimumWidth( 350 );

	addToolBar( Qt::RightToolBarArea,toolBar );
	toolBar->setMovable( true );

	toolBar = new QToolBar( "Metrics filter",this );
#if QT_VERSION >= 0x040700
	filterNodesEdit->setPlaceholderText( "nodes filter" );
#endif
	toolBar->addWidget( filterNodesEdit );
#if QT_VERSION >= 0x040700
	filterEdgesEdit->setPlaceholderText( "edges filter" );
#endif
	toolBar->addWidget( filterEdgesEdit );
	addToolBar( Qt::BottomToolBarArea, toolBar );
	toolBar->setMovable( true );
}

void CoreWindow::loadFunctionCall()
{
	QString file = QFileDialog::getExistingDirectory( this, "Select lua project folder", "." );
	if ( file == "" ) {
		return;
	}
	std::cout << "You selected " << file.toStdString() << std::endl;
	Lua::LuaInterface* lua = Lua::LuaInterface::getInstance();

	Diluculum::LuaValueList path;
	path.push_back( file.toStdString() );
	QString createGraph[] = {"function_call_graph", "extractGraph"};
	lua->callFunction( 2, createGraph, path );
	lua->getLuaState()->doString( "getGraph = function_call_graph.getGraph" );
	Lua::LuaInterface::getInstance()->getLuaState()->doString( "getFullGraph = getGraph" );

	Data::Graph* currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if ( currentGraph != NULL ) {
		Manager::GraphManager::getInstance()->closeGraph( currentGraph );
	}
	currentGraph = Manager::GraphManager::getInstance()->createNewGraph( "LuaGraph" );

	layout->pause();
	coreGraph->setNodesFreezed( true );

	Lua::LuaGraphVisualizer* visualizer = new Lua::SimpleGraphVisualizer( currentGraph, coreGraph->getCamera() );
	visualizer->visualize();

	coreGraph->reloadConfig();
	if ( isPlaying ) {
		layout->play();
		coreGraph->setNodesFreezed( false );
	}
}

void CoreWindow::filterGraph()
{
	std::string nodesQueryText = filterNodesEdit->text().trimmed().toStdString();
	std::string edgesQueryText = filterEdgesEdit->text().trimmed().toStdString();

	Lua::LuaInterface* lua = Lua::LuaInterface::getInstance();

	Diluculum::LuaValueList query;
	query.push_back( nodesQueryText );
	QString validNodesQuery[] = {"logical_filter", "validNodeQuery"};
	if ( lua->callFunction( 2, validNodesQuery, query )[0] == false ) {
		AppCore::Core::getInstance()->messageWindows->showMessageBox( "Upozornenie","Neplatny vyraz filtra vrcholov",false );
		return;
	}
	query[0] = edgesQueryText;
	QString validEdgesQuery[] = {"logical_filter", "validEdgeQuery"};
	if ( lua->callFunction( 2, validEdgesQuery, query )[0] == false ) {
		AppCore::Core::getInstance()->messageWindows->showMessageBox( "Upozornenie","Neplatny vyraz filtra hran",false );
		return;
	}
	query[0] = nodesQueryText;
	query.push_back( edgesQueryText );
	lua->getLuaState()->doString( "getGraph = logical_filter.getGraph" );
	QString filterGraph[] = {"logical_filter", "filterGraph"};
	lua->callFunction( 2, filterGraph, query );
}

void CoreWindow::onChange()
{
//	TODO release models from memory in browser group
//	QAbstractItemModel *model = luaGraphTreeView->model();
//	if (model != NULL){
//		delete model;
//		model = NULL;
//	}

	// <Change> Gloger start: added support for multiple node selection using browser visualization
	QLinkedList<osg::ref_ptr<Data::Node> >* selected = viewerWidget->getPickHandler()->getSelectedNodes();

	coreGraph->getBrowsersGroup()->setSelectedNodes( selected );
	// qDebug() << "Selected nodes count: " << selected->size();

	if ( selected->size() > 0 ) {
		// Get last node model & display it in qt view
		qlonglong lastNodeId = selected->last()->getId();
		Lua::LuaGraphTreeModel* lastNodeModel = coreGraph->getBrowsersGroup()->getSelectedNodesModels()->value( lastNodeId );
		luaGraphTreeView->setModel( lastNodeModel );
	}

	// Gloger end
}

void CoreWindow::browsersGroupingClicked( bool checked )
{
	this->coreGraph->getBrowsersGroup()->setBrowsersGrouping( checked );
}

} // namespace QOSG
