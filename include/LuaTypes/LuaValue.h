#ifndef LUAVALUE_H
#define LUAVALUE_H

#include "Diluculum/Types.hpp"

namespace Lua {

class LuaValue
{
public:
	LuaValue();

	LuaValue( Diluculum::LuaValue val );

	LuaValue( const Lua::LuaValue& obj);

	~LuaValue();

	Diluculum::LuaValue getValue() const;

	void setValue( Diluculum::LuaValue val );

private:
	Diluculum::LuaValue val;

};
}

#endif // LUAVALUE_H
