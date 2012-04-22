#include "Viewer/CoreGraph.h"
#include "Viewer/SkyBox.h"
#include <osgUtil/Optimizer>

#include "Network/Server.h"

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
		for (int x = 7; x > 0; x--)
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
	//this->edgesGroup = new Vwr::EdgeGroup(in_edges, 10);
	root->addChild(edgesGroup->getGroup());
	edgesPosition = currentPos++;

	this->qmetaNodesGroup = new Vwr::NodeGroup(qmetaNodes);
	root->addChild(qmetaNodesGroup->getGroup());
	qmetaNodesPosition = currentPos++;

	this->qmetaEdgesGroup = new Vwr::EdgeGroup(qmetaEdges, appConf->getValue("Viewer.Textures.EdgeScale").toFloat());
	//this->qmetaEdgesGroup = new Vwr::EdgeGroup(qmetaEdges, 10);
	root->addChild(qmetaEdgesGroup->getGroup());
	qmetaEdgesPosition = currentPos++;

	root->addChild(initEdgeLabels());
	labelsPosition = currentPos++;

	this->restrictionVisualizationsGroup = QSharedPointer<Vwr::RestrictionVisualizationsGroup> (new Vwr::RestrictionVisualizationsGroup);
	root->addChild (restrictionVisualizationsGroup->getGroup ());
	restrictionVisualizationsPosition = currentPos++;
	if (this->graph != NULL) {
		graph->getRestrictionsManager ().setObserver (restrictionVisualizationsGroup);
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



osg::ref_ptr<osg::Node> CoreGraph::createSkyBox(){
        SkyBox * skyBox = new SkyBox;
        return skyBox->createSkyBox();
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
