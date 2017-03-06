#include "LuaTypes/LuaState.h"

Lua::LuaState::LuaState()
{
	val = new Diluculum::LuaState;
}

Lua::LuaState::LuaState( Diluculum::LuaState* val ) :
	val( val )
{
}

Lua::LuaState::~LuaState()
{
}

Diluculum::LuaState* Lua::LuaState::getValue() const
{
	return val;
}

void Lua::LuaState::setValue( Diluculum::LuaState* val )
{
	this->val = val;
}
