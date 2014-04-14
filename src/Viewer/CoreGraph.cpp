#include "Viewer/CoreGraph.h"

#include "Viewer/SkyBox.h"
#include "Viewer/EdgeGroup.h"
#include "Viewer/NodeGroup.h"
#include "Viewer/PerlinNoiseTextureGenerator.h"
#include "Viewer/SkyTransform.h"
#include "Viewer/TextureWrapper.h"


#include "Network/Server.h"
#include "Data/Graph.h"
#include "Util/ApplicationConfig.h"
#include "OpenCV/CameraStream.h"

#include <osgUtil/Optimizer>
#include <osg/Depth>


using namespace Vwr;

/*
* TODO prerobit - v sucastnosti je scena jeden velky plochy graf. toto sa da optimalizovat do stromovej strukutry. pri vytvarani grafu ho treba prechadzat ako graf
* a nie vsetko zaradom ako je to teraz
*/


Vwr::CoreGraph::CoreGraph(Data::Graph * graph, osg::ref_ptr<osg::Camera> camera)
{
	this->graph = graph;
	this->camera = ((camera == 0) ? ((osg::ref_ptr<osg::Camera>) new osg::Camera) : (camera));

	this->in_nodes = NULL;
	this->in_edges = NULL;
	this->qmetaNodes = NULL;
	this->qmetaEdges = NULL;
	this->nodesFreezed = false;

	this->edgesGroup = NULL;
	this->qmetaEdgesGroup = NULL;

	appConf = Util::ApplicationConfig::get();

	root = new osg::Group();
	graphRotTransf = new osg::MatrixTransform();
	graphGroup = new osg::Group();


	graphRotTransf->addChild(graphGroup);
	root->addChild(graphRotTransf);


	// backgroung this must be last Node in root !!!  ( because of ortho2d background)
	root->addChild( createBackground());
	backgroundPosition = 1;

	reload(graph);
}

void CoreGraph::reload(Data::Graph * graph)
{
	cleanUp();

	int currentPos = 0;

	while ( graphGroup->getNumChildren() > 0)
	{
		graphGroup->removeChildren(0, 1);
	}

	this->graph = graph;

	if (graph != NULL)
	{
		this->in_nodes = graph->getNodes();
		this->in_edges = graph->getEdges();
		this->qmetaNodes = graph->getMetaNodes();
		this->qmetaEdges = graph->getMetaEdges();
	}
	else
	{
		this->in_nodes = new QMap<qlonglong, osg::ref_ptr<Data::Node> >;
		this->in_edges = new QMap<qlonglong, osg::ref_ptr<Data::Edge> >;
		this->qmetaNodes = new QMap<qlonglong, osg::ref_ptr<Data::Node> >;
		this->qmetaEdges = new QMap<qlonglong, osg::ref_ptr<Data::Edge> >;
	}

	QMapIterator<qlonglong, osg::ref_ptr<Data::Edge> > i(*in_edges);

	while (i.hasNext())
	{
		i.next();
		i.value()->setCamera(camera);
	}

	this->nodesGroup = new Vwr::NodeGroup(in_nodes);
	graphGroup->addChild(nodesGroup->getGroup());
	nodesPosition = currentPos++;

	this->edgesGroup = new Vwr::EdgeGroup(in_edges, appConf->getValue("Viewer.Textures.EdgeScale").toFloat());
	//this->edgesGroup = new Vwr::EdgeGroup(in_edges, 10);
	graphGroup->addChild(edgesGroup->getGroup());
	edgesPosition = currentPos++;

	this->qmetaNodesGroup = new Vwr::NodeGroup(qmetaNodes);
	graphGroup->addChild(qmetaNodesGroup->getGroup());
	qmetaNodesPosition = currentPos++;

	this->qmetaEdgesGroup = new Vwr::EdgeGroup(qmetaEdges, appConf->getValue("Viewer.Textures.EdgeScale").toFloat());
	//this->qmetaEdgesGroup = new Vwr::EdgeGroup(qmetaEdges, 10);
	graphGroup->addChild(qmetaEdgesGroup->getGroup());
	qmetaEdgesPosition = currentPos++;

	graphGroup->addChild(initEdgeLabels());
	labelsPosition = currentPos++;

	this->restrictionVisualizationsGroup = QSharedPointer<Vwr::RestrictionVisualizationsGroup> (new Vwr::RestrictionVisualizationsGroup);
	graphGroup->addChild (restrictionVisualizationsGroup->getGroup ());
	restrictionVisualizationsPosition = currentPos++;

	this->restrictionManipulatorsGroup = QSharedPointer<Vwr::RestrictionManipulatorsGroup> (new Vwr::RestrictionManipulatorsGroup);
	graphGroup->addChild (restrictionManipulatorsGroup->getGroup ());
	restrictionManipulatorsPosition = currentPos++;

	if (this->graph != NULL) {
		graph->getRestrictionsManager().setObservers (restrictionVisualizationsGroup, restrictionManipulatorsGroup);
	}

	customNodesPosition = currentPos;

	osgUtil::Optimizer opt;
	opt.optimize(edgesGroup->getGroup(), osgUtil::Optimizer::CHECK_GEOMETRY);
}

