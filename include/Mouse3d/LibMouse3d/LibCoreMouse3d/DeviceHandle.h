#ifndef DEVICEHANDLE_H
#define DEVICEHANDLE_H

#include <spwmacro.h>
#include <si.h>
#include <siapp.h>

namespace LibMouse3d{
namespace LibCoreMouse3d{

/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief LibCore class responsible for monitoring device.
*/
class DeviceHandle{

public:

    //constructor
    DeviceHandle();

    //destructor
    ~DeviceHandle();

    //handle
    //TODO ERROR DEVICEHANDLE is being defined
    DeviceHandle *hndl;

    //method for registering device
    DeviceHandle *GetInstance();

private:

};

}
}
#endif
