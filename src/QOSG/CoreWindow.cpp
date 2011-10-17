#include "QOSG/CoreWindow.h"
#include "Util/Cleaner.h"

#include "Layout/ShapeGetter_SphereSurface_ByTwoNodes.h"
#include "Layout/ShapeGetter_Sphere_ByTwoNodes.h"
#include "Layout/ShapeGetter_Plane_ByThreeNodes.h"
#include "Layout/RestrictionRemovalHandler_RestrictionNodesRemover.h"

using namespace QOSG;

CoreWindow::CoreWindow(QWidget *parent, Vwr::CoreGraph* coreGraph, QApplication* app, Layout::LayoutThread * thread ) : QMainWindow(parent)
{		
	//inicializacia premennych
    isPlaying = true;
	application = app;
	layout = thread;

        client = NULL;
	
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

	load = new QAction(QIcon("img/gui/open.png"),"&Load graph from file", this);
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

	merge = new QPushButton();
	merge->setIcon(QIcon("img/gui/merge.png"));
	merge->setToolTip("&Merge nodes together");
	merge->setFocusPolicy(Qt::NoFocus);
	connect(merge, SIGNAL(clicked()), this, SLOT(mergeNodes()));

	separate = new QPushButton();
	separate->setIcon(QIcon("img/gui/separate.png"));
	separate->setToolTip("&Separate merged nodes");
	separate->setFocusPolicy(Qt::NoFocus);
	connect(separate, SIGNAL(clicked()), this, SLOT(separateNodes()));

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

	// layout restrictions
	b_SetRestriction_SphereSurface = new QPushButton();
	b_SetRestriction_SphereSurface->setIcon(QIcon("img/gui/restriction_sphere_surface.png"));
	b_SetRestriction_SphereSurface->setToolTip("&Set restriction - sphere surface");
	b_SetRestriction_SphereSurface->setFocusPolicy(Qt::NoFocus);
	connect(b_SetRestriction_SphereSurface, SIGNAL(clicked()), this, SLOT(setRestriction_SphereSurface ()));

	b_SetRestriction_Sphere = new QPushButton();
	b_SetRestriction_Sphere->setIcon(QIcon("img/gui/restriction_sphere.png"));
	b_SetRestriction_Sphere->setToolTip("&Set restriction - sphere");
	b_SetRestriction_Sphere->setFocusPolicy(Qt::NoFocus);
	connect(b_SetRestriction_Sphere, SIGNAL(clicked()), this, SLOT(setRestriction_Sphere ()));

	b_SetRestriction_Plane = new QPushButton();
	b_SetRestriction_Plane->setIcon(QIcon("img/gui/restriction_plane.png"));
	b_SetRestriction_Plane->setToolTip("&Set restriction - plane");
	b_SetRestriction_Plane->setFocusPolicy(Qt::NoFocus);
	connect(b_SetRestriction_Plane, SIGNAL(clicked()), this, SLOT(setRestriction_Plane ()));

	b_UnsetRestriction = new QPushButton();
	b_UnsetRestriction->setIcon(QIcon("img/gui/restriction_unset.png"));
	b_UnsetRestriction->setToolTip("&Unset restriction");
	b_UnsetRestriction->setFocusPolicy(Qt::NoFocus);
	connect(b_UnsetRestriction, SIGNAL(clicked()), this, SLOT(unsetRestriction ()));

        b_start_server = new QPushButton();
        b_start_server->setText("Start server");
        connect(b_start_server, SIGNAL(clicked()), this, SLOT(start_server()));

        b_start_client = new QPushButton();
        b_start_client->setText("Start client");
        connect(b_start_client, SIGNAL(clicked()), this, SLOT(start_client()));

        b_send_message = new QPushButton();
        b_send_message->setText("Send");
        connect(b_send_message, SIGNAL(clicked()), this, SLOT(send_message()));

        le_client_name = new QLineEdit("Nick");
        le_message= new QLineEdit("Message");
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
	toolBar->addSeparator();
	
	// layout restrictions
	frame = createHorizontalFrame();
	toolBar->addWidget(frame);
	frame->layout()->addWidget(b_SetRestriction_SphereSurface);
	frame->layout()->addWidget(b_SetRestriction_Sphere);

	frame = createHorizontalFrame();
	toolBar->addWidget(frame);
	frame->layout()->addWidget(b_SetRestriction_Plane);
	frame->layout()->addWidget(b_UnsetRestriction);

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

        toolBar->addSeparator();
        toolBar->addWidget(b_start_server);
        toolBar->addSeparator();
        toolBar->addWidget(le_client_name);
        toolBar->addWidget(b_start_client);
        toolBar->addSeparator();
        toolBar->addWidget(le_message);
        toolBar->addWidget(b_send_message);

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
	
		if(conn != NULL && conn->open()) { 
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
			qDebug() << "[QOSG::CoreWindow::saveLayoutToDB] Connection to DB not opened";
		}
	}
	else
	{
		qDebug() << "[QOSG::CoreWindow::saveLayoutToDB] There is no active graph loaded";
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

void CoreWindow::mergeNodes()
{	
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL)
	{
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true); 
		QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();

		if(selectedNodes->count() > 0) {
			currentGraph->mergeNodes(selectedNodes, position);
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
			currentGraph->separateNodes(selectedNodes);
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
		if ((*i)->getType()->isMeta() && (*i)->getName() != "mergedNode")
			currentGraph->removeNode((*i));
		++i;
	}

	if (isPlaying)
		layout->play();
}

