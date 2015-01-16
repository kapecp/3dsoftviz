#ifndef LUAGRAPHOBSERVER_H
#define LUAGRAPHOBSERVER_H

namespace Lua {

/**
 * @brief The LuaGraphObserver class for notifying when hzpergraph changes
 *
 * @author Frantisek Nagy
 * @date 19.5.2014
 */
class LuaGraphObserver
{
public:

    /**
     * @brief onUpdate method that will be called after hypergraph changes
     */
    virtual void onUpdate() = 0;
};

}
#endif // LUAGRAPHOBSERVER_H
