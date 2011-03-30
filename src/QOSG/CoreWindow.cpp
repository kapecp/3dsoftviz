#include "QOSG/CoreWindow.h"
#include "Util/Cleaner.h"

#include "Layout/ShapeGetter_SphereSurface_ByTwoNodes.h"
#include "Layout/ShapeGetter_Sphere_ByTwoNodes.h"
#include "Layout/ShapeGetter_Plane_ByThreeNodes.h"

using namespace QOSG;

CoreWindow::CoreWindow(QWidget *parent, Vwr::CoreGraph* coreGraph, QApplication* app, Layout::LayoutThread * thread ) : QMainWindow(parent)
{		
	//inicializacia premennych
    isPlaying = true;
	application = app;
	layout = thread;
	
	//vytvorenie menu a toolbar-ov
	createActions();
	createMenus();
	createToolBar();		
	
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

	connect(lineEdit,SIGNAL(returnPressed()),this,SLOT(sqlQuery()));	
}
 
void CoreWindow::createActions()
{	
	quit = new QAction("Quit", this);
	connect(quit, SIGNAL(triggered()), application, SLOT(quit())); 	

	options = new QAction("Options", this);
	connect(options,SIGNAL(triggered()),this,SLOT(showOptions()));

	load = new QAction(QIcon("img/gui/load.png"),"&Load graph from file", this);
	connect(load, SIGNAL(triggered()), this, SLOT(loadFile()));

	loadGraph = new QAction(QIcon("img/gui/loadFromDB.png"),"&Load graph from database", this);
	connect(loadGraph, SIGNAL(triggered()), this, SLOT(showLoadGraph()));

	saveGraph = new QAction(QIcon("img/gui/saveToDB.png"),"&Save graph layout", this);
	connect(saveGraph, SIGNAL(triggered()), this, SLOT(saveLayoutToDB()));

	play = new QPushButton();
	play->setIcon(QIcon("img/gui/pause.png"));
	play->setToolTip("&Play");
	play->setFocusPolicy(Qt::NoFocus);
	connect(play, SIGNAL(clicked()), this, SLOT(playPause()));

	addMeta = new QPushButton();
	addMeta->setIcon(QIcon("img/gui/meta.png"));
	addMeta->setToolTip("&Add meta node");
	addMeta->setFocusPolicy(Qt::NoFocus);
	connect(addMeta, SIGNAL(clicked()), this, SLOT(addMetaNode()));

	removeMeta = new QPushButton();
	removeMeta->setIcon(QIcon("img/gui/removemeta.png"));
	removeMeta->setToolTip("&Remove meta nodes");
	removeMeta->setFocusPolicy(Qt::NoFocus);
	connect(removeMeta, SIGNAL(clicked()), this, SLOT(removeMetaNodes()));

	fix = new QPushButton();
	fix->setIcon(QIcon("img/gui/fix.png"));
	fix->setToolTip("&Fix nodes");
	fix->setFocusPolicy(Qt::NoFocus);
	connect(fix, SIGNAL(clicked()), this, SLOT(fixNodes()));

	unFix = new QPushButton();
	unFix->setIcon(QIcon("img/gui/unfix.png"));
	unFix->setToolTip("&Unfix nodes");
	unFix->setFocusPolicy(Qt::NoFocus);
	connect(unFix, SIGNAL(clicked()), this, SLOT(unFixNodes()));

	label = new QPushButton();
	label->setIcon(QIcon("img/gui/label.png"));
	label->setToolTip("&Turn on/off labels");
	label->setCheckable(true);
	label->setFocusPolicy(Qt::NoFocus);
	connect(label, SIGNAL(clicked(bool)), this, SLOT(labelOnOff(bool)));

	applyColor = new QPushButton();
	applyColor->setText("Apply color");
	applyColor->setToolTip("Apply selected color");
	applyColor->setFocusPolicy(Qt::NoFocus);
	connect(applyColor,SIGNAL(clicked()),this,SLOT(applyColorClick()));

	// layout restrictions
	b_AddRestriction_SphereSurface = new QPushButton();
	b_AddRestriction_SphereSurface->setIcon(QIcon("img/gui/restriction_sphere_surface.png"));
	b_AddRestriction_SphereSurface->setToolTip("&Add restriction - sphere surface");
	b_AddRestriction_SphereSurface->setFocusPolicy(Qt::NoFocus);
	connect(b_AddRestriction_SphereSurface, SIGNAL(clicked()), this, SLOT(addRestriction_SphereSurface ()));

	b_AddRestriction_Sphere = new QPushButton();
	b_AddRestriction_Sphere->setIcon(QIcon("img/gui/restriction_sphere.png"));
	b_AddRestriction_Sphere->setToolTip("&Add restriction - sphere");
	b_AddRestriction_Sphere->setFocusPolicy(Qt::NoFocus);
	connect(b_AddRestriction_Sphere, SIGNAL(clicked()), this, SLOT(addRestriction_Sphere ()));

	b_AddRestriction_Plane = new QPushButton();
	b_AddRestriction_Plane->setIcon(QIcon("img/gui/restriction_plane.png"));
	b_AddRestriction_Plane->setToolTip("&Add restriction - plane");
	b_AddRestriction_Plane->setFocusPolicy(Qt::NoFocus);
	connect(b_AddRestriction_Plane, SIGNAL(clicked()), this, SLOT(addRestriction_Plane ()));

	//mody - ziadny vyber, vyber jedneho, multi vyber centrovanie
	noSelect = new QPushButton();
	noSelect->setIcon(QIcon("img/gui/noselect.png"));
	noSelect->setToolTip("&No-select mode");
	noSelect->setCheckable(true);
	noSelect->setFocusPolicy(Qt::NoFocus);
	connect(noSelect, SIGNAL(clicked(bool)), this, SLOT(noSelectClicked(bool)));

	singleSelect = new QPushButton();
	singleSelect->setIcon(QIcon("img/gui/singleselect.png"));
	singleSelect->setToolTip("&Single-select mode");
	singleSelect->setCheckable(true);
	singleSelect->setFocusPolicy(Qt::NoFocus);
	connect(singleSelect, SIGNAL(clicked(bool)), this, SLOT(singleSelectClicked(bool)));

	multiSelect = new QPushButton();
	multiSelect->setIcon(QIcon("img/gui/multiselect.png"));
	multiSelect->setToolTip("&Multi-select mode");
	multiSelect->setCheckable(true);
	multiSelect->setFocusPolicy(Qt::NoFocus);
	connect(multiSelect, SIGNAL(clicked(bool)), this, SLOT(multiSelectClicked(bool)));

	center = new QPushButton();
	center->setIcon(QIcon("img/gui/center.png"));
	center->setToolTip("&Center view");
	center->setFocusPolicy(Qt::NoFocus);
	connect(center, SIGNAL(clicked(bool)), this, SLOT(centerView(bool)));
}

