#ifndef CONNECTIONINTERFACE_H
#define CONNECTIONINTERFACE_H


//TODO zistit ako v QtC includovat path nezavisle na strukture projektu
#include "LibCoreMouse3d/DeviceHandle.h"

namespace LibMouse3d{

/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief Interface class responsible for passing device to application once it is registered.
*/
class ConnectionInterface{

public:

    //constructor
    ConnectionInterface();

    //destructor
    ~ConnectionInterface();

private:

};

}
#endif
