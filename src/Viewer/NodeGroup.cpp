#include "Viewer/NodeGroup.h"

#include "Data/Edge.h"
#include "Util/ApplicationConfig.h"
#include "LuaGraph/LuaGraph.h"
#include "City/Module.h"

#include <osg/ShapeDrawable>

#include "typeinfo"
#include "Data/Node.h"
#include "Data/OsgNode.h"

namespace Vwr {

NodeGroup::NodeGroup( QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes )
{
	this->nodes = nodes;
	this->appConf = Util::ApplicationConfig::get();
	this->nodeTransforms = new QMap<qlonglong, osg::ref_ptr<osg::Transform> >;

	initNodes();
}

NodeGroup::~NodeGroup( void )
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
	//osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
	//at->setPosition(node->getTargetPosition() * graphScale);
	//at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
	//at->addChild(node);


	osg::ref_ptr<osg::Group> nodeGroup = new osg::Group();

	if ( appConf->getValue( "Viewer.Display.NodesAlwaysOnTop" ).toInt() ) {
		nodeGroup->getOrCreateStateSet()->setRenderBinDetails( 100, "RenderBin" );
	}

	float graphScale = appConf->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();

	QMapIterator<qlonglong, osg::ref_ptr<Data::Node> > i( *nodes );

	while ( i.hasNext() ) {
		i.next();

		nodeGroup->addChild( wrapChild( i.value(), graphScale ) );

		osg::ref_ptr<osg::Group> g = getNodeGroup( i.value(), NULL, graphScale );

		if ( g != NULL ) {
			g->setNodeMask( g->getNodeMask() & static_cast< unsigned int >( ~0x2 ) );
			nodeGroup->addChild( g );
		}

	}

	this->group = nodeGroup;
}

osg::ref_ptr<osg::Group> NodeGroup::getNodeGroup( osg::ref_ptr<Data::Node> node, osg::ref_ptr<Data::Edge> parentEdge, float graphScale )
{
	osg::ref_ptr<osg::Group> group = NULL;

	if ( !nodeTransforms->contains( node->getId() ) ) {
		group = new osg::Group;

		group->addChild( wrapChild( node, graphScale ) );

		if ( node->isParentNode()==true ) {
			osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
			//at->setPosition(node->getTargetPosition() * graphScale);
			at->setPosition( node->restrictedTargetPosition() * graphScale );
			at->setAutoRotateMode( osg::AutoTransform::ROTATE_TO_SCREEN );

			osg::ShapeDrawable* shape = new osg::ShapeDrawable;
			osg::Sphere* sphere = new osg::Sphere;
			sphere->setRadius( 10 );
			shape->setShape( sphere );

			shape->setColor( osg::Vec4( 0.9f, 0.1f, 0.3f, 0.5f ) );
			shape->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
			shape->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
			osg::Geode* geode = new osg::Geode;
			geode->addDrawable( shape );

			at->addChild( geode );

			node->setOutBall( at );

			group->addChild( at );
		}

		QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = node->getEdges()->begin();

		while ( i != node->getEdges()->end() ) {
			if ( *i != parentEdge ) {
				osg::ref_ptr<Data::Node> n = NULL;

				if ( node->getId() == ( *i )->getSrcNode()->getId() ) {
					n = ( *i )->getDstNode();
				}
				else {
					n = ( *i )->getSrcNode();
				}

				osg::ref_ptr<osg::Group> nodeGroup = getNodeGroup( n, *i, graphScale );

				if ( nodeGroup != NULL ) {
					group->addChild( nodeGroup );
				}
			}

			++i;
		}
	}

	return group;
}

osg::ref_ptr<osg::Transform> NodeGroup::wrapChild( osg::ref_ptr<Data::Node> node, float graphScale )
{
	osg::ref_ptr<osg::PositionAttitudeTransform> at = new osg::PositionAttitudeTransform;
	//at->setPosition(node->getTargetPosition() * graphScale);
	at->setPosition( node->restrictedTargetPosition() * graphScale );
	at->addChild( node );
	node->setNodeMask( 0x1 );
	//node->setColor(osg::Vec4(1, 1, 1, 1));new Data::Node( id, name, type, this->getNodeScaleByType( type ), this, position );
	osg::ref_ptr<Data::Node> queried = new Data::Node( node->getId()+10000,node->Data::AbsNode::getName(),node->getType(),node->getType()->getScale(),node->getGraph(),node->getTargetPosition() );
	//queried->setColor(osg::Vec4(2,2,2,2));
	//node->setColor(osg::Vec4(1, 1, 1, 1));
	queried->setNodeMask( 0x2 );

	at->addChild( queried );

	nodeTransforms->insert( queried->getId(), at );
	nodeTransforms->insert( node->getId(), at );

	return at;
}

