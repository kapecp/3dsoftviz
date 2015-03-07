#ifndef LEAPLISTENER_H
#define LEAPLISTENER_H
#include "Leap.h"

namespace Leap {

class LeapListener;

/**
 * @author Filip Mikle
 * @brief The Leap listener class
 * @brief Used for all operations regarding LeapSensor
 */
class LeapListener : public Listener
{
  public:
    Controller controller;
    void onInit(const Controller&);
    void onConnect(const Controller&);
    void onDisconnect(const Controller&);
    void onExit(const Controller&);
    void onFrame(const Controller&);
    void onFocusGained(const Controller&);
    void onFocusLost(const Controller&);
    void onDeviceChange(const Controller&);
    void onServiceConnect(const Controller&);
    void onServiceDisconnect(const Controller&);
};
}

#endif
