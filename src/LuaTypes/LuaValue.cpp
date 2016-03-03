#include "LuaTypes/LuaValue.h"

Lua::LuaValue::LuaValue(){
}

Lua::LuaValue::LuaValue( Diluculum::LuaValue val ) :
	val( val ){
}

Lua::LuaValue::LuaValue( const Lua::LuaValue& obj) :
	val(obj.getValue()){
}

Lua::LuaValue::~LuaValue(){
}

Diluculum::LuaValue Lua::LuaValue::getValue() const {
	return val;
}

void Lua::LuaValue::setValue( Diluculum::LuaValue val ){
	this->val = val;
}
