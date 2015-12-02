#ifndef EVEBTTHREAH_H
#define EVEBTTHREAH_H

#include "DeviceHandle.h"
#include <si.h>
#include <siapp.h>
#include <QThread>

//using namespace System::Windows::Forms;

namespace LibMouse3d{
namespace LibCoreMouse3d{


/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief LibCore class responsible for catching device activity.
*/
//TODO class MOUSE3DLIB_EXPORT EventThread : public QThread{
class EventThread : public QThread{

    Q_OBJECT

public:


    //constructor
    EventThread(QObject* parent = NULL);

    //destructor
    ~EventThread(void);

    void run();

private:
    //received message -unsigned integer-
    LPMSG msg;

    //storage of received data - platform dependent - probably front
    const std::shared_ptr<SiGetEventData> eData;

    /*
    //message windows-specific parameters
    WPARAM wParam;
    LPARAM lParam;
    */

    /**
     * @brief Mouse3d thread
     */
    LibMouse3d::LibCoreMouse3d::EventThread* mThrMouse3d;


    //incoming mouse event
    std::shared_ptr<SiSpwEvent> siEvent;
};

}
}
#endif
