#ifndef SIGNALINTERFACE_H
#define SIGNALINTERFACE_H


//TODO zistit ako v QtC includovat path nezavisle na strukture projektu
#include "LibCoreMouse3d/EventThread.h"

namespace LibMouse3d{

/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief Interface class responsible for passing received messages from device to application.
*/
class SignalInterface{

public:

    //constructor
    SignalInterface();

    //destructor
    ~SignalInterface();

private:

};

}
#endif
