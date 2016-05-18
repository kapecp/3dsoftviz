#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H

#include "QString"
#include "Diluculum/LuaValue.hpp"
#include "Diluculum/Types.hpp"

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
    Diluculum::LuaValueList callFunction( QString funcName, Diluculum::LuaValueList params = Diluculum::LuaValueList() );

    /**
         * \fn callFunction
         * \brief calls function in Lua and returns result as LuaValueList
         *
         * \params funcName name of the function to be called
         * \params params   function input parameters
         * \return Diluculum::LuaValueList  returned value/s
         */
    Diluculum::LuaValueList callFunction( unsigned int length, QString args[],
                                          Diluculum::LuaValueList params = Diluculum::LuaValueList() );

    /**
         * \fn getLuaState
         * \brief returns Diluculum::LuaState to access Diluculum functions (table iteration, callback registration)
         * \return Diluculum::LuaState  lua state from Diluculum library
         */
    Diluculum::LuaState* getLuaState() const;

    /**
    	 * \fn doString
    	 * \brief returns Diluculum::LuaValueList, performs Diluculum::doString with LuaInterface, instead of LuaState
    	 * \return Diluculum::LuaValueList from Diluculum library
    	 */
    Diluculum::LuaValueList doString( const std::string& what );



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
    Diluculum::LuaState* luaState;
};

}

#endif // LUAINTERFACE_H
