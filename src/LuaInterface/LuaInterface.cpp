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
	delete luaState;
	if ( instance != NULL ) {
		instance = NULL;
	}
}

Lua::LuaInterface::LuaInterface()
{
	luaState = new Diluculum::LuaState;
}

Diluculum::LuaState* Lua::LuaInterface::getLuaState() const
{
	return luaState;
}


void Lua::LuaInterface::executeFile( QString path )
{
	std::stringstream sstm;
	sstm << "../share/3dsoftviz/scripts/app/" << path.toStdString();
	luaState->doFile( sstm.str() );
}

QString Lua::LuaInterface::getString( QString name )
{
	Diluculum::LuaVariable var = ( *luaState )[name.toStdString()];
	return QString::fromStdString( var.value().asString() );
}

QString Lua::LuaInterface::getString( unsigned int length, QString args[] )
{
	Diluculum::LuaVariable var = ( *luaState )[args[0].toStdString()];
	for ( unsigned int i = 1; i < length; i++ ) {
		var = var[args[i].toStdString()];
	}
	return QString::fromStdString( var.value().asString() );
}

int Lua::LuaInterface::getInt( QString name )
{
	Diluculum::LuaVariable var = ( *luaState )[name.toStdString()];
	return static_cast<int>( var.value().asInteger() );
}

int Lua::LuaInterface::getInt( unsigned int length,QString args[] )
{
	Diluculum::LuaVariable var = ( *luaState )[args[0].toStdString()];
	for ( unsigned int i = 1; i < length; i++ ) {
		var = var[args[i].toStdString()];
	}
	return static_cast<int>( var.value().asInteger() );
}

double Lua::LuaInterface::getDouble( QString name )
{
	Diluculum::LuaVariable var = ( *luaState )[name.toStdString()];
	return var.value().asNumber();
}

double Lua::LuaInterface::getDouble( unsigned int length, QString args[] )
{
	Diluculum::LuaVariable var = ( *luaState )[args[0].toStdString()];
	for ( unsigned int i = 1; i < length; i++ ) {
		var = var[args[i].toStdString()];
	}
	return var.value().asNumber();
}

Diluculum::LuaValueList Lua::LuaInterface::callFunction( QString funcName, Diluculum::LuaValueList params )
{
	return ( *luaState )[funcName.toStdString()]( params );
}

Diluculum::LuaValueList Lua::LuaInterface::callFunction( unsigned int length, QString args[], Diluculum::LuaValueList params )
{
	Diluculum::LuaVariable var = ( *luaState )[args[0].toStdString()];
	for ( unsigned int i = 1; i < length; i++ ) {
		var = var[args[i].toStdString()];
	}
	return var( params );
}

Diluculum::LuaValueList Lua::LuaInterface::doString( const std::string& what )
{
	return luaState->Diluculum::LuaState::doString( what );
}
