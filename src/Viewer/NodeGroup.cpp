#include "Viewer/NodeGroup.h"

using namespace Vwr;

NodeGroup::NodeGroup(QMap<qlonglong, osg::ref_ptr<Data::Node> > *nodes)
{
	this->nodes = nodes;
	this->appConf = Util::ApplicationConfig::get();
	this->nodeTransforms = new QMap<qlonglong, osg::ref_ptr<osg::AutoTransform> >;

	initNodes();
}

NodeGroup::~NodeGroup(void)
{
}

/*!
 * 
 * \param scale
 * Koeficient velkosti uzlov.
 * 
 * \returns
 * Podgraf s uzlami.
 * 
 * 
 * Inicializuje uzly na ich pozicie a priradim im textury.
 * 
 */
void NodeGroup::initNodes()
{
	osg::ref_ptr<osg::Group> nodeGroup = new osg::Group();

	if (appConf->getValue("Viewer.Display.NodesAlwaysOnTop").toInt())
		nodeGroup->getOrCreateStateSet()->setRenderBinDetails(100, "RenderBin");

	float graphScale = appConf->getValue("Viewer.Display.NodeDistanceScale").toFloat(); 
	
	QMapIterator<qlonglong, osg::ref_ptr<Data::Node> > i(*nodes);
	
	while (i.hasNext()) 
	{
		i.next();
		
		nodeGroup->addChild(wrapChild(i.value(), graphScale));

		/*
		osg::ref_ptr<osg::Group> g = getNodeGroup(i.value(), NULL, graphScale);

		if (g != NULL)
			nodeGroup->addChild(g);
		*/
	}

	this->group = nodeGroup;
}

osg::ref_ptr<osg::Group> NodeGroup::getNodeGroup(osg::ref_ptr<Data::Node> node, osg::ref_ptr<Data::Edge> parentEdge, float graphScale)
{
	osg::ref_ptr<osg::Group> group = NULL;

	if (!nodeTransforms->contains(node->getId()))
	{
		group = new osg::Group;

		group->addChild(wrapChild(node, graphScale));

		QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = node->getEdges()->begin();

		while (i != node->getEdges()->end()) 
		{
			if (*i != parentEdge)
			{
				osg::ref_ptr<Data::Node> n = NULL;

				if (node->getId() == (*i)->getSrcNode()->getId())
					n = (*i)->getDstNode();
				else
					n = (*i)->getSrcNode();

				osg::ref_ptr<osg::Group> nodeGroup = getNodeGroup(n, *i, graphScale);

				if (nodeGroup != NULL)
					group->addChild(nodeGroup);
			}

			i++;
		}
	}

	return group;
}

osg::ref_ptr<osg::AutoTransform> NodeGroup::wrapChild(osg::ref_ptr<Data::Node> node, float graphScale)
{
	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
	at->setPosition(node->getTargetPosition() * graphScale);
	at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	at->addChild(node);

	nodeTransforms->insert(node->getId(), at);

	return at;
}

void NodeGroup::synchronizeNodes()
{	//navyse
	int count = nodes->size ();

	QList<qlonglong> nodeKeys = nodes->keys();

	QList<qlonglong> nodeTransformsKeys = nodeTransforms->keys();
	QSet<qlonglong> result = nodeTransformsKeys.toSet().subtract(nodeKeys.toSet());

	QSet<qlonglong>::const_iterator i = result.constBegin();

	while (i != result.constEnd()) 
	{	
		bool res = group->removeChild(nodeTransforms->value(*i));

		nodeTransforms->remove(*i);
		++i;
	}

	result = nodeKeys.toSet().subtract(nodeTransformsKeys.toSet());
	i = result.constBegin();

	float graphScale = appConf->getValue("Viewer.Display.NodeDistanceScale").toFloat(); 

	while (i != result.constEnd()) 
	{
		group->addChild(wrapChild(nodes->value(*i), graphScale));
		++i;
	}
}

void NodeGroup::updateNodeCoordinates(float interpolationSpeed)
{
	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = nodes->constBegin();

	while (i != nodes->constEnd()) 
	{
		nodeTransforms->value(i.key())->setPosition((*i)->getCurrentPosition(true, interpolationSpeed));
		++i;
	}
}

void NodeGroup::freezeNodePositions() 
{ 
	float graphScale = appConf->getValue("Viewer.Display.NodeDistanceScale").toFloat();

	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = nodes->constBegin();

	while (i != nodes->constEnd()) 
	{
		(*i)->setTargetPosition((*i)->getCurrentPosition() / graphScale);
		++i;
	}
}
