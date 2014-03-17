#ifndef LUAINTERFACE_H
#define LUAINTERFACE_H

#include "QString"
#include "Diluculum/LuaValue.hpp"
#include "Diluculum/Types.hpp"

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

    Diluculum::LuaValueList callFunction(QString funcName, Diluculum::LuaValueList params = Diluculum::LuaValueList());

    Diluculum::LuaValueList callFunction(unsigned int length, QString args[],
        Diluculum::LuaValueList params = Diluculum::LuaValueList());

    Diluculum::LuaState *getLuaState() const;

private:
    LuaInterface();
    LuaInterface(LuaInterface const&);
    void operator=(LuaInterface const&);

    static LuaInterface *instance;

    Diluculum::LuaState *luaState;
};

}

#endif // LUAINTERFACE_H
