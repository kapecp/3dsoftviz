#ifndef LUAVALUELIST_H
#define LUAVALUELIST_H

#include "Diluculum/LuaValue.hpp"
#include "Diluculum/Types.hpp"

namespace Lua {

/**
	 * \class LuaValueList
	 * \brief Wrapper for Diluculum:LuaValueList data type
	 *
	 * Class is implemented as proxy. Allows access to LuaValueList data type and it's basic methods.
	 *
	 * \author Dalibor Mészáros
	 * \date 5.12.2015
	 */
class LuaValueList
{
public:
	LuaValueList();

	LuaValueList( Diluculum::LuaValueList val );

	LuaValueList( const Lua::LuaValueList& obj);

	~LuaValueList();

	/**
		 * \fn getValue
		 * \brief returns Diluculum:LuaValueList wrapped value.
		 *
		 * \return Diluculum:LuaValueList value
		 */
	Diluculum::LuaValueList getValue() const;

	/**
		 * \fn setValue
		 * \brief sets Diluculum:LuaValueList wrapped value.
		 *
		 * \param Diluculum::LuaValueList value
		 */
	void setValue( Diluculum::LuaValueList val );

	/**
		 * \fn push_back
		 * \brief performs push_back on the Diluculum::LuaValueList wrapped value.
		 *
		 * \param reference to the pushed element Diluculum::LuaValue
		 */
	void push_back( const Diluculum::LuaValue& val );

	Diluculum::LuaValue& operator[]( int index );

private:
	Diluculum::LuaValueList val;

};
}

#endif // LUAVALUELIST_H
