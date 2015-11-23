#include "Mouse3d/LibMouse3d/LibCoreMouse3d/DeviceHandle.h"

LibMouse3d::LibCoreMouse3d::DeviceHandle::DeviceHandle() {

}

LibMouse3d::LibCoreMouse3d::DeviceHandle::~DeviceHandle() {

}

/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief DeviceHandle method responsible for binding device to running application.
*@brief TODO implement as SINGLETON.
*/
LibMouse3d::LibCoreMouse3d::DeviceHandle* LibMouse3d::LibCoreMouse3d::DeviceHandle::GetInstance(){
    DeviceHandle *dvchndl = NULL;

    return dvchndl;
}