void CoreWindow::loadFile()
{
	//treba overit
	layout->pause();
	coreGraph->setNodesFreezed(true);
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open file"), ".", tr("GraphML files (*.graphml);;GXL files (*.gxl);;RSF files (*.rsf)"));

	if (fileName != "") {
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
		//ak je edge skryta, nebudeme jej menit farbu
		if((*ei)->getScale() != 0)
		{
			(*ei)->setEdgeColor(osg::Vec4(red, green, blue, alpha));
		}
		++ei;
	}
}

void CoreWindow::setRestriction_SphereSurface ()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL)
	{
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

		osg::ref_ptr<Data::Node> centerNode = currentGraph->addRestrictionNode (QString ("center"), position);
		osg::ref_ptr<Data::Node> surfaceNode = currentGraph->addRestrictionNode (QString ("surface"), position + osg::Vec3f (10, 0, 0));

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;
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
	}
}

void CoreWindow::setRestriction_Sphere ()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL)
	{
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

		osg::ref_ptr<Data::Node> centerNode = currentGraph->addRestrictionNode (QString ("center"), position);
		osg::ref_ptr<Data::Node> surfaceNode = currentGraph->addRestrictionNode (QString ("surface"), position + osg::Vec3f (10, 0, 0));

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;
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
	}
}

void CoreWindow::setRestriction_Plane ()
{
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL) {
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true);

		osg::ref_ptr<Data::Node> node1 = currentGraph->addRestrictionNode (QString ("plane_node_1"), position);
		osg::ref_ptr<Data::Node> node2 = currentGraph->addRestrictionNode (QString ("plane_node_2"), position + osg::Vec3f (10, 0, 0));
		osg::ref_ptr<Data::Node> node3 = currentGraph->addRestrictionNode (QString ("plane_node_3"), position + osg::Vec3f (0, 10, 0));

		Layout::RestrictionRemovalHandler_RestrictionNodesRemover::NodesListType restrictionNodes;
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
			)
		);
	}
}

void CoreWindow::unsetRestriction () {
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	if (currentGraph != NULL) {
		setRestrictionToSelectedNodes (
			QSharedPointer<Layout::ShapeGetter> (NULL),
			currentGraph,
			QSharedPointer<Layout::RestrictionRemovalHandler> (NULL)
		);
	}
}

void CoreWindow::setRestrictionToSelectedNodes (
	QSharedPointer<Layout::ShapeGetter> shapeGetter,
	Data::Graph * currentGraph,
	QSharedPointer<Layout::RestrictionRemovalHandler> removalHandler
) {
	QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();

	QSet<Data::Node *> nodes;
	for (QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator it = selectedNodes->constBegin (); it != selectedNodes->constEnd (); ++it) {
		nodes.insert (it->get ());
	}

	currentGraph->getRestrictionsManager ().setRestrictions (nodes, shapeGetter);

	if ((! shapeGetter.isNull ()) && (! removalHandler.isNull ())) {
		currentGraph->getRestrictionsManager ().setOrRunRestrictionRemovalHandler (shapeGetter, removalHandler);
	}

	if (isPlaying)
		layout->play();
}

