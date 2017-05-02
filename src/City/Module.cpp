#include "City/Module.h"
#include <Layout/LayoutAlgorithms.h>
#include <Shapes/Cuboid.h>
#include <Util/ApplicationConfig.h>
#include "Manager/ResourceManager.h"

static const float DEFAULT_MODULE_SECTOR_HEIGHT = 3.5f;
static const float DEFAULT_BUILDING_SPACING = 0.5f;

City::Module::Module()
{	
	variablesPAT = new osg::PositionAttitudeTransform();
	functionsPAT = new osg::PositionAttitudeTransform();	
	interfacesPAT = new osg::PositionAttitudeTransform();
	addChild( variablesPAT );
	addChild( functionsPAT );
	addChild( interfacesPAT );
	getOrCreateStateSet()->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );
}


void City::Module::addVariableNode( osg::ref_ptr<Data::Node> variableNode )
{
	variableNodes << variableNode;
}

void City::Module::addFunctionNode( osg::ref_ptr<Data::Node> functionNode )
{
	functionNodes << functionNode;
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

void City::Module::showLabels( bool state )
{
	forEachList( [state]( osg::ref_ptr<Data::Node> node ) {
		auto building = node->getBuilding();
		if ( building ) { building->showLabel( state ); }
		auto ball = node->getModuleBall();
		if ( ball ) { ball->showLabel( state ); }
	} );
}

void City::Module::selectAll( bool state )
{
	if ( !state ) {
		forEachList( [state]( osg::ref_ptr<Data::Node> node ) {
			auto building = node->getBuilding();
			if ( building ) { building->select( state ); }
			auto ball = node->getModuleBall();
			if ( ball ) { ball->select( state ); }
		} );
	}

}

void City::Module::refresh()
{
	// nacitanie konfiguracii z aplikacie
	auto config = Util::ApplicationConfig::get();
	const float MODULE_SECTOR_HEIGHT = config->getFloatValue( "City.Module.SectorPlaneHeight", DEFAULT_MODULE_SECTOR_HEIGHT );
	const float BUILDING_SPACING = config->getFloatValue( "City.Module.BuildingSpacing", DEFAULT_BUILDING_SPACING );

	static const osg::BoundingBox zeroBoudingBox( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );

	// vymazanie starej geometrie vsetkych regionov
	variablesPAT->removeChildren( 0, variablesPAT->getNumChildren() );
	functionsPAT->removeChildren( 0, functionsPAT->getNumChildren() );
	interfacesPAT->removeChildren( 0, interfacesPAT->getNumChildren() );



	osg::BoundingBox varRegion;
	QList<osg::Vec3> varLayouts;
	Layout::LayoutAlgorithms::layoutInsideRegion( variableNodes.empty() ? zeroBoudingBox : variableNodes.first()->getBuilding()->getBoundingBox(), variableNodes.count(), MODULE_SECTOR_HEIGHT, BUILDING_SPACING, &varLayouts, &varRegion );
	for ( int i = 0; i < variableNodes.count(); ++i ) {
		auto varNodePAT = getNodeParentPAT( variableNodes[i] );
		varNodePAT->setPosition( varLayouts[i] );
		variablesPAT->addChild( varNodePAT );

		auto otherNode = otherNodes.value( variableNodes[i] );
		if( otherNode ) {
			std::cout << "found something" << std::endl;
			std::cout << "    -> {" << variableNodes[i]->AbsNode::getName().toStdString() << ", " <<  otherNode->AbsNode::getName().toStdString() << "}" << std::endl;
			auto otherNodePAT = getNodeParentPAT( otherNode );
			otherNodePAT->setPosition( varNodePAT->getPosition() + osg::Vec3( 0.0f, 0.0f, 10.0f ) );
			variableNodes[i]->getResidenceAsPAT()->addChild( otherNodePAT );
		}
	}
	variablesPAT->addChild( new Shapes::Cuboid( varRegion.xMax() - varRegion.xMin(), MODULE_SECTOR_HEIGHT, varRegion.yMax() - varRegion.yMin(), osg::Vec3( 0, 0, MODULE_SECTOR_HEIGHT / 2 ) ) );

	osg::BoundingBox funcRegion = varRegion;
	QList<Layout::ElementLayout> funcLayouts;
	Layout::LayoutAlgorithms::layoutAroundRegion( functionNodes.empty() ? zeroBoudingBox : functionNodes.first()->getBuilding()->getBoundingBox(), functionNodes.count(), varRegion, BUILDING_SPACING, &funcLayouts, &funcRegion );
	for ( int i = 0; i < functionNodes.count(); ++i ) {
		auto funcNodePAT = getNodeParentPAT( functionNodes[i] );
		funcNodePAT->setPosition( funcLayouts[i].position + osg::Vec3( 0.0f, 0.0f, MODULE_SECTOR_HEIGHT ) );
		funcNodePAT->setAttitude( osg::Quat( funcLayouts[i].yawRotation, osg::Vec3( 0.0f, 0.0f, 1.0f ) ) );
		functionsPAT->addChild( funcNodePAT );
	}

	osg::BoundingBox funcPlane( funcRegion.xMin(), funcRegion.yMin(), 0, funcRegion.xMax(), funcRegion.yMax(), MODULE_SECTOR_HEIGHT );
	functionsPAT->addChild( new Shapes::Cuboid( funcPlane ) );

	osg::BoundingBox intrfcRegion = funcPlane;
	QList<Layout::ElementLayout> intrfcLayouts;
	Layout::LayoutAlgorithms::layoutAroundRegion( interfaceNodes.empty() ? zeroBoudingBox : interfaceNodes.first()->getBuilding()->getBoundingBox(), interfaceNodes.count(), funcRegion, BUILDING_SPACING, &intrfcLayouts, &intrfcRegion );
	for ( int i = 0; i < interfaceNodes.count(); ++i ) {
		auto intrfcNodePAT = getNodeParentPAT( interfaceNodes[i] );
		intrfcNodePAT->setPosition( intrfcLayouts[i].position + osg::Vec3( 0.0f, 0.0f, MODULE_SECTOR_HEIGHT ) );
		intrfcNodePAT->setAttitude( osg::Quat( intrfcLayouts[i].yawRotation, osg::Vec3( 0.0f, 0.0f, 1.0f ) ) );
		interfacesPAT->addChild( intrfcNodePAT );
	}
	osg::BoundingBox intrfcPlane( intrfcRegion.xMin(), intrfcRegion.yMin(), 0, intrfcRegion.xMax(), intrfcRegion.yMax(), MODULE_SECTOR_HEIGHT );
	interfacesPAT->addChild( new Shapes::Cuboid( intrfcPlane ) );

	const auto offset = intrfcRegion.center();
	float moduleSectorOffset = 0;
	interfacesPAT->setPosition( osg::Vec3( -offset.x(), -offset.y(), moduleSectorOffset ) );
	moduleSectorOffset += MODULE_SECTOR_HEIGHT;
	functionsPAT->setPosition( osg::Vec3( -offset.x(), -offset.y(), moduleSectorOffset ) );
	moduleSectorOffset += MODULE_SECTOR_HEIGHT;
	variablesPAT->setPosition( osg::Vec3( -offset.x(), -offset.y(), moduleSectorOffset ) );
	moduleSectorOffset += MODULE_SECTOR_HEIGHT;
}

void City::Module::decompose()
{
	for ( auto varNode : variableNodes ) {
		varNode->clearResidence( 1 ); //from 1 - do not delete Building at index 0
	}

	variablesPAT->removeChildren( 0, variablesPAT->getNumChildren() );
	functionsPAT->removeChildren( 0, functionsPAT->getNumChildren() );
	interfacesPAT->removeChildren( 0, interfacesPAT->getNumChildren() );

	moduleNode->clearResidence( 0 );
	//from 0 - delete variablesPAT, functionsPAT, interfacesPAT
	//same as:
	//removeChild( variablesPAT );
	//removeChild( functionsPAT );
	//removeChild( interfacesPAT );

	variableNodes.clear();
	functionNodes.clear();
	interfaceNodes.clear();
	otherNodes.clear();

	auto resMgr = Manager::ResourceManager::getInstance();
	auto white = resMgr->getMaterial( osg::Vec3( 1.0f, 1.0f, 1.0f ) );

	auto config = Util::ApplicationConfig::get();
	const float NODE_BASE_SIZE = config->getFloatValue( "City.Building.LuaNodeBaseSize", 4.0f );
	const float NODE_HEIGHT = config->getFloatValue( "City.Building.LuaNodeHeight", 4.0f );

	//add Building to moduleNode
	auto building = new City::Building();
	building->setLabel(moduleNode->AbsNode::getName());
	building->setBaseSize( NODE_BASE_SIZE );
	building->setHeight( NODE_HEIGHT );
	building->setLieOnGround( false );
	building->setStateSet( new osg::StateSet() );
	building->getStateSet()->setAttribute( white );
	building->getStateSet()->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );
	building->refresh();
	moduleNode->setResidence( building );
}