void CoreWindow::createMenus()
{
	file = menuBar()->addMenu("File");	
	file->addAction(load);
	file->addAction(loadGraph);
	file->addSeparator();
	file->addAction(saveGraph);
	file->addSeparator();
	file->addAction(quit);
	
	edit = menuBar()->addMenu("Edit");
	edit->addAction(options);	
}

void CoreWindow::createToolBar()
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

	toolBar->addWidget(label);
	toolBar->addSeparator();
	toolBar->addWidget(play);
	toolBar->addSeparator();
	toolBar->addWidget(applyColor);

	frame = createHorizontalFrame();

	toolBar->addWidget(frame);
	frame->layout()->addWidget(b_AddRestriction_SphereSurface);
	frame->layout()->addWidget(b_AddRestriction_Sphere);
	frame->layout()->addWidget(b_AddRestriction_Plane);

	//inicializacia colorpickera
	QtColorPicker * colorPicker = new QtColorPicker();
	colorPicker->setStandardColors();
	connect(colorPicker,SIGNAL(colorChanged(const QColor &)),this,SLOT(colorPickerChanged(const QColor &)));
	toolBar->addWidget(colorPicker);	
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
	toolBar->setMovable(false);
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

void CoreWindow::saveLayoutToDB()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if(currentGraph != NULL)
	{
		QSqlDatabase * conn = Manager::GraphManager::getInstance()->getDB()->tmpGetConn();
		bool ok;
	
		QString layout_name = QInputDialog::getText(this, tr("New layout name"), tr("Layout name:"), QLineEdit::Normal, "", &ok);
		
		if (ok && !layout_name.isEmpty())
		{
			Data::GraphLayout* layout = Model::GraphLayoutDAO::addLayout(layout_name, currentGraph, conn);
			currentGraph->selectLayout(layout);

			currentGraph->saveLayoutToDB(conn, currentGraph);
		}
		else
		{
			qDebug() << "[QOSG::CoreWindow::saveLayoutToDB] Input dialog canceled";
		}
	}
	else
	{
		qDebug() << "[QOSG::CoreWindow::saveLayoutToDB] There is no active graph loaded";
	}
}

