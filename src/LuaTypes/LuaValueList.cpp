#include "LuaTypes/LuaValueList.h"

Lua::LuaValueList::LuaValueList(){
}

Lua::LuaValueList::LuaValueList( Diluculum::LuaValueList val ) :
	val( val ){
}

Lua::LuaValueList::LuaValueList( const Lua::LuaValueList& obj) :
	val ( obj.getValue() ){
}

Lua::LuaValueList::~LuaValueList(){
}

Diluculum::LuaValueList Lua::LuaValueList::getValue() const {
	return val;
}

void Lua::LuaValueList::setValue( Diluculum::LuaValueList val ){
	this->val = val;
}

void Lua::LuaValueList::push_back( const Diluculum::LuaValue& val ){
	this->val.push_back( val );
}

Diluculum::LuaValue& Lua::LuaValueList::operator []( int index ){
	 return val[index];
}
