#include "Mouse3d/LibMouse3d/LibCoreMouse3d/EventThread.h"
#include "Mouse3d/LibMouse3d/LibCoreMouse3d/DeviceHandle.h"

LibMouse3d::LibCoreMouse3d::EventThread::EventThread( QObject* parent ) : QThread( parent ) {

}

LibMouse3d::LibCoreMouse3d::EventThread::~EventThread() {

}

void LibMouse3d::LibCoreMouse3d::EventThread::run(){


    //initialization for platform dependent SiGetEvent
    SiGetEventWinInit(eData.get(), msg, wParam, lParam);

    //get a message from message front of device
    while(true)//(GetMessage(msg, NULL, 0, 0))
    {

        if (SiGetEvent(LibMouse3d::LibCoreMouse3d::DeviceHandle::getInstance()->getDeviceRef(),
                       0, eData.get(), siEvent.get()) == SI_IS_EVENT){
          //events use interface class to trigger action in main window
          switch (siEvent.get()->type)
            {
            case SI_MOTION_EVENT:
              break;

            case SI_ZERO_EVENT:
              break;

            case SI_APP_EVENT:
              break;

            case SI_CMD_EVENT:
              break;

            case SI_DEVICE_CHANGE_EVENT:
              break;

            default:
              break;
            }
          }
        }

     LibMouse3d::LibCoreMouse3d::DeviceHandle::TerminateDevice(LibCoreMouse3d::DeviceHandle::getInstance()->getDeviceRef());
}

