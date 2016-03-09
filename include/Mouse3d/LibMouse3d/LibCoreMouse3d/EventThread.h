#ifndef EVEBTTHREAH_H
#define EVEBTTHREAH_H

#include "Mouse3d/LibMouse3d/LibCoreMouse3d/EventThread.h"
#include "Mouse3d/LibMouse3d/LibCoreMouse3d/Mouse3DInput.h"
#include "Mouse3d/LibMouse3d/LibCoreMouse3d/MouseParameters.h"

#include <QThread>



//using namespace System::Windows::Forms;
namespace QOSG{
    class CoreWindow;
}
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
    Mouse3DInput* mouse;
    QOSG::CoreWindow* win;
    //constructor
    EventThread(QOSG::CoreWindow* window, QObject* parent = NULL);

    //destructor
    ~EventThread(void);

    void run();

};

}
}
#endif
