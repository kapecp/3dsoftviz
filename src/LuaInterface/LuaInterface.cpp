#include "LuaInterface/LuaInterface.h"


// lua development
extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

// include diluculum header files
#include "Diluculum/CppObject.hpp"
#include "Diluculum/LuaExceptions.hpp"
#include "Diluculum/LuaFunction.hpp"
#include "Diluculum/LuaState.hpp"
#include "Diluculum/LuaUserData.hpp"
#include "Diluculum/LuaUtils.hpp"
#include "Diluculum/LuaValue.hpp"
#include "Diluculum/LuaVariable.hpp"
#include "Diluculum/Types.hpp"

Lua::LuaInterface *Lua::LuaInterface::instance;

Lua::LuaInterface *Lua::LuaInterface::getInstance(){
    if (instance == NULL)
        instance = new LuaInterface;
    return instance;
}

Lua::LuaInterface::~LuaInterface()
{
    delete luaState;
    if (instance != NULL){
        instance = NULL;
    }
}

Lua::LuaInterface::LuaInterface()
{
    luaState = new Diluculum::LuaState;
}

void Lua::LuaInterface::executeFile(std::string path)
{
    luaState->doFile(path);
}

std::string Lua::LuaInterface::getString(std::string name)
{
    Diluculum::LuaVariable var = (*luaState)[name];
    return var.value().asString();
}

std::string Lua::LuaInterface::getString(unsigned int length,std::string args[])
{
    Diluculum::LuaVariable var = (*luaState)[args[0]];
    for(unsigned int i = 1; i < length; i++) {
        var = var[args[i]];
    }
    return var.value().asString();
}

int Lua::LuaInterface::getInt(std::string name)
{
    Diluculum::LuaVariable var = (*luaState)[name];
    return var.value().asInteger();
}

int Lua::LuaInterface::getInt(unsigned int length,std::string args[])
{
    Diluculum::LuaVariable var = (*luaState)[args[0]];
    for(unsigned int i = 1; i < length; i++) {
        var = var[args[i]];
    }
    return var.value().asInteger();
}

double Lua::LuaInterface::getDouble(std::string name)
{
    Diluculum::LuaVariable var = (*luaState)[name];
    return var.value().asNumber();
}

double Lua::LuaInterface::getDouble(unsigned int length, std::string args[])
{
    Diluculum::LuaVariable var = (*luaState)[args[0]];
    for(unsigned int i = 1; i < length; i++) {
        var = var[args[i]];
    }
    return var.value().asNumber();
}
