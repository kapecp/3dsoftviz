#ifndef LUAVALUEMAP_H
#define LUAVALUEMAP_H

#include "Diluculum/LuaValue.hpp"
#include "Diluculum/Types.hpp"

namespace Lua {

/**
	 * \class LuaValueMap
	 * \brief Wrapper for Diluculum:LuaValueMap data type
	 *
	 * Class is implemented as proxy. Allows access to LuaValueMap data type and it's basic methods.
	 *
	 * \author Dalibor Mészáros
	 * \date 5.12.2015
	 */
class LuaValueMap
{
public:
	LuaValueMap();

	LuaValueMap( Diluculum::LuaValueMap val );

	LuaValueMap( const Lua::LuaValueMap& obj);

	~LuaValueMap();

	/**
		 * \fn getValue
		 * \brief returns Diluculum:LuaValueMap wrapped value.
		 *
		 * \return Diluculum:LuaValueMap value
		 */
	Diluculum::LuaValueMap getValue() const;

	/**
		 * \fn setValue
		 * \brief sets Diluculum:LuaValueMap wrapped value.
		 *
		 * \param Diluculum::LuaValueMap value
		 */
	void setValue( Diluculum::LuaValueMap val );

	/**
		 * \fn begin
		 * \brief returns Diluculum::LuaValueMap::iterator to the first element of the wrapped value.
		 *
		 * \return iterator to the first element
		 */
	Diluculum::LuaValueMap::iterator begin();

	/**
		 * \fn end
		 * \brief returns Diluculum::LuaValueMap::iterator to the last element of the wrapper value.
		 *
		 * \return iterator to the last element
		 */
	Diluculum::LuaValueMap::iterator end();

	/**
		 * \fn find
		 * \brief returns Diluculum::LuaValueMap::iterator to the searched element of wrapped value.
		 *
		 * \return iterator to the searched element
		 */
	Diluculum::LuaValueMap::iterator find( const std::string& str );

	/**
		 * \fn insert
		 * \brief performs insert on the Diluculum::LuaValueMap wrapped value.
		 *
		 * \param inserted pair
		 */
	void insert( std::pair<Diluculum::LuaValue, Diluculum::LuaValue> val );

	/**
		 * \fn insertPair
		 * \brief transforms the input to std::pair<Diluculum::LuaValue, Diluculum::LuaValue> and then performs insert on the Diluculum::LuaValueMap wrapped value.
		 *
		 * \param values of the inserted pair
		 */
	void insertPair( long val_one, Diluculum::LuaValue val_two );

private:
	Diluculum::LuaValueMap val;

};
}

#endif // LUAVALUEMAP_H
