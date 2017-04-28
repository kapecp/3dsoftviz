#include "City/Module.h"
#include <Layout/LayoutAlgorithms.h>
#include <Shapes/Cuboid.h>
#include <Util/ApplicationConfig.h>
#include "Manager/ResourceManager.h"

static const float DEFAULT_MODULE_SECTOR_HEIGHT = 3.5f;
static const float DEFAULT_BUILDING_SPACING = 0.5f;

City::Module::Module()
{	
	functionsPAT = new osg::PositionAttitudeTransform();
	variablesPAT = new osg::PositionAttitudeTransform();
	interfacesPAT = new osg::PositionAttitudeTransform();
	addChild( functionsPAT );
	addChild( variablesPAT );
	addChild( interfacesPAT );
	getOrCreateStateSet()->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );
}

void City::Module::addFunctionNode( osg::ref_ptr<Data::Node> functionNode )
{
	functionNodes << functionNode;
}

void City::Module::addVariableNode( osg::ref_ptr<Data::Node> variableNode )
{
	variableNodes << variableNode;
}

void City::Module::addInterfaceNode( osg::ref_ptr<Data::Node> interfaceNode )
{
	interfaceNodes << interfaceNode;
}

void City::Module::addOtherNode( osg::ref_ptr<Data::Node> parentNode, osg::ref_ptr<Data::Node> otherNode )
{
	otherNodes.insert(parentNode, otherNode);
}

osg::ref_ptr<osg::PositionAttitudeTransform> City::Module::getNodeParentPAT( osg::ref_ptr<Data::Node> node )
{
	return node->getParent(0)->asTransform()->asPositionAttitudeTransform();
}

void City::Module::addBuilding( osg::ref_ptr<City::Building> building )
{
	insertChild( 0, building );
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

	osg::BoundingBox funcRegion;
	QList<osg::Vec3> funcLayouts;
	Layout::LayoutAlgorithms::layoutInsideRegion( functionNodes.empty() ? zeroBoudingBox : functionNodes.first()->getBuilding()->getBoundingBox(), functionNodes.count(), DEFAULT_MODULE_SECTOR_HEIGHT, DEFAULT_BUILDING_SPACING, &funcLayouts, &funcRegion );
	for ( int i = 0; i < functionNodes.count(); ++i ) {
		auto funcNodePAT = getNodeParentPAT( functionNodes[i] );
		funcNodePAT->setPosition( funcLayouts[i] );
		//std::cout << "funcNodePAT[" << i << "] has position: (" << funcLayouts[i].x() << ", " << funcLayouts[i].y() << ", " << funcLayouts[i].z() << ")" << std::endl;
		functionsPAT->addChild( funcNodePAT );
	}
	functionsPAT->addChild( new Shapes::Cuboid( funcRegion.xMax() - funcRegion.xMin(), DEFAULT_MODULE_SECTOR_HEIGHT, funcRegion.yMax() - funcRegion.yMin(), osg::Vec3( 0, 0, DEFAULT_MODULE_SECTOR_HEIGHT / 2 ) ) );

	osg::BoundingBox varRegion = funcRegion;
	QList<Layout::ElementLayout> varLayouts;
	Layout::LayoutAlgorithms::layoutAroundRegion( variableNodes.empty() ? zeroBoudingBox : variableNodes.first()->getBuilding()->getBoundingBox(), variableNodes.count(), funcRegion, DEFAULT_BUILDING_SPACING, &varLayouts, &varRegion );
	for ( int i = 0; i < variableNodes.count(); ++i ) {
		auto varNodePAT = getNodeParentPAT( variableNodes[i] );
		varNodePAT->setPosition( varLayouts[i].position + osg::Vec3( 0.0f, 0.0f, DEFAULT_MODULE_SECTOR_HEIGHT ) );
		varNodePAT->setAttitude( osg::Quat( varLayouts[i].yawRotation, osg::Vec3( 0.0f, 0.0f, 1.0f ) ) );
		variablesPAT->addChild( varNodePAT );
	}
	osg::BoundingBox varPlane( varRegion.xMin(), varRegion.yMin(), 0, varRegion.xMax(), varRegion.yMax(), DEFAULT_MODULE_SECTOR_HEIGHT );
	variablesPAT->addChild( new Shapes::Cuboid( varPlane ) );

	osg::BoundingBox intrfcRegion = varRegion;
	QList<Layout::ElementLayout> intrfcLayouts;
	Layout::LayoutAlgorithms::layoutAroundRegion( interfaceNodes.empty() ? zeroBoudingBox : interfaceNodes.first()->getBuilding()->getBoundingBox(), interfaceNodes.count(), varRegion, DEFAULT_BUILDING_SPACING, &intrfcLayouts, &intrfcRegion );
	for ( int i = 0; i < interfaceNodes.count(); ++i ) {
		auto intrfcNodePAT = getNodeParentPAT( interfaceNodes[i] );
		intrfcNodePAT->setPosition( intrfcLayouts[i].position + osg::Vec3( 0.0f, 0.0f, DEFAULT_MODULE_SECTOR_HEIGHT ) );
		intrfcNodePAT->setAttitude( osg::Quat( intrfcLayouts[i].yawRotation, osg::Vec3( 0.0f, 0.0f, 1.0f ) ) );
		interfacesPAT->addChild( intrfcNodePAT );
	}
	osg::BoundingBox intrfcPlane( intrfcRegion.xMin(), intrfcRegion.yMin(), 0, intrfcRegion.xMax(), intrfcRegion.yMax(), DEFAULT_MODULE_SECTOR_HEIGHT );
	variablesPAT->addChild( new Shapes::Cuboid( intrfcPlane ) );

//	for every node in interfaceNodes:
//		auto interfaceNodePAT = getNodeParentPAT(node);
//		interfacesPAT->addChild(interfaceNodePAT);
//	---the same for functionNodes and variableNodes

//	for every node in otherNodes //is QMap
//		auto otherNodePat = getNodeParentPAT(node.value);
//		auto parentNode = node.key
//		parentNode->addChild(otherNodePat)

	const auto offset = intrfcRegion.center();
	float moduleSectorOffset = 0;
	interfacesPAT->setPosition( osg::Vec3( -offset.x(), -offset.y(), moduleSectorOffset ) );
	moduleSectorOffset += DEFAULT_MODULE_SECTOR_HEIGHT;
	variablesPAT->setPosition( osg::Vec3( -offset.x(), -offset.y(), moduleSectorOffset ) );
	moduleSectorOffset += DEFAULT_MODULE_SECTOR_HEIGHT;
	functionsPAT->setPosition( osg::Vec3( -offset.x(), -offset.y(), moduleSectorOffset ) );
	moduleSectorOffset += DEFAULT_MODULE_SECTOR_HEIGHT;
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
