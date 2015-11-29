#include "Mouse3d/LibMouse3d/LibCoreMouse3d/DeviceHandle.h"
#include <string.h>
#include <QDebug>

LibMouse3d::LibCoreMouse3d::DeviceHandle* LibMouse3d::LibCoreMouse3d::DeviceHandle::device;

LibMouse3d::LibCoreMouse3d::DeviceHandle::DeviceHandle() {

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

LibMouse3d::LibCoreMouse3d::DeviceHandle::~DeviceHandle() {

}

/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief DeviceHandle method responsible for binding device to object if not bound yet.
*/
LibMouse3d::LibCoreMouse3d::DeviceHandle* LibMouse3d::LibCoreMouse3d::DeviceHandle::GetInstance(){

    if(LibMouse3d::LibCoreMouse3d::DeviceHandle::device == NULL){

        //call constructor
        LibMouse3d::LibCoreMouse3d::DeviceHandle::device = new LibMouse3d::LibCoreMouse3d::DeviceHandle();
    }

    return LibMouse3d::LibCoreMouse3d::DeviceHandle::device;
}

/**
 * @brief LibMouse3d::LibCoreMouse3d::DeviceHandle::MouseCanceled
 * @return bool
 * @brief return true if mouse was set to cancel in main window, else false
 */
bool LibMouse3d::LibCoreMouse3d::DeviceHandle::MouseCanceled(){
    return LibCoreMouse3d::DeviceHandle::mouseCancel;
}

void LibMouse3d::LibCoreMouse3d::DeviceHandle::TerminateDevice(){

    //terminate thread
    this->SetMouseCancel(true);

    //terminate handle
    SiClose(this->GetDeviceRef());

    //terminate connection to device;
    SiTerminate();
}

void LibMouse3d::LibCoreMouse3d::DeviceHandle::SetMouseCancel(bool set){

    this->mouseCancel = set;
}

/**
 * @brief LibMouse3d::LibCoreMouse3d::DeviceHandle::InitDevice
 * @param windowHandleId
 * @param appName
 * @author Michal Fasanek
 * @brief creates a connection to the hardware device using SI functions that should be passed to thread receiving events
 */
void LibMouse3d::LibCoreMouse3d::DeviceHandle::InitDevice(){


    SiInitialize();

    this->SetDeviceRef(SiOpen("3dSoftviz", SI_ANY_DEVICE, SI_NO_MASK, SI_EVENT, this->initData.get()));
    if(this->GetDeviceRef() == SI_NO_HANDLE)
        qDebug() << "Failed to open 3DxWare device \n";

    else
        qDebug() << "Success opening 3DxWare device \n";
}

bool LibMouse3d::LibCoreMouse3d::DeviceHandle::IsInitialized(){
    if(this->deviceRef != NULL)
        return true;

    else
        return false;
}

SiHdl LibMouse3d::LibCoreMouse3d::DeviceHandle::GetDeviceRef(){
        return this->deviceRef;
}

void LibMouse3d::LibCoreMouse3d::DeviceHandle::SetDeviceRef(SiHdl ref){
    this->deviceRef = ref;
}
