#include "City/Module.h"
#include <Layout/LayoutAlgorithms.h>
#include <Shapes/Cuboid.h>
#include <Util/ApplicationConfig.h>
#include "Manager/ResourceManager.h"

static const float DEFAULT_MODULE_SECTOR_HEIGHT = 0.2f;
static const float DEFAULT_BUILDING_SPACING = 0.5f;

/*void Module::forEachBuilding( std::function<void( Building& b )> func )
{
	for ( auto& b : interfaceBuildingNodes ) {
		func( *b );
	}
	for ( auto& b : functionBuildingNodes ) {
		func( *b );
	}
	for ( auto& b : variableBuildingNodes ) {
		func( *b );
	}
	for ( auto& b : otherBuildingNodes ) {
		func( *b );
	}
}*/

City::Module::Module()
{	
	interfaceBuildingPAT = new osg::PositionAttitudeTransform();
	functionBuildingPAT = new osg::PositionAttitudeTransform();
	variableBuildingPAT = new osg::PositionAttitudeTransform();
	otherBuildingPAT = new osg::PositionAttitudeTransform();
	addChild( interfaceBuildingPAT );
	addChild( functionBuildingPAT );
	addChild( variableBuildingPAT );
	addChild( otherBuildingPAT );
	getOrCreateStateSet()->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );
}

void City::Module::addInterfaceBuilding( osg::ref_ptr<City::Building> interfaceBuilding, osg::ref_ptr<Data::Node> interfaceNode )
{	
	interfaceBuildings << interfaceBuilding;
	interfaceNodes << interfaceNode;
}

void City::Module::addFunctionBuilding( osg::ref_ptr<City::Building> functionBuilding, osg::ref_ptr<Data::Node> functionNode )
{
	functionBuildings << functionBuilding;
	functionNodes << functionNode;
}

void City::Module::addVariableBuilding( osg::ref_ptr<City::Building> variableBuilding, osg::ref_ptr<Data::Node> variableNode )
{
	variableBuildings << variableBuilding;
	variableNodes << variableNode;
}