void NodeGroup::synchronizeNodes()
{
	//navyse

	QList<qlonglong> nodeKeys = nodes->keys();

	QList<qlonglong> nodeTransformsKeys = nodeTransforms->keys();
	QSet<qlonglong> result = nodeTransformsKeys.toSet().subtract( nodeKeys.toSet() );

	QSet<qlonglong>::const_iterator i = result.constBegin();

	while ( i != result.constEnd() ) {

		nodeTransforms->remove( *i );
		++i;
	}

	result = nodeKeys.toSet().subtract( nodeTransformsKeys.toSet() );
	i = result.constBegin();

	float graphScale = appConf->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();

	while ( i != result.constEnd() ) {
		group->addChild( wrapChild( nodes->value( *i ), graphScale ) );
		++i;
	}


}

void NodeGroup::updateNodeCoordinates( float interpolationSpeed )
{
	QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator i;
	for ( i = nodes->begin();
			i != nodes->end();
			++i ) {

		//Illes have to be updated
		if ( i.value()->isInModule() ) {
			continue;
		}
		auto modulePAT = i.value()->getModule();
		if ( modulePAT ) {
			modulePAT->updateNodesPosition();
		}

		//string a = typeid (i.value()).name();
		//string b = typeid (Data::Node).name();
		//if(typeid (i.value()).name() == "aa")
		//;
		auto transform = nodeTransforms->value( i.key() ).get();
		auto posAttrTransform = dynamic_cast<osg::PositionAttitudeTransform*>( transform );
		auto autoTransform = dynamic_cast<osg::AutoTransform*>( transform );
		if ( posAttrTransform ) {
			posAttrTransform->setPosition( ( *i )->getCurrentPosition( true, interpolationSpeed ) );
		}
		else if ( autoTransform ) {
			autoTransform->setPosition( ( *i )->getCurrentPosition( true, interpolationSpeed ) );
		}

		osg::ref_ptr<osg::AutoTransform> at = NULL;
		at = i.value()->getOutBall();

		if ( at!=NULL ) {
			i.value()->getOutBall()->setPosition( ( *i )->getCurrentPosition( true, interpolationSpeed ) );
		}

	}

//	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = nodes->constBegin();

//	while ( i != nodes->constEnd() ) {

//		//Illes have to be updated
//		if ( i.value()->isInModule() ) {
//			++i;
//			continue;
//		}

//		//string a = typeid (i.value()).name();
//		//string b = typeid (Data::Node).name();
//		//if(typeid (i.value()).name() == "aa")
//		//;
//		auto transform = nodeTransforms->value( i.key() ).get();
//		auto posAttrTransform = dynamic_cast<osg::PositionAttitudeTransform*>( transform );
//		auto autoTransform = dynamic_cast<osg::AutoTransform*>( transform );
//		if ( posAttrTransform ) {
//			posAttrTransform->setPosition( ( *i )->getCurrentPosition( true, interpolationSpeed ) );
//		}
//		else if ( autoTransform ) {
//			autoTransform->setPosition( ( *i )->getCurrentPosition( true, interpolationSpeed ) );
//		}

//		osg::ref_ptr<osg::AutoTransform> at = NULL;
//		at = i.value()->getOutBall();

//		if ( at!=NULL ) {
//			i.value()->getOutBall()->setPosition( ( *i )->getCurrentPosition( true, interpolationSpeed ) );
//		}

//		++i;
//	}
}

void NodeGroup::freezeNodePositions()
{
	float graphScale = appConf->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();

	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = nodes->constBegin();

	while ( i != nodes->constEnd() ) {
		( *i )->setTargetPosition( ( *i )->getCurrentPosition() / graphScale );
		++i;
	}
}
} // namespace Vwr
