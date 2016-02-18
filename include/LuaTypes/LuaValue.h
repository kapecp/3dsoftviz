#ifndef LUAVALUE_H
#define LUAVALUE_H

#include "Diluculum/LuaValue.hpp"
#include "Diluculum/Types.hpp"

namespace Lua {

/**
	 * \class LuaValue
	 * \brief Wrapper for Diluculum:LuaValue data type
	 *
	 * Class is implemented as proxy. Allows access to LuaValue data type and it's basic methods.
	 *
	 * \author Dalibor Mészáros
	 * \date 5.12.2015
	 */
class LuaValue
{
public:
	LuaValue();

	LuaValue( Diluculum::LuaValue val );

	LuaValue( const Lua::LuaValue& obj);

	~LuaValue();

	/**
		 * \fn getValue
		 * \brief returns Diluculum:LuaValue wrapped value.
		 *
		 * \return Diluculum:LuaValue value
		 */
	Diluculum::LuaValue getValue() const;

	/**
		 * \fn setValue
		 * \brief sets Diluculum:LuaValue wrapped value.
		 *
		 * \param Diluculum::LuaValue value
		 */
	void setValue( Diluculum::LuaValue val );

private:
	Diluculum::LuaValue val;

};
}

#endif // LUAVALUE_H