void City::Module::addOtherBuilding( osg::ref_ptr<City::Building> otherBuilding, osg::ref_ptr<Data::Node> otherNode )
{
	otherBuildings << otherBuilding;
	otherNodes << otherNode;
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

	// nacitanie konfiguracii z aplikacie
	auto config = Util::ApplicationConfig::get();
	const float RESIDENCE_SECTOR_HEIGHT = config->getFloatValue( "City.Residence.SectorPlaneHeight", DEFAULT_MODULE_SECTOR_HEIGHT );
	const float BUILDING_SPACING = config->getFloatValue( "City.Residence.BuildingSpacing", DEFAULT_BUILDING_SPACING );

	// vymazanie starej geometrie vsetkych regionov
	interfaceBuildingPAT->removeChildren( 0, interfaceBuildingPAT->getNumChildren() );
	functionBuildingPAT->removeChildren( 0, functionBuildingPAT->getNumChildren() );
	variableBuildingPAT->removeChildren( 0, variableBuildingPAT->getNumChildren() );
	otherBuildingPAT->removeChildren( 0, otherBuildingPAT->getNumChildren() );

	osg::BoundingBox functionRegion;
	QList<osg::Vec3> functionLayouts;
	Layout::LayoutAlgorithms::layoutInsideRegion( functionBuildings.empty() ? zeroBoudingBox : functionBuildings.first()->getBoundingBox(), functionBuildings.count(), RESIDENCE_SECTOR_HEIGHT, BUILDING_SPACING, &functionLayouts, &functionRegion );

	for ( int i = 0; i < functionBuildings.count(); ++i ) {
		std::cout << "adjusting building: " << i << "\n";
		auto& b = functionBuildings[i];
		b->setPosition( functionLayouts[i] );
		b->refresh();
		functionBuildingPAT->addChild( b );
	}
	functionBuildingPAT->addChild( new Shapes::Cuboid( functionRegion.xMax() - functionRegion.xMin(), RESIDENCE_SECTOR_HEIGHT, functionRegion.yMax() - functionRegion.yMin(), osg::Vec3( 0, 0, RESIDENCE_SECTOR_HEIGHT / 2 ) ) );


	/*osg::BoundingBox interfaceRegion = functionRegion;
	QList<Layout::ElementLayout> interfaceLayouts;
	Layout::LayoutAlgorithms::layoutAroundRegion( interfaceBuildings.empty() ? zeroBoudingBox : interfaceBuildings.first()->getBoundingBox(), interfaceBuildings.count(), functionRegion, BUILDING_SPACING, &interfaceLayouts, &interfaceRegion );

	for ( int i = 0; i <interfaceBuildings.count(); ++i ) {
		auto& b = interfaceBuildings[i];
		interfaceLayouts[i].position.z() += RESIDENCE_SECTOR_HEIGHT;
		b->setPosition( interfaceLayouts[i].position );
		b->setAttitude( osg::Quat( interfaceLayouts[i].yawRotation, osg::Vec3( 0.0f, 0.0f, 1.0f ) ) );
		b->refresh();
		interfaceBuildingPAT->addChild( b );
	}
	osg::BoundingBox interfacePlane( interfaceRegion.xMin(), interfaceRegion.yMin(), 0, interfaceRegion.xMax(), interfaceRegion.yMax(), RESIDENCE_SECTOR_HEIGHT );
	interfaceBuildingPAT->addChild( new Shapes::Cuboid( interfacePlane ) );
*/
	/*

	osg::BoundingBox attrRegion;
	QList<osg::Vec3> attrLayouts;
	Layout::LayoutAlgorithms::layoutInsideRegion( attributesBuildings.empty() ? zeroBoudingBox : attributesBuildings.first()->getBoundingBox(), attributesBuildings.count(), RESIDENCE_SECTOR_HEIGHT, BUILDING_SPACING, &attrLayouts, &attrRegion );
	for ( int i = 0; i < attributesBuildings.count(); ++i ) {
		auto& b = attributesBuildings[i];
		b->setPosition( attrLayouts[i] );
		b->refresh();
		attributesBuildingsNode->addChild( b );
	}
	attributesBuildingsNode->addChild( new Shapes::Cuboid( attrRegion.xMax() - attrRegion.xMin(), RESIDENCE_SECTOR_HEIGHT, attrRegion.yMax() - attrRegion.yMin(), osg::Vec3( 0, 0, RESIDENCE_SECTOR_HEIGHT / 2 ) ) );

	osg::BoundingBox getSetRegion = attrRegion;
	QList<Layout::ElementLayout> getSetLayouts;
	Layout::LayoutAlgorithms::layoutAroundRegion( gettersSettersBuildings.empty() ? zeroBoudingBox : gettersSettersBuildings.first()->getBoundingBox(), gettersSettersBuildings.count(), attrRegion, BUILDING_SPACING, &getSetLayouts, &getSetRegion );
	for ( int i = 0; i <gettersSettersBuildings.count(); ++i ) {
		auto& b = gettersSettersBuildings[i];
		getSetLayouts[i].position.z() += RESIDENCE_SECTOR_HEIGHT;
		b->setPosition( getSetLayouts[i].position );
		b->setAttitude( osg::Quat( getSetLayouts[i].yawRotation, osg::Vec3( 0.0f, 0.0f, 1.0f ) ) );
		b->refresh();
		gettersSettersBuildingsNode->addChild( b );
	}
	osg::BoundingBox getSetPlane( getSetRegion.xMin(), getSetRegion.yMin(), 0, getSetRegion.xMax(), getSetRegion.yMax(), RESIDENCE_SECTOR_HEIGHT );
	gettersSettersBuildingsNode->addChild( new Shapes::Cuboid( getSetPlane ) );

	osg::BoundingBox internalRegion = getSetRegion;
	QList<Layout::ElementLayout> internalLayouts;
	Layout::LayoutAlgorithms::layoutAroundRegion( internalMethodsBuildings.empty() ? zeroBoudingBox : internalMethodsBuildings.first()->getBoundingBox(), internalMethodsBuildings.count(), getSetRegion, BUILDING_SPACING, &internalLayouts, &internalRegion );
	for ( int i = 0; i < internalMethodsBuildings.count(); ++i ) {
		auto& b = internalMethodsBuildings[i];
		internalLayouts[i].position.z() += RESIDENCE_SECTOR_HEIGHT;
		b->setPosition( internalLayouts[i].position );
		b->setAttitude( osg::Quat( internalLayouts[i].yawRotation, osg::Vec3( 0.0f, 0.0f, 1.0f ) ) );
		b->refresh();
		internalMethodsBuildingsNode->addChild( b );
	}
	osg::BoundingBox internalPlane( internalRegion.xMin(), internalRegion.yMin(), 0, internalRegion.xMax(), internalRegion.yMax(), RESIDENCE_SECTOR_HEIGHT );
	internalMethodsBuildingsNode->addChild( new Shapes::Cuboid( internalPlane ) );

	osg::BoundingBox interfaceRegion = internalRegion;
	QList<Layout::ElementLayout> interfaceLayouts;
	Layout::LayoutAlgorithms::layoutAroundRegion( interfaceMethodsBuildings.empty() ? zeroBoudingBox : interfaceMethodsBuildings.first()->getBoundingBox(), interfaceMethodsBuildings.count(), internalRegion, BUILDING_SPACING, &interfaceLayouts, &interfaceRegion );
	for ( int i = 0; i < interfaceMethodsBuildings.count(); ++i ) {
		auto& b = interfaceMethodsBuildings[i];
		interfaceLayouts[i].position.z() += RESIDENCE_SECTOR_HEIGHT;
		b->setPosition( interfaceLayouts[i].position );
		b->setAttitude( osg::Quat( interfaceLayouts[i].yawRotation, osg::Vec3( 0.0f, 0.0f, 1.0f ) ) );
		b->refresh();
		interfaceMethodsBuildingsNode->addChild( b );
	}
	osg::BoundingBox interfacePlane( interfaceRegion.xMin(), interfaceRegion.yMin(), 0, interfaceRegion.xMax(), interfaceRegion.yMax(), RESIDENCE_SECTOR_HEIGHT );
	interfaceMethodsBuildingsNode->addChild( new Shapes::Cuboid( interfacePlane ) );

	const auto offset = interfaceRegion.center();
	float residenceSectorOffset = 0;
	interfaceMethodsBuildingsNode->setPosition( osg::Vec3( -offset.x(), -offset.y(), residenceSectorOffset ) );
	residenceSectorOffset += RESIDENCE_SECTOR_HEIGHT;
	internalMethodsBuildingsNode->setPosition( osg::Vec3( -offset.x(), -offset.y(), residenceSectorOffset ) );
	residenceSectorOffset += RESIDENCE_SECTOR_HEIGHT;
	gettersSettersBuildingsNode->setPosition( osg::Vec3( -offset.x(), -offset.y(), residenceSectorOffset ) );
	residenceSectorOffset += RESIDENCE_SECTOR_HEIGHT;
	attributesBuildingsNode->setPosition( osg::Vec3( -offset.x(), -offset.y(), residenceSectorOffset ) );
	residenceSectorOffset += RESIDENCE_SECTOR_HEIGHT;
	*/
}

