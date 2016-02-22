#include "LuaTypes/LuaValueMap.h"

Lua::LuaValueMap::LuaValueMap(){
}

Lua::LuaValueMap::LuaValueMap(Diluculum::LuaValueMap val) :
	val( val ){
}

Lua::LuaValueMap::LuaValueMap( const Lua::LuaValueMap& obj) :
	val(obj.getValue()){
}

Lua::LuaValueMap::~LuaValueMap(){
}

Diluculum::LuaValueMap Lua::LuaValueMap::getValue() const {
	return val;
}

void Lua::LuaValueMap::setValue( Diluculum::LuaValueMap val ){
	this->val = val;
}

Diluculum::LuaValueMap::iterator Lua::LuaValueMap::begin(){
	return val.begin();
}

Diluculum::LuaValueMap::iterator Lua::LuaValueMap::end(){
	return val.end();
}

Diluculum::LuaValueMap::iterator Lua::LuaValueMap::find( const std::string& str ){
	return this->val.find( str );
}

void Lua::LuaValueMap::insert( std::pair<Diluculum::LuaValue, Diluculum::LuaValue> val ){
	this->val.insert( val );
}

void Lua::LuaValueMap::insertPair( long val_one, Diluculum::LuaValue val_two ){
	val.insert( std::pair<Diluculum::LuaValue, Diluculum::LuaValue>( val_one, val_two ) );
}
