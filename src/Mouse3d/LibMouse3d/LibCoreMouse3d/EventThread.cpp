#include "Mouse3d/LibMouse3d/LibCoreMouse3d/EventThread.h"
#include "Mouse3d/LibMouse3d/LibCoreMouse3d/DeviceHandle.h"

using namespace LibMouse3d::LibCoreMouse3d;

EventThread::EventThread( QObject* parent ) : QThread( parent ) {
    qDebug() << "Mouse3d thread created";
}

EventThread::~EventThread() {
    qDebug() << "Mouse3d thread destroyed";
}

/**
 * @author Michal Fasanek
 * @brief LibMouse3d::LibCoreMouse3d::EventThread::TerminateThread
 * @brief Called from application - terminates thread and connection to the device
 */


void EventThread::run(){

    qDebug() << "Create connection call - EventThread" ;

    DeviceHandle* instance = DeviceHandle::GetInstance();

    qDebug() << "Got instance of handle";
    //get message posted by device
    //Considered using PeekMessage() instead, staying with GetMessage for now


    while(GetMessage(this->msg, NULL, 0, 0))
    {
        qDebug() << "Entered while cycle";

        //initialization for platform dependent SiGetEvent
        SiGetEventWinInit(this->eData.get(), this->msg->message, this->msg->wParam, this->msg->lParam);

        //get message from message front]
        //if SI_NOT_EVENT, ignore
        if (SiGetEvent(instance->GetDeviceRef(), 0, this->eData.get(), this->siEvent.get()) == SI_IS_EVENT)
        {
            //until mouse is flagged to cancel in main window
            if(instance->MouseCanceled())
                qDebug() << "Canceling mouse thread";
                break;


            //events use interface class to trigger action in main window
            switch (this->siEvent.get()->type)
            {
                //mouse moved event
                case SI_MOTION_EVENT:
                    qDebug() << "Captured MOTION event";
                    break;

                //zero event
                case SI_ZERO_EVENT:
                    break;

                //mouse application event
                case SI_APP_EVENT:
                    qDebug() << "Captured APP event";
                    break;

                //command event
                case SI_CMD_EVENT:
                    qDebug() << "Captured CMD event";
                    break;

                //shouldn't happen
                case SI_DEVICE_CHANGE_EVENT:
                    qDebug() << "Captured DEV event";
                    break;

                default:
                    DispatchMessage(this->msg);
                    break;
            }
        }


    }
    qDebug() << "Out of mouse thread";
}