void City::Module::updateNodesPosition()
{
	for ( int i = 0; i < interfaceBuildings.count(); ++i ) {
		auto& b = interfaceBuildings[i];
		auto& node = interfaceNodes[i];
		node->setTargetPosition(b->getPosition());
		//node->setRestrictedTargetPosition(b->getPosition());
		//node->setRestrictedTargetPosition(b->getPosition() + moduleNode->getTargetPosition());
	}

	for ( int i = 0; i < functionBuildings.count(); ++i ) {
		auto& b = functionBuildings[i];
		auto& node = functionNodes[i];
		node->setTargetPosition(b->getPosition());
		//node->setRestrictedTargetPosition(b->getPosition());
		//node->setRestrictedTargetPosition(b->getPosition() + moduleNode->getTargetPosition());
	}

	for ( int i = 0; i < variableBuildings.count(); ++i ) {
		auto& b = variableBuildings[i];
		auto& node = variableNodes[i];
		node->setTargetPosition(b->getPosition());
		//node->setRestrictedTargetPosition(b->getPosition());
		//node->setRestrictedTargetPosition(b->getPosition() + moduleNode->getTargetPosition());
	}

	for ( int i = 0; i < otherBuildings.count(); ++i ) {
		auto& b = otherBuildings[i];
		auto& node = otherNodes[i];
		node->setTargetPosition(b->getPosition());
		//node->setRestrictedTargetPosition(b->getPosition());
		//node->setRestrictedTargetPosition(b->getPosition() + moduleNode->getTargetPosition());
	}
}