void CoreGraph::cleanUp()
{
	if (this->graph == NULL)
	{
		delete in_nodes;
		delete in_edges;
		delete qmetaNodes;
		delete qmetaEdges;
	} else {
		graph->getRestrictionsManager ().resetObserver ();
	}

	delete qmetaEdgesGroup;
	delete edgesGroup;
}



#ifdef OPENCV_FOUND
osg::ref_ptr<osg::Node> CoreGraph::createTextureBackground()
{

	// rectangle
	// coordinates
	osg::Vec3Array* coords = new osg::Vec3Array(4);
	(*coords)[0].set( -1.0f, 1.0f, -1.0f );
	(*coords)[1].set(  1.0f, 1.0f, -1.0f );
	(*coords)[2].set(  1.0f, 1.0f,  1.0f );
	(*coords)[3].set( -1.0f, 1.0f,  1.0f );

	// normals
	osg::Vec3Array* normals = new osg::Vec3Array(1);
	(*normals)[0].set( 0.0f, 1.0f, 0.0f );

	// texture coordinates
	osg::Vec2Array* texCoords = new osg::Vec2Array(4);
	(*texCoords)[0].set(0.0f, 1.0f);
	(*texCoords)[1].set(1.0f, 1.0f);
	(*texCoords)[2].set(1.0f, 0.0f);
	(*texCoords)[3].set(0.0f, 0.0f);

	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
	geom->setDataVariance(osg::Object::DYNAMIC);
	geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
	geom->setVertexArray(coords);
	geom->setNormalArray(normals);
	geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	geom->setTexCoordArray(0,texCoords);


	// texture
	mCameraStream = new OpenCV::CameraStream( geom );
	mCameraStream->setDataVariance(osg::Object::DYNAMIC);

	osg::ref_ptr<osg::Texture2D> skymap = new osg::Texture2D( mCameraStream );
	skymap->setDataVariance(osg::Object::DYNAMIC);
	skymap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	skymap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	skymap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	skymap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	skymap->setResizeNonPowerOfTwoHint(false);



	// stateset
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
	stateset->setRenderBinDetails(-1,"RenderBin");

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1, 1);
	stateset->setAttributeAndModes(depth, osg::StateAttribute::ON );



	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->setCullingActive(false);
	geode->setStateSet(stateset);
	geode->addDrawable(geom);

	osg::ref_ptr<osg::Transform> transform = new SkyTransform;

	//osg::ref_ptr<osg::Transform> transform = new MoveEarthySkyWithEyePointTran;
	transform->setCullingActive(false);
	transform->addChild(geode);

	osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	clearNode->setRequiresClear(false);
	clearNode->addChild(transform);
	//clearNode->addChild(geode);


	return clearNode;

}

