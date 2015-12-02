#include "Mouse3d/LibMouse3d/LibCoreMouse3d/DeviceHandle.h"
#include <string.h>
#include <QDebug>

using namespace LibMouse3d::LibCoreMouse3d;

DeviceHandle* DeviceHandle::device;

DeviceHandle::DeviceHandle(){

    //initialize
    this->InitDevice();

    this->SetMouseCancel(false);

    //get device driver version
    if(SiGetDriverInfo(driverVersion.get()) == SPW_ERROR)
         qDebug() << "Could not retrieve version info \n";
    else{
         qDebug() << "Version: ";
         qDebug() << driverVersion.get()->version;
         qDebug() << "\n";
    }
}


/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief DeviceHandle method responsible for binding device to object if not bound yet.
*/
DeviceHandle* DeviceHandle::GetInstance(){

    if(device == NULL){

        //call constructor
        device = new DeviceHandle();
    }

    return device;
}

/**
 * @brief LibMouse3d::LibCoreMouse3d::DeviceHandle::MouseCanceled
 * @return bool
 * @brief return true if mouse was set to cancel in main window, else false
 */
bool DeviceHandle::MouseCanceled(){
    return mouseCancel;
}

void DeviceHandle::TerminateDevice(){

    //terminate thread
    this->SetMouseCancel(true);

    //terminate handle
    SiClose(this->deviceRef);

    //terminate connection to device;
    SiTerminate();
}

void DeviceHandle::SetMouseCancel(bool set){

    this->mouseCancel = set;
}

/**
 * @brief LibMouse3d::LibCoreMouse3d::DeviceHandle::InitDevice
 * @param windowHandleId
 * @param appName
 * @author Michal Fasanek
 * @brief creates a connection to the hardware device using SI functions that should be passed to thread receiving events
 */
void DeviceHandle::InitDevice(){

    //initializing internal states
    SiInitialize();

    //trying to grab device handle
    if ( (this->deviceRef = SiOpen ("3dSoftviz", SI_ANY_DEVICE, SI_NO_MASK,
                           SI_EVENT, &initData)) == SI_NO_HANDLE)

        qDebug() << "Failed to open 3DxWare device \n";

    else

        qDebug() << "Success opening 3DxWare device \n";

}

bool DeviceHandle::IsInitialized(){
    if(this->deviceRef != NULL)
        return true;

    else
        return false;
}

SiHdl DeviceHandle::GetDeviceRef(){
        return this->deviceRef;
}

void DeviceHandle::SetDeviceRef(SiHdl &ref){
    this->deviceRef = ref;
}
