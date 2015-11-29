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
// TODO class MOUSE3DLIB_EXPORT DeviceHandle{
class DeviceHandle{

public:

     //destructor
    ~DeviceHandle();

    void InitDevice();


    /**
     * @brief MouseCanceled
     * @return
     * @brief check if mouse is flagged (as/to) cancel
     */
    bool MouseCanceled();

    /**
     * @brief SetMouseCancel
     * @param set
     * @brief Set cancel flag on mouse as requested by parameter
     */
    void SetMouseCancel(bool set);

    /**
     * @brief TerminateDevice
     * @author Michal Fasanek
     * @brief Terminate connection, terminate thread, terminate handle
     * @brief Called from library interface
     */
    void TerminateDevice();

    /**
     * @brief isInitialized
     * @return
     * @brief Returns TRUE if device handle is initialized, else FALSE
     */
    bool IsInitialized();

    /**
     * @brief GetInstance
     * @return Returns DeviceHandle Singleton instance
     * @brief This is core Singleton method of DeviceHandle class. It checks whether DeviceHandle already exists.
     * @brief If exists, returns existing instance, else initializes instance and returns it.
     */
    static DeviceHandle* GetInstance();

    //getter method for deviceRef parameter of instance
    SiHdl GetDeviceRef();

    //setter method for deviceRef parameter of instance
    void SetDeviceRef(SiHdl ref);

    //constructor
    DeviceHandle();

private:


    //reference to device returned by SiOpen
    SiHdl deviceRef;

    std::shared_ptr<SiVerInfo> driverVersion;

    //singleton object - device
    static DeviceHandle* device;

    bool mouseCancel;

    const std::shared_ptr<SiOpenData> initData;

};

}
}
#endif