osg::ref_ptr<osg::Node> CoreGraph::createOrtho2dBackground()
{

	osg::ref_ptr<osg::Geode> GeodeHUD = new osg::Geode();

	osg::ref_ptr<osg::Projection> ProjectionMatrixHUD = new osg::Projection;
	osg::ref_ptr<osg::MatrixTransform> ModelViewMatrixHUD = new osg::MatrixTransform;

	ModelViewMatrixHUD->setMatrix(osg::Matrix::identity());
	ModelViewMatrixHUD->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

	ProjectionMatrixHUD->setMatrix(osg::Matrix::ortho2D(0,640,0,480));
	ProjectionMatrixHUD->addChild( ModelViewMatrixHUD );
	ModelViewMatrixHUD->addChild( GeodeHUD );


	osg::Vec3Array* coordsHUD = new osg::Vec3Array;
	coordsHUD->push_back( osg::Vec3(   0,    0, -1 ));
	coordsHUD->push_back( osg::Vec3( 640,    0, -1 ));
	coordsHUD->push_back( osg::Vec3( 640,  480, -1 ));
	coordsHUD->push_back( osg::Vec3(   0,  480, -1 ));

	osg::Vec2Array* texCoords = new osg::Vec2Array(4);
	(*texCoords)[0].set( 0.0f, 1.0f);
	(*texCoords)[1].set( 1.0f, 1.0f);
	(*texCoords)[2].set( 1.0f, 0.0f);
	(*texCoords)[3].set( 0.0f, 0.0f);

	osg::Vec3Array* normalsHUD = new osg::Vec3Array;
	normalsHUD->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));


	osg::ref_ptr<osg::Geometry> GeomHUD = new osg::Geometry();
	GeomHUD->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POLYGON,0,4));
	GeomHUD->setVertexArray(coordsHUD);
	GeomHUD->setNormalArray(normalsHUD);
	GeomHUD->setNormalBinding(osg::Geometry::BIND_OVERALL);
	GeomHUD->setTexCoordArray(0,texCoords);


	mCameraStream = new OpenCV::CameraStream();
	mCameraStream->setDataVariance(osg::Object::DYNAMIC);

	osg::ref_ptr<osg::Texture2D> textureHUD = new osg::Texture2D( mCameraStream );
	textureHUD->setDataVariance(osg::Object::DYNAMIC);
	textureHUD->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	textureHUD->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	textureHUD->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	textureHUD->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	textureHUD->setResizeNonPowerOfTwoHint(false);



	osg::ref_ptr<osg::StateSet> statesetHUD = new osg::StateSet();
	statesetHUD->setTextureAttributeAndModes(0, textureHUD, osg::StateAttribute::ON);
	statesetHUD->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	statesetHUD->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
	statesetHUD->setMode(GL_BLEND,osg::StateAttribute::OFF);


	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1, 1);
	statesetHUD->setAttributeAndModes(depth, osg::StateAttribute::ON );
	statesetHUD->setRenderBinDetails( -1, "RenderBin");


	GeodeHUD->setStateSet(statesetHUD);
	GeodeHUD->addDrawable( GeomHUD );

	osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	clearNode->setRequiresClear(false);
	clearNode->addChild( ProjectionMatrixHUD );

	return clearNode;

}
#endif

osg::ref_ptr<osg::Node> CoreGraph::createSkyNoiseBox()
{
	unsigned char red = (unsigned char) appConf->getValue("Viewer.Display.BackGround.R").toInt();
	unsigned char green = (unsigned char) appConf->getValue("Viewer.Display.BackGround.G").toInt();
	unsigned char blue =(unsigned char) appConf->getValue("Viewer.Display.BackGround.B").toInt() ;
	osg::ref_ptr<osg::Texture2D> skymap =
			PerlinNoiseTextureGenerator::getCoudTexture(2048, 1024,
														red,
														green,
														blue,
														255);

	skymap->setDataVariance(osg::Object::DYNAMIC);
	skymap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	skymap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	skymap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	skymap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	stateset->setTextureAttributeAndModes(0, skymap, osg::StateAttribute::ON);
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
	stateset->setRenderBinDetails(-1,"RenderBin");

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction(osg::Depth::ALWAYS);
	depth->setRange(1, 1);
	stateset->setAttributeAndModes(depth, osg::StateAttribute::ON );

	osg::ref_ptr<osg::Drawable> drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0.0f,0.0f,0.0f), 1));
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	geode->setCullingActive(false);
	geode->setStateSet(stateset);
	geode->addDrawable(drawable);

	osg::ref_ptr<osg::Transform> transform = new SkyTransform;
	transform->setCullingActive(false);
	transform->addChild(geode);

	osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	clearNode->setRequiresClear(false);
	clearNode->addChild(transform);

	return clearNode;
}


osg::ref_ptr<osg::Node> CoreGraph::createBackground(){

	// skybox
	int background = appConf->getValue("Viewer.SkyBox.Noise").toInt();


	if ( background == 0) {
		SkyBox * skyBox = new SkyBox;
		return skyBox->createSkyBox();
	}

	// skynoise
	if ( background == 1) {
		return createSkyNoiseBox();
	}

#ifdef OPENCV_FOUND
	// video backgroung as 3d rectangle
	if ( background == 2) {
		return createTextureBackground();
	}

	// video backgroung as rectangle in ortho2d
	if ( background == 3) {
		return createOrtho2dBackground();
	}
#endif

	return NULL;
}


