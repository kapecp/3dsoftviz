#ifndef DEVICEHANDLE_H
#define DEVICEHANDLE_H

#include <spwmacro.h>
#include <si.h>
#include <siapp.h>

using std::string;

namespace LibMouse3d{
namespace LibCoreMouse3d{

/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief LibCore class responsible for monitoring device.
*/
class DeviceHandle{

public:

    void InitDevice();

    /**
     * @brief TerminateDevice
     * @param hndl
     * @author Michal Fasanek
     * @brief Terminate connection to a device passed by parameter
     */
    static void TerminateDevice(SiHdl device);

    /**
     * @brief isInitialized
     * @return
     * @brief Returns TRUE if device handle is initialized, else FALSE
     */
    bool isInitialized();

    //method for registering device
    static DeviceHandle* getInstance();

    SiHdl getDeviceRef();

    //destructor
    ~DeviceHandle();


private:
    //constructor
    DeviceHandle();

    //reference to device returned by SiOpen
    SiHdl deviceRef;

    //singleton object - device
    static DeviceHandle* device;

    const std::shared_ptr<SiOpenData> initData;

};

}
}
#endif
