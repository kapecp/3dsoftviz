#include "Viewer/CoreGraph.h"
#include "Viewer/SkyBox.h"
#include <osgUtil/Optimizer>

#include "Network/Server.h"

#include <math.h>

using namespace Vwr;

/*
* TODO prerobit - v sucastnosti je scena jeden velky plochy graf. toto sa da optimalizovat do stromovej strukutry. pri vytvarani grafu ho treba prechadzat ako graf
* a nie vsetko zaradom ako je to teraz
*/

osg::ref_ptr<osg::AutoTransform> getSphere(osg::Vec3 position, float radius, osg::Vec4 color) {
    osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
    at->setPosition(position * 1);
    at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

    osg::ShapeDrawable * shape = new osg::ShapeDrawable;
    osg::Sphere * sphere = new osg::Sphere;
    sphere->setRadius(radius);
    shape->setShape(sphere);
    shape->setColor(color); //osg::Vec4(0.9, 0.1, 0.3, 0.5));
    shape->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
    shape->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    osg::Geode * geode = new osg::Geode;
    geode->addDrawable(shape);

    at->addChild(geode);
    return at;
}

osg::Vec3f getMidPoint(QSet<Data::Node *> nodes) {

    osg::Vec3 total = osg::Vec3(0,0,0);
    int count = 0;

    QSet<Data::Node *>::const_iterator i = nodes.constBegin();
     while (i != nodes.constEnd()) {

         Data::Node* v = *i;

        osg::Vec3f pos = v->getCurrentPosition();

        total += pos;

         ++i; ++count;
     }
     total.operator /=(count);

     return total;
}

float getRadius(QSet<Data::Node *> nodes, osg::Vec3f midPoint) {

    float maxDistance = 0;

    QSet<Data::Node *>::const_iterator i = nodes.constBegin();
    while (i != nodes.constEnd()) {
        Data::Node* v = *i;
        osg::Vec3f pos = v->getCurrentPosition();

        float newDistance = sqrt(pow(pos.x()-midPoint.x(),2) + pow(pos.y()-midPoint.y(),2) + pow(pos.z()-midPoint.z(),2));

        if (newDistance > maxDistance) {
            maxDistance = newDistance;
        }
        ++i;
    }
    return maxDistance;
}

osg::ref_ptr<osg::Group> CoreGraph::test2() {
/*
    if (testGroup != NULL) {
        return testGroup;
    }*/
//    qDebug() << "***** INIT test2 ";
    testGroup = new osg::Group;

    if (graph != NULL) {

//    Manager::GraphManager * manager = Manager::GraphManager::getInstance();
//    QMap<qlonglong, Data::Type*> * types = manager->getActiveGraph()->getTypes();
//    Data::Type * type = types->value(1);

    QMap<qlonglong, osg::ref_ptr<Data::Node> > clusters = Clustering::Clusterer::getInstance().getClusters();

    QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator i;
//int tempID = 0;
    for (i = clusters.begin(); i != clusters.end(); i++)
    {
        osg::ref_ptr<Data::Node> node = i.value();

        // TODO pripadne prerobit vrece "clusters" nech uchovava len typ Cluster {aj tak v nom nie su Nody}
        Data::Cluster* cluster = dynamic_cast<Data::Cluster*>(node.get());


    //    osg::ref_ptr<Data::Cluster> cluster = node->getCluster();

     //   osg::ref_ptr<Data::Cluster> cluster = new Data::Cluster(tempID++, "name", type, graph->getNodeScale(), graph, osg::Vec3f(0,0,0));

    //    qDebug() << "***** test2 cluster " << cluster->getId() << " count: " << cluster->getClusteredNodesCount();

    //    testGroup->addChild(getSphere(osg::Vec3( cluster->getId() * 10, cluster->getId() * 10, cluster->getId() * 10)));

        osg::Vec3f midPoint = getMidPoint(cluster->getALLClusteredNodes());

        testGroup->addChild(getSphere(midPoint, getRadius(cluster->getALLClusteredNodes(), midPoint), cluster->getColor()));
    }

    }
    return testGroup;
}

osg::Geode* test() {
    //cout << " test ...";
    osg::Geode* pyramidGeode = new osg::Geode();
    osg::Geometry* pyramidGeometry = new osg::Geometry();
    pyramidGeode->addDrawable(pyramidGeometry);

    osg::Vec3Array* pyramidVertices = new osg::Vec3Array;
    pyramidVertices->push_back( osg::Vec3( 0, 0, 0) ); // front left
    pyramidVertices->push_back( osg::Vec3(100, 0, 0) ); // front right
    pyramidVertices->push_back( osg::Vec3(100,100, 0) ); // back right
    pyramidVertices->push_back( osg::Vec3( 0,100, 0) ); // back left
    pyramidVertices->push_back( osg::Vec3( 50, 50,100) ); // peak

    pyramidGeometry->setVertexArray( pyramidVertices );

    osg::DrawElementsUInt* pyramidBase =
    new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
    pyramidBase->push_back(3);
    pyramidBase->push_back(2);
    pyramidBase->push_back(1);
    pyramidBase->push_back(0);
    pyramidGeometry->addPrimitiveSet(pyramidBase);

    osg::DrawElementsUInt* pyramidFaceOne =
    new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    pyramidFaceOne->push_back(0);
    pyramidFaceOne->push_back(1);
    pyramidFaceOne->push_back(4);
    pyramidGeometry->addPrimitiveSet(pyramidFaceOne);

    osg::DrawElementsUInt* pyramidFaceTwo =
    new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    pyramidFaceTwo->push_back(1);
    pyramidFaceTwo->push_back(2);
    pyramidFaceTwo->push_back(4);
    pyramidGeometry->addPrimitiveSet(pyramidFaceTwo);

    osg::DrawElementsUInt* pyramidFaceThree =
    new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    pyramidFaceThree->push_back(2);
    pyramidFaceThree->push_back(3);
    pyramidFaceThree->push_back(4);
    pyramidGeometry->addPrimitiveSet(pyramidFaceThree);

    osg::DrawElementsUInt* pyramidFaceFour =
    new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
    pyramidFaceFour->push_back(3);
    pyramidFaceFour->push_back(0);
    pyramidFaceFour->push_back(4);
    pyramidGeometry->addPrimitiveSet(pyramidFaceFour);

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); //index 0 red
    colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f) ); //index 1 green
    colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); //index 2 blue
    colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f) ); //index 3 white
    colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); //index 4 red

    pyramidGeometry->setColorArray(colors);
    pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    return pyramidGeode;
}



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

    root->addChild(test2());
    currentPos++;

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
    if (appConf->getValue("Viewer.SkyBox.Noise").toInt() == 0) {
        SkyBox * skyBox = new SkyBox;
        return skyBox->createSkyBox();
    } else {
        osg::ref_ptr<osg::Texture2D> skymap =
                PerlinNoiseTextureGenerator::getCoudTexture(2048, 1024,
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

    // TODO ... malo by byt odkomentovane.. zobrazuje to obdlzniky na select viac uzlov
    // ale ked som zakomentoval, tak funguje zobrazovanie zhlukov nizsie
//	root->addChild(initCustomNodes());

    root->addChild(test2());

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
