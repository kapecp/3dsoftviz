#include "Viewer/CoreGraph.h"
#include <osgUtil/Optimizer>

using namespace Vwr;

/*
* TODO prerobit - v sucastnosti je scena jeden velky plochy graf. toto sa da optimalizovat do stromovej strukutry. pri vytvarani grafu ho treba prechadzat ako graf
* a nie vsetko zaradom ako je to teraz
*/


Vwr::CoreGraph::CoreGraph(Data::Graph * graph, osg::ref_ptr<osg::Camera> camera)
{
	this->graph = graph;
    this->camera = (camera == 0 ? (osg::ref_ptr<osg::Camera>) new osg::Camera : camera);

	this->in_nodes = NULL;
	this->in_edges = NULL;
	this->qmetaNodes = NULL;
	this->qmetaEdges = NULL;
	this->nodesFreezed = false;

	this->edgesGroup = NULL;
	this->qmetaEdgesGroup = NULL;

	appConf = Util::ApplicationConfig::get();

	root = new osg::Group();
	root->addChild(createSkyBox());
	backgroundPosition = 0;

	reload(graph);
}

void CoreGraph::reload(Data::Graph * graph)
{
	cleanUp();

	int currentPos = 1;

	if (root->getNumChildren() > 1)
	{
		for (int x = 6; x > 0; x--)
			root->removeChildren(x,1);
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
	root->addChild(nodesGroup->getGroup());
	nodesPosition = currentPos++;

	this->edgesGroup = new Vwr::EdgeGroup(in_edges, appConf->getValue("Viewer.Textures.EdgeScale").toFloat());
	root->addChild(edgesGroup->getGroup());
	edgesPosition = currentPos++;

	this->qmetaNodesGroup = new Vwr::NodeGroup(qmetaNodes);
	root->addChild(qmetaNodesGroup->getGroup());
	qmetaNodesPosition = currentPos++;

	this->qmetaEdgesGroup = new Vwr::EdgeGroup(qmetaEdges, appConf->getValue("Viewer.Textures.EdgeScale").toFloat());
	root->addChild(qmetaEdgesGroup->getGroup());
	qmetaEdgesPosition = currentPos++;

	root->addChild(initEdgeLabels());
	labelsPosition = currentPos++;

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
	}

	delete qmetaEdgesGroup;
	delete edgesGroup;
}



osg::ref_ptr<osg::Node> CoreGraph::createSkyBox()
{
	osg::ref_ptr<osg::Texture2D> skymap = PerlinNoiseTextureGenerator::getCoudTexture(2048, 1024, 
		appConf->getValue("Viewer.Display.BackGround.R").toInt(),
		appConf->getValue("Viewer.Display.BackGround.G").toInt(),
		appConf->getValue("Viewer.Display.BackGround.B").toInt(),
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
	root->removeChildren(customNodesPosition,1);

	synchronize();

	float graphScale = appConf->getValue("Viewer.Display.NodeDistanceScale").toFloat();
	
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
	root->addChild(initCustomNodes());
}

void CoreGraph::synchronize()
{
	qmetaNodesGroup->synchronizeNodes();
	qmetaEdgesGroup->synchronizeEdges();
}

void CoreGraph::setEdgeLabelsVisible(bool visible)
{
	root->getChild(labelsPosition)->setNodeMask(visible);
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
	root->setChild(backgroundPosition, createSkyBox());	

	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = in_nodes->constBegin();

	while (i != in_nodes->constEnd()) 
	{
		(*i)->reloadConfig();
		++i;
	}

	root->setChild(labelsPosition, initEdgeLabels());
	Vwr::TextureWrapper::reloadTextures();
}

CoreGraph::~CoreGraph(void)
{	
	cleanUp();
}