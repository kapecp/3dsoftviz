#ifndef CONNECTIONINTERFACE_H
#define CONNECTIONINTERFACE_H


//TODO zistit ako v QtC includovat path nezavisle na strukture projektu
#include "LibCoreMouse3d/EventThread.h"

namespace QOSG{
    class CoreWindow;
}

namespace LibMouse3d{

/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief Interface class responsible for passing device to application once it is registered.
*/
class ConnectionInterface{

public:

    LibMouse3d::LibCoreMouse3d::EventThread* thread;

    //passed pointer to the main window
    QOSG::CoreWindow* win;

    //constructor
    ConnectionInterface(QOSG::CoreWindow* win);

    //destructor
    ~ConnectionInterface();

    void CreateConnection();

private:

};

}
#endif
