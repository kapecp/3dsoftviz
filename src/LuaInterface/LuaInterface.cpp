#include "LuaInterface/LuaInterface.h"

// lua development
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// include diluculum header files
#include "Diluculum/LuaState.hpp"
#include "Diluculum/Types.hpp"

#include <sstream>
#include <string>

Lua::LuaInterface* Lua::LuaInterface::instance;

Lua::LuaInterface* Lua::LuaInterface::getInstance()
{
	if ( instance == NULL ) {
		instance = new LuaInterface;
	}
	return instance;
}

Lua::LuaInterface::~LuaInterface()
{
	delete &luaState;
	if ( instance != NULL ) {
		instance = NULL;
	}
}

Lua::LuaInterface::LuaInterface()
{
	luaState = Lua::LuaState();
}

Lua::LuaState Lua::LuaInterface::getLuaState() const
{
	return luaState;
}


void Lua::LuaInterface::executeFile( QString path )
{
	std::stringstream sstm;
	sstm << "../share/3dsoftviz/scripts/app/" << path.toStdString();
	luaState.getValue()->doFile( sstm.str() );
}

QString Lua::LuaInterface::getString( QString name )
{
	Diluculum::LuaVariable var = ( *luaState.getValue() )[name.toStdString()];
	return QString::fromStdString( var.value().asString() );
}

QString Lua::LuaInterface::getString( unsigned int length, QString args[] )
{
	Diluculum::LuaVariable var = ( *luaState.getValue() )[args[0].toStdString()];
	for ( unsigned int i = 1; i < length; i++ ) {
		var = var[args[i].toStdString()];
	}
	return QString::fromStdString( var.value().asString() );
}

int Lua::LuaInterface::getInt( QString name )
{
	Diluculum::LuaVariable var = ( *luaState.getValue() )[name.toStdString()];
	return static_cast<int>( var.value().asInteger() );
}

int Lua::LuaInterface::getInt( unsigned int length,QString args[] )
{
	Diluculum::LuaVariable var = ( *luaState.getValue() )[args[0].toStdString()];
	for ( unsigned int i = 1; i < length; i++ ) {
		var = var[args[i].toStdString()];
	}
	return static_cast<int>( var.value().asInteger() );
}

double Lua::LuaInterface::getDouble( QString name )
{
	Diluculum::LuaVariable var = ( *luaState.getValue() )[name.toStdString()];
	return var.value().asNumber();
}

double Lua::LuaInterface::getDouble( unsigned int length, QString args[] )
{
	Diluculum::LuaVariable var = ( *luaState.getValue() )[args[0].toStdString()];
	for ( unsigned int i = 1; i < length; i++ ) {
		var = var[args[i].toStdString()];
	}
	return var.value().asNumber();
}

Lua::LuaValueList Lua::LuaInterface::callFunction( QString funcName, Lua::LuaValueList params )
{
	return Lua::LuaValueList( ( *luaState.getValue() )[funcName.toStdString()]( params.getValue() ) );
}

Lua::LuaValueList Lua::LuaInterface::callFunction( unsigned int length, QString args[], Lua::LuaValueList params )
{
	Diluculum::LuaVariable var = ( *luaState.getValue() )[args[0].toStdString()];
	for ( unsigned int i = 1; i < length; i++ ) {
		var = var[args[i].toStdString()];
	}
	return Lua::LuaValueList( var( params.getValue() ) );
}

Lua::LuaValueList Lua::LuaInterface::doString( const std::string& what )
{
	return Lua::LuaValueList( luaState.getValue()->Diluculum::LuaState::doString( what ) );
}