void City::Module::updateNodesPosition()
{
	auto config = Util::ApplicationConfig::get();
	float interpolationSpeed = config->getValue( "Viewer.Display.InterpolationSpeed" ).toFloat();

	for ( auto varNode : variableNodes ) {
		auto varNodePAT = getNodeParentPAT( varNode );
		varNode->setCurrentPosition( varNodePAT->getPosition() + moduleNode->getCurrentPosition( true, interpolationSpeed ) );
	}

	for ( auto otherNode : otherNodes ) {
		auto otherNodePAT = getNodeParentPAT( otherNode );
		otherNode->setCurrentPosition( otherNodePAT->getPosition() + moduleNode->getCurrentPosition( true, interpolationSpeed ) );
	}

	for ( auto funcNode : functionNodes ) {
		auto funcNodePAT = getNodeParentPAT( funcNode );
		funcNode->setCurrentPosition( funcNodePAT->getPosition() + moduleNode->getCurrentPosition( true, interpolationSpeed ) );
	}

	for ( auto intrfcNode : interfaceNodes ) {
		auto intrfcNodePAT = getNodeParentPAT( intrfcNode );
		intrfcNode->setCurrentPosition( intrfcNodePAT->getPosition() + moduleNode->getCurrentPosition( true, interpolationSpeed ) );
	}
}

void City::Module::forEachList( std::function<void( osg::ref_ptr<Data::Node> node )> func )
{
	for ( auto node : variableNodes ) {
		func( node );
	}
	for ( auto node : functionNodes ) {
		func( node );
	}
	for ( auto node : otherNodes ) {
		func( node );
	}
	for ( auto node : interfaceNodes ) {
		func( node );
	}
}