bool CoreWindow::add_EdgeClick()
{
	Data::Type *edgeType = NULL;
	Data::Type *nodeType = NULL;
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	
	
	QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();
	QLinkedList<osg::ref_ptr<Data::Node> >::const_iterator ni = selectedNodes->constBegin();
	
	if (
		selectedNodes==NULL
			) {
				AppCore::Core::getInstance()->messageWindows->showMessageBox("Upozornenie","Žiadny uzol oznaèený",false);
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
				AppCore::Core::getInstance()->messageWindows->showMessageBox("Upozornenie","Musite vybrat práve 2 vrcholy",false);
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
				existingEdge->getSrcNode () ->getName () == node1 ->getName () && 
				existingEdge->getDstNode () ->getName () == node2 ->getName ()
			) {
				AppCore::Core::getInstance()->messageWindows->showMessageBox("Hrana najdená","Medzi vrcholmi nesmie byt hrana",false);
				return false;
			}
			if (
				existingEdge->getSrcNode () ->getName () == node2 ->getName () && 
				existingEdge->getDstNode () ->getName () == node1 ->getName ()
			) {
				AppCore::Core::getInstance()->messageWindows->showMessageBox("Hrana najdená","Medzi vrcholmi nesmie byt hrana",false);
				return false;
			}
		}

	
	currentGraph->addEdge("GUI_edge", node1, node2, type, false);
	if (isPlaying)
			layout->play();
	QString nodename1 = QString(node1->getName());
	QString nodename2 = QString(node2->getName());
	return true;
	//context.getGraph ().addEdge (QString (""), node1[1], node1[2], edgeType, true);

}

bool CoreWindow::add_NodeClick()
{	
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	Data::Type *edgeType = NULL;
	Data::Type *nodeType = NULL;

	if (currentGraph != NULL)
	{
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true); 

		osg::ref_ptr<Data::Node> node1 = currentGraph->addNode("newNode", currentGraph->getNodeMetaType(), position);	

		if (isPlaying)
			layout->play();
	}
	else
	{
		currentGraph= Manager::GraphManager::getInstance()->createNewGraph("NewGraph");
		osg::Vec3 position = viewerWidget->getPickHandler()->getSelectionCenter(true); 
		Data::MetaType* type = currentGraph->addMetaType(Data::GraphLayout::META_NODE_TYPE);
		osg::ref_ptr<Data::Node> node1 = currentGraph->addNode("newNode", type);	
		if (isPlaying)
			layout->play();
	}
	return true;
}

bool CoreWindow::removeClick()
{	
	Data::Graph * currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QLinkedList<osg::ref_ptr<Data::Edge> > * selectedEdges = viewerWidget->getPickHandler()->getSelectedEdges();
	
	while (selectedEdges->size () > 0) {
		osg::ref_ptr<Data::Edge> existingEdge1 = (* (selectedEdges->constBegin()));
		currentGraph->removeEdge(existingEdge1);
		selectedEdges->removeFirst ();
	}
	currentGraph = Manager::GraphManager::getInstance()->getActiveGraph();
	QLinkedList<osg::ref_ptr<Data::Node> > * selectedNodes = viewerWidget->getPickHandler()->getSelectedNodes();

	while (selectedNodes->size () > 0) {
		osg::ref_ptr<Data::Node> existingNode1 = (* (selectedNodes->constBegin()));
		if (existingNode1->isRemovableByUser())
			currentGraph->removeNode(existingNode1);
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
    Network::Server *server = new Network::Server();
    server->setLayoutThread(layout);
    bool success = server->listen(QHostAddress::Any, 4200);
    if(!success)
    {
        qFatal("Could not listen on port 4200.");
    }

    qDebug() << "Server started";
}

void CoreWindow::start_client()
{
    if (client == NULL) {
        client = new Network::Client(this);
        client -> ServerConnect(le_client_name->text());
    } else {
        qDebug() << "Client already running";
    }
}


void CoreWindow::send_message()
{
    if (client != NULL) {
        client->send_message(le_message->text());
        //client->send_message("GET_GRAPH");
    }
}
