#include "LuaGraph/LuaGraphObject.h"

#include <string>

qlonglong Lua::LuaGraphObject::getId() const
{
	return id;
}

void Lua::LuaGraphObject::setId( qlonglong value )
{
	id = value;
}

QString Lua::LuaGraphObject::getLabel() const
{
	return label;
}

void Lua::LuaGraphObject::setLabel( const QString& value )
{
	label = value;
}
Lua::LuaValue Lua::LuaGraphObject::getParams() const
{
	return params;
}

void Lua::LuaGraphObject::setParams( const Lua::LuaValue& value )
{
	params = value;
}

float Lua::LuaGraphObject::getFloatParam( std::string name, float defaultValue )
{
	if ( params.getValue().type() == 0 ) {
		return defaultValue;
	}
	if ( params.getValue()[name].typeName() != "number" ) {
		return defaultValue;
	}
	return static_cast<float>( params.getValue()[name].asNumber() );
}