/*void CoreWindow::saveGraphToDB()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if(currentGraph != NULL)
	{
		currentGraph->saveGraphToDB();
	}
	else
	{
		qDebug() << "[QOSG::CoreWindow::saveGraphToDB] There is no active graph loaded";
	}
}*/

void CoreWindow::sqlQuery()
{
	cout << lineEdit->text().toStdString() << endl;
}

void CoreWindow::playPause()
{
	if(isPlaying)
	{
		play->setIcon(QIcon("img/gui/play.png"));
		isPlaying = 0;
		layout->pause();
		coreGraph->setNodesFreezed(true);
	}
	else
	{
		play->setIcon(QIcon("img/gui/pause.png"));
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
}

void CoreWindow::singleSelectClicked(bool checked)
{
	viewerWidget->getPickHandler()->setPickMode(Vwr::PickHandler::PickMode::SINGLE);
	noSelect->setChecked(false);
	multiSelect->setChecked(false);
	center->setChecked(false);
}

void CoreWindow::multiSelectClicked(bool checked)
{
	viewerWidget->getPickHandler()->setPickMode(Vwr::PickHandler::PickMode::MULTI);
	noSelect->setChecked(false);
	singleSelect->setChecked(false);
	center->setChecked(false);
}

void CoreWindow::centerView(bool checked)
{
	noSelect->setChecked(false);
	singleSelect->setChecked(false);
	multiSelect->setChecked(false);	

	viewerWidget->getCameraManipulator()->setCenter(viewerWidget->getPickHandler()->getSelectionCenter(false));
}


void CoreWindow::addMetaNode()
{	
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL)
	{
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true); 

		osg::ref_ptr<Data::Node> metaNode = currentGraph->addNode("metaNode", currentGraph->getNodeMetaType(), position);	
		QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();

		QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selectedNodes->constBegin();

		while (i != selectedNodes->constEnd()) 
		{
			Data::Edge * e = currentGraph->addEdge("metaEdge", (*i), metaNode, currentGraph->getEdgeMetaType(), true);
			e->setCamera(viewerWidget->getCamera());
			++i;
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

void CoreWindow::removeMetaNodes()
{
	QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator i = selectedNodes->constBegin();

	while (i != selectedNodes->constEnd()) 
	{
		if ((*i)->getType()->isMeta())
			currentGraph->removeNode((*i));
		++i;
	}

	if (isPlaying)
		layout->play();
}

void CoreWindow::loadFile()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open file"), ".", tr("GraphML files (*.graphml);;GXL files (*.gxl);;RSF files (*.rsf)"));

	if (fileName != "") {
		Manager::GraphManager::getInstance()->loadGraph(fileName);

		viewerWidget->getCameraManipulator()->home();
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
	else if (viewerWidget->getPickHandler()->getSelectionType() == Vwr::PickHandler::SelectionType::ALL)
	{
		bool state = edgeLabelsVisible & nodeLabelsVisible;

		nodeLabelsVisible = edgeLabelsVisible = !state;

		coreGraph->setEdgeLabelsVisible(!state);
		coreGraph->setNodeLabelsVisible(!state);
	}
}

void CoreWindow::sliderValueChanged(int value)
{	
	layout->setAlphaValue((float)value * 0.001);
}


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
		}
}

