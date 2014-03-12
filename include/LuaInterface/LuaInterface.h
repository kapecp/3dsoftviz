#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H

#include "QString"

namespace Diluculum {
	class LuaState;
}

namespace Lua {

class LuaInterface
{
public:
    static LuaInterface *getInstance();

    ~LuaInterface();

    void executeFile(QString path);

    QString getString(QString name);

    QString getString(unsigned int length, QString args[]);

    int getInt(QString name);

    int getInt(unsigned int length, QString args[]);

    double getDouble(QString name);

    double getDouble(unsigned int length, QString args[]);
private:
    LuaInterface();
    LuaInterface(LuaInterface const&);
    void operator=(LuaInterface const&);

    static LuaInterface *instance;

    Diluculum::LuaState *luaState;
};

}

#endif // LUAINTERFACE_H
