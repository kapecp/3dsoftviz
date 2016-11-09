#include "City/Building.h"
#include <Shapes/QuadPyramide.h>
#include <Util/ApplicationConfig.h>
#include <Data/OsgNode.h>
#include <Manager/ResourceManager.h>
#include <osgText/FadeText>

namespace City {
static float DEFAULT_BUILDING_DEFAULT_BASE_SIZE = 1.0f;
static float DEFAULT_BUILDING_DEFAULT_ROOF_HEIGHT = 0.3f;
static float DEFAULT_BUILDING_CAPTION_CHARACTER_SIZE = 3.0f;
static float DEFAULT_BUILDING_CAPTION_OFFSET = 0.3f;

Building::Building( const QString& name, const QString& info, const QList<Floor*>& inputFloors )
{
	const float BUILDING_DEFAULT_BASE_SIZE = Util::ApplicationConfig::get()->getFloatValue( "City.Building.DefaultBaseSize", DEFAULT_BUILDING_DEFAULT_BASE_SIZE );

	setLabel( name );
	this->info = info;
	for ( auto f : inputFloors ) {
		floors << f;
	}
	this->triangleRoof = false;
	this->lieOnGround = true;
	this->labelVisible = false;
	if ( floors.empty() ) {
		auto f = new Floor();
		f->setDivideBorder( false );
		floors << f;
	}
	minBuildingHeight = Floor::getFloorMinHeight() * static_cast<float>( floors.count() );
	buildingHeight = minBuildingHeight;
	for ( const auto& f : floors ) {
		f->setBaseSize( BUILDING_DEFAULT_BASE_SIZE );
	}
}

void Building::setHeight( float height )
{
	buildingHeight = 0.0f;
	const float floorHeight = height / static_cast<float>( floors.count() );
	for ( auto& f : floors ) {
		f->setFloorHeight( floorHeight );
		buildingHeight += f->getFloorHeight();
	}
}

float Building::getHeight( bool includeRoof ) const
{
	const float BUILDING_DEFAULT_ROOF_HEIGHT = Util::ApplicationConfig::get()->getFloatValue( "City.Building.DefaultRoofHeight", DEFAULT_BUILDING_DEFAULT_ROOF_HEIGHT );

	return buildingHeight + ( includeRoof && triangleRoof ? BUILDING_DEFAULT_ROOF_HEIGHT : 0.0f );
}

void Building::setBaseSize( float size )
{
	for ( auto& f : floors ) {
		f->setBaseSize( size );
	}
}

float Building::getBaseSize() const
{
	return floors.first()->getBaseSize();
}

void Building::setTriangleRoof( bool state )
{
	triangleRoof = state;
}

void Building::setLieOnGround( bool state )
{
	lieOnGround = state;
}

void Building::setLabel( const QString& name )
{
	if ( label.valid() && labelVisible ) {
		removeChild( label );
	}

	this->name = name;
	label.release();
	if ( !name.isEmpty() ) {
		auto config = Util::ApplicationConfig::get();
		const float BUILDING_CAPTION_CHARACTER_SIZE = config->getFloatValue( "City.Building.CaptionCharacterSize", DEFAULT_BUILDING_CAPTION_CHARACTER_SIZE );

		label = new osg::Geode();
		auto text = new osgText::FadeText();
		text->setText( name.toStdString() );
		text->setCharacterSize( BUILDING_CAPTION_CHARACTER_SIZE );
		text->setAutoRotateToScreen( true );
		label->addDrawable( text );
		if ( labelVisible ) {
			addChild( label );
		}
	}
}

void Building::showLabel( bool state )
{
	if ( label.valid() ) {
		if ( labelVisible && !state ) {
			removeChild( label );
		}
		else if ( !labelVisible && state ) {
			addChild( label );
		}
	}
	labelVisible = state;
}

const QString& Building::getInfo() const
{
	return info;
}

osg::BoundingBox Building::getBoundingBox() const
{
	const float BUILDING_BASE_SIZE_HALF = getBaseSize() / 2;
	return osg::BoundingBox( -BUILDING_BASE_SIZE_HALF, -BUILDING_BASE_SIZE_HALF, 0, BUILDING_BASE_SIZE_HALF, BUILDING_BASE_SIZE_HALF, getHeight() );
}

void Building::refresh()
{
	// nacitanie konfiguracii z aplikacie
	auto config = Util::ApplicationConfig::get();
	const float BUILDING_DEFAULT_ROOF_HEIGHT = config->getFloatValue( "City.Building.DefaultRoofHeight", DEFAULT_BUILDING_DEFAULT_ROOF_HEIGHT );
	const float BUILDING_CAPTION_OFFSET = config->getFloatValue( "City.Building.CaptionOffset", DEFAULT_BUILDING_CAPTION_OFFSET );

	// vymazanie starej geometrie
	removeChildren( 0, getNumChildren() );

	osg::Vec3 pos( 0.0f, 0.0f, lieOnGround ? 0.0f : -getHeight() / 2.0f );
	for ( auto& f : floors ) { // generovanie geometrie poschodi
		f->setPosition( pos );
		pos.z() += f->getFloorHeight();
		f->refresh();
		addChild( f );
	}
	if ( triangleRoof ) { // ma mat strechu
		auto roof = new osg::PositionAttitudeTransform();
		roof->setPosition( pos );
		roof->addChild( new Shapes::QuadPyramide( getBaseSize(), getBaseSize(), BUILDING_DEFAULT_ROOF_HEIGHT ) );
		addChild( roof );
	}

	if ( label.valid() ) { // ma nastaveny label
		static_cast<osgText::TextBase*>( label->asGeode()->getDrawable( 0 ) )->setPosition( osg::Vec3( 0.0f, 0.0f, getHeight( true ) + BUILDING_CAPTION_OFFSET ) );
		if ( labelVisible ) {
			addChild( label );
		}
	}
}

void Building::select( bool selected )
{
	if ( selected && !lastMaterial.valid() ) {
		lastMaterial = static_cast<osg::Material*>( getOrCreateStateSet()->getAttribute( osg::StateAttribute::Type::MATERIAL ) );
		auto color = lastMaterial->getDiffuse( osg::Material::Face::FRONT_AND_BACK );
		getOrCreateStateSet()->setAttribute( Manager::ResourceManager::getInstance()->getMaterial( osg::Vec3( color.r() + 0.4f, color.g() + 0.4f, color.b() + 0.4f ) ), osg::StateAttribute::ON );
	}
	else {
		if ( lastMaterial.valid() ) {
			getOrCreateStateSet()->setAttribute( lastMaterial );
		}
		lastMaterial.release();
	}
}

float Building::getMinHeight() const
{
	return minBuildingHeight;
}
}