void CoreWindow::applyColorClick()
{
	float alpha = color.alphaF();
	float red = color.redF();
	float green = color.greenF();
	float blue = color.blueF();

	QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator ni = selectedNodes->constBegin();

	while (ni != selectedNodes->constEnd()) 
	{
		(*ni)->setColor(osg::Vec4(red, green, blue, alpha));
		++ni;
	}

	QLinkedList<osg::ref_ptr<Data::Edge> > * selectedEdges = viewerWidget->getPickHandler()->getSelectedEdges();
	QLinkedList<osg::ref_ptr<Data::Edge> >::const_iterator ei = selectedEdges->constBegin();

	while (ei != selectedEdges->constEnd()) 
	{
		(*ei)->setEdgeColor(osg::Vec4(red, green, blue, alpha));
		++ei;
	}
}

void CoreWindow::addRestriction_SphereSurface ()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL)
	{
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

		osg::ref_ptr<Data::Node> centerNode = currentGraph->addRestrictionNode (QString ("center"), position);
		osg::ref_ptr<Data::Node> surfaceNode = currentGraph->addRestrictionNode (QString ("surface"), position + osg::Vec3f (10, 0, 0));

		QSharedPointer<Layout::ShapeGetter> shapeGetter (new Layout::ShapeGetter_SphereSurface_ByTwoNodes (centerNode, surfaceNode));

		setRestrictionToSelectedNodes (shapeGetter, currentGraph);
	}
}

void CoreWindow::addRestriction_Sphere ()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL)
	{
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

		osg::ref_ptr<Data::Node> centerNode = currentGraph->addRestrictionNode (QString ("center"), position);
		osg::ref_ptr<Data::Node> surfaceNode = currentGraph->addRestrictionNode (QString ("surface"), position + osg::Vec3f (10, 0, 0));

		QSharedPointer<Layout::ShapeGetter> shapeGetter (new Layout::ShapeGetter_Sphere_ByTwoNodes (centerNode, surfaceNode));

		setRestrictionToSelectedNodes (shapeGetter, currentGraph);
	}
}

void CoreWindow::addRestriction_Plane ()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL) {
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

		osg::ref_ptr<Data::Node> node1 = currentGraph->addRestrictionNode (QString ("plane_node_1"), position);
		osg::ref_ptr<Data::Node> node2 = currentGraph->addRestrictionNode (QString ("plane_node_2"), position + osg::Vec3f (10, 0, 0));
		osg::ref_ptr<Data::Node> node3 = currentGraph->addRestrictionNode (QString ("plane_node_3"), position + osg::Vec3f (0, 10, 0));

		QSharedPointer<Layout::ShapeGetter> shapeGetter (new Layout::ShapeGetter_Plane_ByThreeNodes (node1, node2, node3));

		setRestrictionToSelectedNodes (shapeGetter, currentGraph);
	}
}

void CoreWindow::setRestrictionToSelectedNodes (
	QSharedPointer<Layout::ShapeGetter> shapeGetter,
	Data::Graph * currentGraph
) {
	QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();

	QSet<Data::Node *> nodes;
	for (QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator it = selectedNodes->constBegin (); it != selectedNodes->constEnd (); ++it) {
		nodes.insert (it->get ());
	}

	currentGraph->getRestrictionsManager ().setRestrictions (nodes, shapeGetter);

	if (isPlaying)
		layout->play();
}
