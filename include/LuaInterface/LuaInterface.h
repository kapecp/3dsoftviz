#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H

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

namespace Lua {

class LuaInterface
{
public:
    static LuaInterface *getInstance();

    void executeFile(std::string path);

    std::string getString(std::string name);

    std::string getString(unsigned int length, std::string args[]);

    int getInt(std::string name);

    int getInt(unsigned int length, std::string args[]);

    double getDouble(std::string name);

    double getDouble(unsigned int length, std::string args[]);
private:
    LuaInterface();
    LuaInterface(LuaInterface const&);
    void operator=(LuaInterface const&);

    static LuaInterface *instance;

    Diluculum::LuaState *luaState;
};

}

#endif // LUAINTERFACE_H
