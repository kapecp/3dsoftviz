#include "SpecialMatrix/MatrixViewer.h"

#include "osg/LineWidth"
#include "Importer/GraphOperations.h"


#include <QDebug>

SpecialMatrix::MatrixViewer::MatrixViewer(QWidget* parent, Qt::WindowFlags f, Vwr::CoreGraph* coreGraph): QWidget(parent, f)
{
	osgViewer::CompositeViewer::setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);	
	simpleGraph = new Data::Graph( 1, "simple", 0, 0, NULL );

	QMap<QString, QString>* settings = new QMap<QString, QString>;

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.SquareNode" ) );
	Data::Type* type = simpleGraph->addType( "default1", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultEdgeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );
	Data::Type* type2 = simpleGraph->addType( "default2", settings );

	osg::ref_ptr<Data::Node> u1 = simpleGraph->addNode( 10, "u1", type, osg::Vec3f(0.0f, 0.0f, 0.0f) );
	osg::ref_ptr<Data::Node> u2 = simpleGraph->addNode( 20, "u2", type, osg::Vec3f(5.0f, 5.0f, 5.0f) );
	osg::ref_ptr<Data::Node> u3 = simpleGraph->addNode( 30, "u3", type, osg::Vec3f(10.0f, 10.0f, 10.0f) );

	simpleGraph->addEdge( "e1", u1, u2, type2, false );
	simpleGraph->addEdge( "e2", u1, u3, type2, false );
	simpleGraph->addEdge( "e3", u2, u3, type2, false );

	//coreGraph->reload(simpleGraph);


	qDebug() << "nodes: " << simpleGraph->getNodes()->count() << "endl";

	QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = simpleGraph->getNodes();

	qDebug() << "0: " << allNodes->key(u1) ;
	qDebug() << "1: " << allNodes->key(u2) ;
	qDebug() << "2: " << allNodes->key(u3) ;





	//Data::Type* edgeType;
	//Data::Type* nodeType;
	//Importer::GraphOperations* operations = new Importer::GraphOperations( *graph );
	//operations->addDefaultTypes( edgeType, nodeType );

	//osg::ref_ptr<Data::Node> node0 = graph->addNode(13, "center", nodeType, osg::Vec3f(0.0f, 0.0f, 0.0f));
	//osg::ref_ptr<Data::Node> node1 = graph->addNode(17, "E63", nodeType, osg::Vec3f(2.0f, 0.0f, 0.0f));
	//osg::ref_ptr<Data::Node> node2 = graph->addNode(22, "E63", nodeType, osg::Vec3f(10.0f, 10.0f, 10.0f));

	//graph->addEdge(2,"edge1",node0,node1,edgeType, false );
	//graph->addEdge(3,"edge2",node0,node2,edgeType, false );

	// disable the default setting of viewer.done() by pressing Escape.
	setKeyEventSetsDone(0);

	_gwQt = createGraphicsWindow(0, 0, 100, 100, "Embedded Windows", false);

	//QWidget* popupWidget = addViewWidget( _gwQt, createScene() );
	QWidget* popupWidget = addViewWidget( _gwQt, coreGraph->getScene() );
	QGridLayout* grid = new QGridLayout;
	grid->addWidget( popupWidget, 0, 0 );
	setLayout(grid);	
	connect( &_timer, SIGNAL(timeout()), this, SLOT(update()) );
	_timer.start( 10 );
}

QWidget* SpecialMatrix::MatrixViewer::addViewWidget( osgQt::GraphicsWindowQt* gwQt, osg::Group* scene )
{	
	osgViewer::View* view = new osgViewer::View();

	addView( view );

	osg::Camera* camera = view->getCamera();
	camera->setGraphicsContext( gwQt );

	const osg::GraphicsContext::Traits* traits = gwQt->getTraits();

	camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
	camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
	camera->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );

	view->setSceneData( scene );
	view->addEventHandler( new osgViewer::StatsHandler );
	view->setCameraManipulator( new osgGA::MultiTouchTrackballManipulator );
	gwQt->setTouchEventsEnabled( true );
	return gwQt->getGLWidget();
}