osg::ref_ptr<osg::Group> CoreGraph::initEdgeLabels()
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = in_edges->begin();

	while (i != in_edges->end())
	{
		geode->addDrawable(i.value()->createLabel(i.value()->getName()));
		i++;
	}

	osg::ref_ptr<osg::Group> labels = new osg::Group;
	labels->addChild(geode);
	labels->setNodeMask(0);

	return labels;
}

osg::ref_ptr<osg::Group> CoreGraph::initCustomNodes()
{
	osg::ref_ptr<osg::Group> customNodes = new osg::Group;

	QLinkedList<osg::ref_ptr<osg::Node> >::const_iterator i = customNodeList.constBegin();
	while (i != customNodeList.constEnd())
	{
		customNodes->addChild(*i);
		++i;
	}
	return customNodes;
}

/*!
 *
 *
 * Zaktualizuje pozicie uzlov a hran na obrazovke.
 *
 */
void CoreGraph::update()
{
	graphGroup->removeChildren(customNodesPosition,1);

	synchronize();

	if (!this->nodesFreezed)
	{
		float interpolationSpeed = appConf->getValue("Viewer.Display.InterpolationSpeed").toFloat();
		nodesGroup->updateNodeCoordinates(interpolationSpeed);
		qmetaNodesGroup->updateNodeCoordinates(interpolationSpeed);
	}
	else
	{
		nodesGroup->updateNodeCoordinates(1);
		qmetaNodesGroup->updateNodeCoordinates(1);
	}

	edgesGroup->updateEdgeCoords();
	qmetaEdgesGroup->updateEdgeCoords();
	graphGroup->addChild(initCustomNodes());

	//posli layout ostatnym klientom (ak nejaki su)
	Network::Server *server = Network::Server::getInstance();
	int time = QTime::currentTime().msec();
	if (abs(time - prevTime) > 100) {
		server -> sendLayout();
		prevTime = time;
	}
}

void CoreGraph::synchronize()
{
	nodesGroup->synchronizeNodes();
	edgesGroup->synchronizeEdges();
	qmetaNodesGroup->synchronizeNodes();
	qmetaEdgesGroup->synchronizeEdges();
}

void CoreGraph::setEdgeLabelsVisible(bool visible)
{
	graphGroup->getChild(labelsPosition)->setNodeMask(visible);
}

void CoreGraph::setNodeLabelsVisible(bool visible)
{
	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = in_nodes->constBegin();

	while (i != in_nodes->constEnd())
	{
		(*i)->showLabel(visible);
		++i;
	}
}

void CoreGraph::reloadConfig()
{
	root->setChild(backgroundPosition, createBackground());

	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = in_nodes->constBegin();

	while (i != in_nodes->constEnd())
	{
		(*i)->reloadConfig();
		++i;
	}

	graphGroup->setChild(labelsPosition, initEdgeLabels());
	Vwr::TextureWrapper::reloadTextures();
}

CoreGraph::~CoreGraph(void)
{
	cleanUp();
}

void CoreGraph::setNodesFreezed(bool val)
{
	this->nodesFreezed = val;
	nodesGroup->freezeNodePositions();
	qmetaNodesGroup->freezeNodePositions();
}

void CoreGraph::updateGraphRotByAruco(const osg::Quat quat )
{
	mRotAruco = quat;
	computeGraphRotTransf();
}

void CoreGraph::updateGraphRotByMouse( const osg::Quat quat )
{
	mRotMouse = mRotMouse * quat;
	computeGraphRotTransf();
}

void CoreGraph::updateGraphRotByFaceDet( const osg::Quat quat )
{
	mRotFaceDet = quat;
	computeGraphRotTransf();
}

void CoreGraph::computeGraphRotTransf()
{
	osg::Matrixd graphTransfMat( mRotMouse * mRotAruco * mRotFaceDet );
	graphRotTransf->setMatrix(graphTransfMat);
}

#ifdef OPENCV_FOUND
OpenCV::CameraStream* CoreGraph::getCameraStream() const
{
	return mCameraStream;
}
#endif

