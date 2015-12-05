#ifndef LUAVALUEMAP_H
#define LUAVALUEMAP_H

#include "Diluculum/Types.hpp"

namespace Lua {

class LuaValueMap
{
public:
	LuaValueMap();

	LuaValueMap( Diluculum::LuaValueMap val );

	LuaValueMap( const Lua::LuaValueMap& obj);

	~LuaValueMap();

	Diluculum::LuaValueMap getValue() const;

	void setValue( Diluculum::LuaValueMap val );

	Diluculum::LuaValueMap::iterator begin();

	Diluculum::LuaValueMap::iterator end();

	Diluculum::LuaValueMap::iterator find( const std::string& str );

	void insert( std::pair<Diluculum::LuaValue, Diluculum::LuaValue> val );

	void insertPair( long val_one, Diluculum::LuaValue val_two );

private:
	Diluculum::LuaValueMap val;

};
}

#endif // LUAVALUEMAP_H
