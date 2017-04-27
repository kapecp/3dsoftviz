#include "City/Module.h"
#include <Layout/LayoutAlgorithms.h>
#include <Shapes/Cuboid.h>
#include <Util/ApplicationConfig.h>
#include "Manager/ResourceManager.h"

static const float DEFAULT_MODULE_SECTOR_HEIGHT = 3.5f;
static const float DEFAULT_BUILDING_SPACING = 0.5f;

City::Module::Module()
{	
	interfacesPAT = new osg::PositionAttitudeTransform();
	functionsPAT = new osg::PositionAttitudeTransform();
	variablesPAT = new osg::PositionAttitudeTransform();
	addChild( interfacesPAT );
	addChild( functionsPAT );
	addChild( variablesPAT );
	getOrCreateStateSet()->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );
}

void City::Module::addInterfaceNode( osg::ref_ptr<Data::Node> interfaceNode )
{	
	interfaceNodes << interfaceNode;
}

void City::Module::addFunctionNode( osg::ref_ptr<Data::Node> functionNode )
{
	functionNodes << functionNode;
}

void City::Module::addVariableNode( osg::ref_ptr<Data::Node> variableNode )
{
	variableNodes << variableNode;
}

void City::Module::addOtherNode( osg::ref_ptr<Data::Node> parentNode, osg::ref_ptr<Data::Node> otherNode )
{
	otherNodes.insert(parentNode, otherNode);
}

osg::ref_ptr<osg::PositionAttitudeTransform> City::Module::getNodeParentPAT( osg::ref_ptr<Data::Node> node )
{
	return node->getParent(0)->asTransform()->asPositionAttitudeTransform();
}


/*
void Module::showLabels( bool state )
{
	forEachBuilding( [state]( Building& b ) {
		b.showLabel( state );
	} );
}
*/

/*
void Module::selectAll( bool state )
{
	if ( !state ) {
		forEachBuilding( [state]( Building& b ) {
			b.select( state );
		} );
	}
}
*/

void City::Module::refresh()
{
	static const osg::BoundingBox zeroBoudingBox( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );

	// vymazanie starej geometrie vsetkych regionov
	interfacesPAT->removeChildren( 0, interfacesPAT->getNumChildren() );
	functionsPAT->removeChildren( 0, functionsPAT->getNumChildren() );
	variablesPAT->removeChildren( 0, variablesPAT->getNumChildren() );

//	for every node in interfaceNodes:
//		auto interfaceNodePAT = getNodeParentPAT(node);
//		interfacesPAT->addChild(interfaceNodePAT);
//	---the same for functionNodes and variableNodes

//	for every node in otherNodes //is QMap
//		auto otherNodePat = getNodeParentPAT(node.value);
//		auto parentNode = node.key
//		parentNode->addChild(otherNodePat)


}

void City::Module::updateNodesPosition()
{
//	for ( int i = 0; i < interfaceBuildings.count(); ++i ) {
//		auto& b = interfaceBuildings[i];
//		auto& node = interfaceNodes[i];
//		node->setTargetPosition(b->getPosition());
//		//node->setRestrictedTargetPosition(b->getPosition());
//		//node->setRestrictedTargetPosition(b->getPosition() + moduleNode->getTargetPosition());
//	}

//	for ( int i = 0; i < functionBuildings.count(); ++i ) {
//		auto& b = functionBuildings[i];
//		auto& node = functionNodes[i];
//		node->setTargetPosition(b->getPosition());
//		//node->setRestrictedTargetPosition(b->getPosition());
//		//node->setRestrictedTargetPosition(b->getPosition() + moduleNode->getTargetPosition());
//	}

//	for ( int i = 0; i < variableBuildings.count(); ++i ) {
//		auto& b = variableBuildings[i];
//		auto& node = variableNodes[i];
//		node->setTargetPosition(b->getPosition());
//		//node->setRestrictedTargetPosition(b->getPosition());
//		//node->setRestrictedTargetPosition(b->getPosition() + moduleNode->getTargetPosition());
//	}

//	for ( int i = 0; i < otherBuildings.count(); ++i ) {
//		auto& b = otherBuildings[i];
//		auto& node = otherNodes[i];
//		node->setTargetPosition(b->getPosition());
//		//node->setRestrictedTargetPosition(b->getPosition());
//		//node->setRestrictedTargetPosition(b->getPosition() + moduleNode->getTargetPosition());
//	}
}