osgQt::GraphicsWindowQt* SpecialMatrix::MatrixViewer::createGraphicsWindow( int x, int y, int w, int h, const std::string& name, bool windowDecoration )
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowName = name;
	traits->windowDecoration = windowDecoration;
	traits->x = x;
	traits->y = y;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = ds->getNumMultiSamples();

	return new osgQt::GraphicsWindowQt(traits.get());
}

osg::Group* SpecialMatrix::MatrixViewer::createScene()
{
	osg::ref_ptr<osg::Group> scene = new osg::Group;
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(createAxis(
				osg::Vec3(0.0f, 0.0f, 0.0f),
				osg::Vec3(5.0f, 0.0f, 0.0f),
				osg::Vec3(0.0f, 5.0f, 0.0f),
				osg::Vec3(0.0f, 0.0f, 5.0f)));

	scene->addChild(geode.get());

	/*osg::ref_ptr<osg::Geometry> xAxisGeometry = new osg::Geometry;
	osg::ref_ptr<osg::Geometry> yAxisGeometry = new osg::Geometry;

	geode->addDrawable(xAxisGeometry);
	geode->addDrawable(yAxisGeometry);
	scene->addChild(geode.get());

	osg::Vec3Array* xAxisVertices = new osg::Vec3Array;
	xAxisVertices->push_back( osg::Vec3( 0, 0, 0) ); // zero
	xAxisVertices->push_back( osg::Vec3( 15, 0, 0) ); // x left

	xAxisGeometry->setVertexArray(xAxisVertices);

	osg::Vec3Array* yAxisVertices = new osg::Vec3Array;
	yAxisVertices->push_back( osg::Vec3( 0, 0, 0) ); // zero
	yAxisVertices->push_back( osg::Vec3( 0, 0, 15) ); // z up

	yAxisGeometry->setVertexArray(yAxisVertices);

	osg::DrawElementsUInt* axisXBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
	axisXBase->push_back(1);
	axisXBase->push_back(0);
	xAxisGeometry->addPrimitiveSet(axisXBase);

	osg::DrawElementsUInt* axisYBase = new osg::DrawElementsUInt(osg::PrimitiveSet::LINES, 0);
	axisYBase->push_back(1);
	axisYBase->push_back(0);
	yAxisGeometry->addPrimitiveSet(axisYBase);*/

	/*osg::ref_ptr<osg::TessellationHints> hints = new osg::TessellationHints;
	osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f, -2.0f, 0.0f), 10, 2.0f, 10), hints.get());
	shape->setColor(osg::Vec4(0.5f, 0.5f, 0.7f, 1.0f));
	geode->addDrawable(shape.get());*/

	return scene.release();
}

osg::Drawable* SpecialMatrix::MatrixViewer::createAxis(const osg::Vec3& corner,const osg::Vec3& xdir,const osg::Vec3& ydir,const osg::Vec3& zdir)
{
	osg::ref_ptr<osg::Geometry> axisGeometry = new osg::Geometry;

	osg::Vec3Array* axisVertices = new osg::Vec3Array;
	axisVertices->push_back(corner);
	axisVertices->push_back(corner + xdir);
	axisVertices->push_back(corner);
	axisVertices->push_back(corner + ydir);
	axisVertices->push_back(corner);
	axisVertices->push_back(corner + zdir);

	axisGeometry->setVertexArray(axisVertices);

	osg::Vec4 xColour(0.0f,1.0f,1.0f,1.0f);
	osg::Vec4 yColour(0.0f,1.0f,1.0f,1.0f);
	osg::Vec4 zColour(1.0f,0.0f,0.0f,1.0f);

	osg::Vec4Array* axisColours = new osg::Vec4Array;
	axisColours->push_back(xColour);
	axisColours->push_back(xColour);
	axisColours->push_back(yColour);
	axisColours->push_back(yColour);
	axisColours->push_back(zColour);
	axisColours->push_back(zColour);

	axisGeometry->setColorArray(axisColours, osg::Array::BIND_PER_VERTEX);
	axisGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 6));

	osg::LineWidth* lineWidth = new osg::LineWidth();
	lineWidth->setWidth(4.0f);

	osg::StateSet* stateset = new osg::StateSet;
	stateset->setAssociatedModes(lineWidth, osg::StateAttribute::ON);
	stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

	axisGeometry->setStateSet(stateset);

	return axisGeometry.release();
}
