#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H

#include "QString"
#include "Diluculum/LuaValue.hpp"
#include <Diluculum/LuaWrappers.hpp>
#include "Diluculum/Types.hpp"
#include <Diluculum/LuaExceptions.hpp>
#include "LuaTypes/LuaValueList.h"
#include "LuaTypes/LuaState.h"

namespace Diluculum {
class LuaState;
}

namespace Lua {

/**
     * \class LuaInterface
     * \brief Interface between C++ and Lua
     *
     * Class is implemented as singleton. Allows access to variables in lua, executing files and functions.
     *
     * \author Frantisek Nagy
     * \date 19.5.2014
     */
class LuaInterface
{
public:
	/**
	     * \fn getInstance
	     * \brief returns single instance of LuaInterface.
	     *
	     * \return static instance (singleton)
	     */
	static LuaInterface* getInstance();

	~LuaInterface();

	/**
	     * \fn executeFile
	     * \brief executes lua file in resources/scripts directory
	     *
	     * \param path  path to file
	     */
	void executeFile( QString path );

	/**
	     * \fn getString
	     * \brief returns string value of given variable in Lua
	     *
	     * \param name  name of variable
	     * \return QString  value of given variable
	     */
	QString getString( QString name );

	/**
	     * \fn getString
	     * \brief returns string value of given nested (in table) variable in Lua
	     *
	     * \param length    depth of nesting in table (length of args)
	     * \param args      names of table and indexes to access nested variable
	     * \return QString  value of given variable
	     */
	QString getString( unsigned int length, QString args[] );

	/**
	     * \fn getInt
	     * \brief returns integer value of given variable in Lua
	     *
	     * \param name  name of variable
	     * \return int  value of given variable
	     */
	int getInt( QString name );

	/**
	     * \fn getInt
	     * \brief returns integer value of given nested (in table) variable in Lua
	     *
	     * \param length    depth of nesting in table (length of args)
	     * \param args      names of table and indexes to access nested variable
	     * \return int  value of given variable
	     */
	int getInt( unsigned int length, QString args[] );

	/**
	     * \fn getDouble
	     * \brief returns double precision value of given variable in Lua
	     *
	     * \param name  name of variable
	     * \return double  value of given variable
	     */
	double getDouble( QString name );

	/**
	     * \fn getDouble
	     * \brief returns double precision value of given nested (in table) variable in Lua
	     *
	     * \param length    depth of nesting in table (length of args)
	     * \param args      names of table and indexes to access nested variable
	     * \return double  value of given variable
	     */
	double getDouble( unsigned int length, QString args[] );

	/**
	     * \fn callFunction
	     * \brief calls function in Lua and returns result as LuaValueList
	     *
	     * \params funcName name of the function to be called
	     * \params params   function input parameters
	     * \return Diluculum::LuaValueList  returned value/s
	     */
	Lua::LuaValueList callFunction( QString funcName, Lua::LuaValueList params = Lua::LuaValueList() );

	/**
	     * \fn callFunction
	     * \brief calls function in Lua and returns result as LuaValueList
	     *
	     * \params funcName name of the function to be called
	     * \params params   function input parameters
	     * \return Diluculum::LuaValueList  returned value/s
	     */
	Lua::LuaValueList callFunction( unsigned int length, QString args[],
									Lua::LuaValueList params = Lua::LuaValueList() );

	/**
	     * \fn getLuaState
	     * \brief returns Diluculum::LuaState to access Diluculum functions (table iteration, callback registration)
	     * \return Diluculum::LuaState  lua state from Diluculum library
	     */
	Lua::LuaState getLuaState() const;

	/**
		 * \fn doString
		 * \brief returns Diluculum::LuaValueList, performs Diluculum::doString with LuaInterface, instead of LuaState
		 * \return Diluculum::LuaValueList from Diluculum library
		 */
	Lua::LuaValueList doString( const std::string& what );



private:
	LuaInterface();
	LuaInterface( LuaInterface const& );
	void operator=( LuaInterface const& );

	/**
	    *  LuaInterface *instance
	    *  \brief instance of LuaInterface
	    */
	static LuaInterface* instance;

	/**
	    *  Diluculum::LuaState *luaState
	    *  \brief underlying Diluculum::LuaState
	    */
	Lua::LuaState luaState;
};

}


/**
 * TOTO A NASLEDUJUCE MAKRO PREBRANE Z Diluculum:LuaWrappers
 * Returns the name of the wrapper function that is created by
 *  \c DILUCULUM_WRAP_FUNCTION() for a given function name.
 *  @param FUNC The function whose wrapper name is desired.
 */
#define LUA_WRAPPER_FUNCTION(FUNC)        \
   Lua__ ## FUNC ## __Wrapper_Function


/** Creates a \c lua_CFunction that wraps a function with the signature like the
 *  following one:
 *  <p><tt>Lua::LuaValueList Func (const Lua::LuaValueList& params)</tt>
 *  <p>Notice that, thanks to the use of <tt>Lua::LuaValueList</tt>s, the
 *  wrapped function can effectively take and return an arbitrary number of
 *  values.
 *  @note The name of the created wrapper function is a decorated version of the
 *        \c FUNC parameter. The decoration scheme can be quite complicated and
 *        is subject to change in future releases of Diluculum, so don't try to
 *        use it directly. Use the \c LUA_WRAPPER_FUNCTION() macro to
 *        obtain it instead.
 *  @note The proper way to report errors from the function being wrapped is by
 *        <tt>throw</tt>ing a \c Diluculum::LuaError. The created wrapper
 *        function will handle these exceptions and "translate" them to a call
 *        to \c lua_error().
 *  @see LUA_WRAPPER_FUNCTION() To find out the name of the created
 *       wrapper function.
 *  @param FUNC The function to be wrapped.
 */
#define LUA_WRAP_FUNCTION(FUNC)                                               \
int LUA_WRAPPER_FUNCTION(FUNC) (lua_State* ls)                                \
{                                                                             \
   using std::for_each;                                                       \
   using boost::bind;                                                         \
   using Diluculum::PushLuaValue;                                             \
   using Diluculum::Impl::ReportErrorFromCFunction;                           \
																			  \
   try                                                                        \
   {                                                                          \
	  /* Read parameters and empty the stack */                               \
	  const int numParams = lua_gettop (ls);                                  \
	  Diluculum::LuaValueList params;										  \
	  for (int i = 1; i <= numParams; ++i)                                    \
		 params.push_back (Diluculum::ToLuaValue (ls, i));                    \
	  lua_pop (ls, numParams);                                                \
																			  \
	  /* Call the wrapped function */                                         \
	  auto ret = FUNC (params);                                               \
	  /* Push the return values and return */                                 \
	  for_each (ret.getValue().begin(), ret.getValue().end(), bind (PushLuaValue, ls, _1));\
	  return ret.getValue().size();                                           \
   }                                                                          \
   catch (Diluculum::LuaError& e)                                             \
   {                                                                          \
	  ReportErrorFromCFunction (ls, e.what());                                \
	  return 0;                                                               \
   }                                                                          \
   catch(...)                                                                 \
   {                                                                          \
	  ReportErrorFromCFunction (ls, "Unknown exception caught by wrapper.");  \
	  return 0;                                                               \
   }                                                                          \
}

#endif // LUAINTERFACE_H
