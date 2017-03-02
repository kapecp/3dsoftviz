#include "ModuleGraph/Module.h"

#include <Shapes/Cuboid.h>
#include <Util/ApplicationConfig.h>

static float DEFAULT_MODULE_LENGTH = 2;
static float DEFAULT_MODULE_WIDTH = 4;
static float DEFAULT_MODULE_HEIGTH = 6;

ModuleGraph::Module::Module()
{
	length = Util::ApplicationConfig::get()->getFloatValue( "GraphModule.Module.ModuleLength", DEFAULT_MODULE_LENGTH );
	width = Util::ApplicationConfig::get()->getFloatValue( "GraphModule.Module.ModuleWidth", DEFAULT_MODULE_WIDTH );
	heigth = Util::ApplicationConfig::get()->getFloatValue( "GraphModule.Module.ModuleHeigth", DEFAULT_MODULE_HEIGTH );

}

void ModuleGraph::Module::refresh()
{
	removeChildren( 0, getNumChildren() );

	addChild( new Shapes::Cuboid( length, heigth, width, osg::Vec3( 0.0f, 0.0f, 0.0f ) ) );
}

float ModuleGraph::Module::getLength()
{
	return length;
}
float ModuleGraph::Module::getWidth()
{
	return width;
}

float ModuleGraph::Module::getHeigth()
{
	return heigth;
}


void ModuleGraph::Module::setLength( float newLength )
{
	length = newLength;
}
void ModuleGraph::Module::setWidth( float newWidth )
{
	width = newWidth;
}
void ModuleGraph::Module::setHeigth( float newHeigth )
{
	heigth = newHeigth;
}

void ModuleGraph::Module::setModuleSize( float newLength, float newWidth, float newHeigth )
{
	length = newLength;
	width = newWidth;
	heigth = newHeigth;

}
