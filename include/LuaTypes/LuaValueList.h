#ifndef LUAVALUELIST_H
#define LUAVALUELIST_H

#include "Diluculum/Types.hpp"

namespace Lua {

class LuaValueList
{
public:
	LuaValueList();

	LuaValueList( Diluculum::LuaValueList val );

	LuaValueList( const Lua::LuaValueList& obj);

	~LuaValueList();

	Diluculum::LuaValueList getValue() const;

	void setValue( Diluculum::LuaValueList val );

	void push_back( const Diluculum::LuaValue& val );

	Diluculum::LuaValue& operator[]( int index );

private:
	Diluculum::LuaValueList val;

};
}

#endif // LUAVALUELIST_H
