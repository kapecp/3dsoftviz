#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H

namespace Diluculum {
	class LuaState;
}

namespace Lua {

class LuaInterface
{
public:
    static LuaInterface *getInstance();

    ~LuaInterface();

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
