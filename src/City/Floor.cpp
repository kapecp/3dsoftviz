#include "City/Floor.h"
#include <Manager/ResourceManager.h>
#include <Shapes/Cuboid.h>
#include <Util/ApplicationConfig.h>

namespace Clustering {
static float DEFAULT_FLOOR_MIN_HEIGHT = 0.15f;
static float DEFAULT_FLOOR_DIVIDE_BORDER_STICK_UP = 0.1f;
static float DEFAULT_FLOOR_DIVIDE_BORDER_HEIGHT = 0.05f;

Floor::Floor()
{
	const float FLOOR_MIN_HEIGHT = Util::ApplicationConfig::get()->getFloatValue( "City.Floor.MinHeight", DEFAULT_FLOOR_MIN_HEIGHT );

	this->divideBorder = true;
	baseSize = 0;
	floorHeight = FLOOR_MIN_HEIGHT;
}

void Floor::setBaseSize( float size )
{
	baseSize = size;
}

float Floor::getBaseSize() const
{
	return baseSize;
}

void Floor::setFloorHeight( float height )
{
	const float FLOOR_MIN_HEIGHT = Util::ApplicationConfig::get()->getFloatValue( "City.Floor.MinHeight", DEFAULT_FLOOR_MIN_HEIGHT );
	floorHeight = std::max( height, FLOOR_MIN_HEIGHT );
}

float Floor::getFloorHeight() const
{
	return floorHeight;
}

void Floor::setDivideBorder( bool state )
{
	divideBorder = state;
}

void Floor::refresh()
{
	// nacitanie konfiguracii z aplikacie
	auto config = Util::ApplicationConfig::get();
	const float FLOOR_DIVIDE_BORDER_STICK_UP = config->getFloatValue( "City.Floor.DivideBorderStickUp", DEFAULT_FLOOR_DIVIDE_BORDER_STICK_UP );
	const float FLOOR_DIVIDE_BORDER_HEIGHT = config->getFloatValue( "City.Floor.DivideBorderHeight", DEFAULT_FLOOR_DIVIDE_BORDER_HEIGHT );

	// vypocet velkosti
	const float realFloorDivideBorderHeight = divideBorder ? FLOOR_DIVIDE_BORDER_HEIGHT : 0.0f;
	const float floorDivideBorderBaseSize = baseSize + FLOOR_DIVIDE_BORDER_STICK_UP;
	const float floorDivideBorderGroundOffset = realFloorDivideBorderHeight / 2;
	const float floorWallHeight = floorHeight - realFloorDivideBorderHeight;
	const float floorGroundOffset = realFloorDivideBorderHeight + floorWallHeight / 2;

	// vymazanie starej geometrie
	removeChildren( 0, getNumChildren() );

	if ( divideBorder ) { // ak ma mat rozdelovaci okraj poschodia
		addChild( new Cuboid( floorDivideBorderBaseSize, FLOOR_DIVIDE_BORDER_HEIGHT, floorDivideBorderBaseSize, osg::Vec3( 0.0f, 0.0f, floorDivideBorderGroundOffset ) ) );
	}
	addChild( new Cuboid( baseSize, floorWallHeight, baseSize, osg::Vec3( 0.0f, 0.0f, floorGroundOffset ) ) );
}

float Floor::getFloorMinHeight()
{
	const float FLOOR_MIN_HEIGHT = Util::ApplicationConfig::get()->getFloatValue( "City.Floor.MinHeight", DEFAULT_FLOOR_MIN_HEIGHT );
	return FLOOR_MIN_HEIGHT;
}
}
