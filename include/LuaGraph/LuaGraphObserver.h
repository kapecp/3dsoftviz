#ifndef LUAGRAPHOBSERVER_H
#define LUAGRAPHOBSERVER_H

namespace Lua {

class LuaGraphObserver
{
public:
    virtual void onUpdate() = 0;
};

}
#endif // LUAGRAPHOBSERVER_H
