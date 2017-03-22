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

float Lua::LuaGraphObject::getFloatParam( std::string tableName, std::string key, float defaultValue )
{
	if ( params.getValue().type() == 0 ) {
		return defaultValue;
	}
	if ( params.getValue()[tableName].typeName() != "table" ) {
		return defaultValue;
	}
	Lua::LuaValue tableFromParams;
	tableFromParams.setValue(params.getValue()[tableName].asTable());

	if ( tableFromParams.getValue().type() == 0 ) {
		return defaultValue;
	}
	if ( tableFromParams.getValue()[key].typeName() != "number" ) {
		return defaultValue;
	}
	return static_cast<float>( tableFromParams.getValue()[key].asNumber() );

}


