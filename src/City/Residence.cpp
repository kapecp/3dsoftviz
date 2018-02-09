#include "City/Residence.h"
#include <Layout/LayoutAlgorithms.h>
#include <Shapes/Cuboid.h>
#include <Util/ApplicationConfig.h>

namespace City {
static const float DEFAULT_RESIDENCE_SECTOR_HEIGHT = 0.2f;
static const float DEFAULT_BUILDING_SPACING2 = 0.5f;

void Residence::forEachBuilding( std::function<void( Building& b )> func )
{
	for ( auto& b : attributesBuildings ) {
		func( *b );
	}
	for ( auto& b : gettersSettersBuildings ) {
		func( *b );
	}
	for ( auto& b : internalMethodsBuildings ) {
		func( *b );
	}
	for ( auto& b : interfaceMethodsBuildings ) {
		func( *b );
	}
}

Residence::Residence()
{
	attributesBuildingsNode = new osg::PositionAttitudeTransform();
	gettersSettersBuildingsNode = new osg::PositionAttitudeTransform();
	internalMethodsBuildingsNode = new osg::PositionAttitudeTransform();
	interfaceMethodsBuildingsNode = new osg::PositionAttitudeTransform();
	addChild( attributesBuildingsNode );
	addChild( gettersSettersBuildingsNode );
	addChild( internalMethodsBuildingsNode );
	addChild( interfaceMethodsBuildingsNode );
	getOrCreateStateSet()->setMode( GL_RESCALE_NORMAL, osg::StateAttribute::ON );
}

void Residence::addAttributeBuilding( Building* attrBuilding )
{
	attributesBuildings << attrBuilding;
}

void Residence::addGetterSeterBuilding( Building* getSetBuilding )
{
	gettersSettersBuildings << getSetBuilding;
}

void Residence::addInternalBuilding( Building* internalBuilding )
{
	internalMethodsBuildings << internalBuilding;
}

void Residence::addInterfaceBuilding( Building* interfaceBuilding )
{
	interfaceMethodsBuildings << interfaceBuilding;
}

void Residence::showLabels( bool state )
{
	forEachBuilding( [state]( Building& b ) {
		b.showLabel( state );
	} );
}

void Residence::selectAll( bool state )
{
	if ( !state ) {
		forEachBuilding( [state]( Building& b ) {
			b.select( state );
		} );
	}
}

void Residence::refresh()
{
	static const osg::BoundingBox zeroBoudingBox( 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f );

	// nacitanie konfiguracii z aplikacie
	auto config = Util::ApplicationConfig::get();
	const float RESIDENCE_SECTOR_HEIGHT = config->getFloatValue( "City.Residence.SectorPlaneHeight", DEFAULT_RESIDENCE_SECTOR_HEIGHT );
	const float BUILDING_SPACING = config->getFloatValue( "City.Residence.BuildingSpacing", DEFAULT_BUILDING_SPACING2 );

	// vymazanie starej geometrie vsetkych regionov
	attributesBuildingsNode->removeChildren( 0, attributesBuildingsNode->getNumChildren() );
	gettersSettersBuildingsNode->removeChildren( 0, gettersSettersBuildingsNode->getNumChildren() );
	internalMethodsBuildingsNode->removeChildren( 0, internalMethodsBuildingsNode->getNumChildren() );
	interfaceMethodsBuildingsNode->removeChildren( 0, interfaceMethodsBuildingsNode->getNumChildren() );

	osg::BoundingBox attrRegion;
	QList<osg::Vec3> attrLayouts;
	Layout::LayoutAlgorithms::layoutInsideRegion( attributesBuildings.empty() ? zeroBoudingBox : attributesBuildings.first()->getBoundingBox(), static_cast<uint>( attributesBuildings.count() ), RESIDENCE_SECTOR_HEIGHT, BUILDING_SPACING, &attrLayouts, &attrRegion );
	for ( int i = 0; i < attributesBuildings.count(); ++i ) {
		auto& b = attributesBuildings[i];
		b->setPosition( attrLayouts[i] );
		b->refresh();
		attributesBuildingsNode->addChild( b );
	}
	attributesBuildingsNode->addChild( new Shapes::Cuboid( attrRegion.xMax() - attrRegion.xMin(), RESIDENCE_SECTOR_HEIGHT, attrRegion.yMax() - attrRegion.yMin(), osg::Vec3( 0, 0, RESIDENCE_SECTOR_HEIGHT / 2 ) ) );

	osg::BoundingBox getSetRegion = attrRegion;
	QList<Layout::ElementLayout> getSetLayouts;
	Layout::LayoutAlgorithms::layoutAroundRegion( gettersSettersBuildings.empty() ? zeroBoudingBox : gettersSettersBuildings.first()->getBoundingBox(), static_cast<uint>( gettersSettersBuildings.count() ), attrRegion, BUILDING_SPACING, &getSetLayouts, &getSetRegion );
	for ( int i = 0; i <gettersSettersBuildings.count(); ++i ) {
		auto& b = gettersSettersBuildings[i];
		getSetLayouts[i].position.z() += RESIDENCE_SECTOR_HEIGHT;
		b->setPosition( getSetLayouts[i].position );
		b->setAttitude( osg::Quat( static_cast<double>( getSetLayouts[i].yawRotation ), osg::Vec3( 0.0f, 0.0f, 1.0f ) ) );
		b->refresh();
		gettersSettersBuildingsNode->addChild( b );
	}
	osg::BoundingBox getSetPlane( getSetRegion.xMin(), getSetRegion.yMin(), 0, getSetRegion.xMax(), getSetRegion.yMax(), RESIDENCE_SECTOR_HEIGHT );
	gettersSettersBuildingsNode->addChild( new Shapes::Cuboid( getSetPlane ) );

	osg::BoundingBox internalRegion = getSetRegion;
	QList<Layout::ElementLayout> internalLayouts;
	Layout::LayoutAlgorithms::layoutAroundRegion( internalMethodsBuildings.empty() ? zeroBoudingBox : internalMethodsBuildings.first()->getBoundingBox(), static_cast<uint>( internalMethodsBuildings.count() ), getSetRegion, BUILDING_SPACING, &internalLayouts, &internalRegion );
	for ( int i = 0; i < internalMethodsBuildings.count(); ++i ) {
		auto& b = internalMethodsBuildings[i];
		internalLayouts[i].position.z() += RESIDENCE_SECTOR_HEIGHT;
		b->setPosition( internalLayouts[i].position );
		b->setAttitude( osg::Quat( static_cast<double>( internalLayouts[i].yawRotation ), osg::Vec3( 0.0f, 0.0f, 1.0f ) ) );
		b->refresh();
		internalMethodsBuildingsNode->addChild( b );
	}
	osg::BoundingBox internalPlane( internalRegion.xMin(), internalRegion.yMin(), 0, internalRegion.xMax(), internalRegion.yMax(), RESIDENCE_SECTOR_HEIGHT );
	internalMethodsBuildingsNode->addChild( new Shapes::Cuboid( internalPlane ) );

	osg::BoundingBox interfaceRegion = internalRegion;
	QList<Layout::ElementLayout> interfaceLayouts;
	Layout::LayoutAlgorithms::layoutAroundRegion( interfaceMethodsBuildings.empty() ? zeroBoudingBox : interfaceMethodsBuildings.first()->getBoundingBox(), static_cast<uint>( interfaceMethodsBuildings.count() ), internalRegion, BUILDING_SPACING, &interfaceLayouts, &interfaceRegion );
	for ( int i = 0; i < interfaceMethodsBuildings.count(); ++i ) {
		auto& b = interfaceMethodsBuildings[i];
		interfaceLayouts[i].position.z() += RESIDENCE_SECTOR_HEIGHT;
		b->setPosition( interfaceLayouts[i].position );
		b->setAttitude( osg::Quat( static_cast<double>( interfaceLayouts[i].yawRotation ), osg::Vec3( 0.0f, 0.0f, 1.0f ) ) );
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

}
}
