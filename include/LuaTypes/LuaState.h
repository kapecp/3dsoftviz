#ifndef LUASTATE_H
#define LUASTATE_H


#include "Diluculum/LuaState.hpp"

namespace Lua {

/**
	 * \class LuaState
	 * \brief Wrapper for Diluculum:LuaState data type
	 *
	 * Class is implemented as proxy. Allows access to LuaState data type and it's basic methods.
	 *
	 * \author Hagara Lukas
	 * \date 17.11.2016
	 */
class LuaState
{
public:
	LuaState();

	LuaState( Diluculum::LuaState* val );

	~LuaState();

	/**
		 * \fn getValue
		 * \brief returns Diluculum:LuaState* wrapped value.
		 *
		 * \return Diluculum:LuaState* value
		 */
	Diluculum::LuaState* getValue() const;

	/**
		 * \fn setValue
		 * \brief sets Diluculum:LuaState* wrapped value.
		 *
		 * \param Diluculum::LuaState* value
		 */
	void setValue( Diluculum::LuaState* val );

private:
	Diluculum::LuaState* val;

};
}

#endif // LUASTATE_H
