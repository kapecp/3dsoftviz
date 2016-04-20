#ifndef EVEBTTHREAH_H
#define EVEBTTHREAH_H

#include <QDebug>
#include <QThread>

#include "QOSG/CoreWindow.h"

#include "Mouse3d/LibMouse3d/LibCoreMouse3d/MouseParameters.h"
#include "Mouse3d/LibMouse3d/Mouse3d.h"

//using namespace System::Windows::Forms;
namespace QOSG{
    class CoreWindow;
}
namespace LibMouse3d{
class Mouse3dDevice;

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
    EventThread(QOSG::CoreWindow* window, QObject* parent = NULL);

    //destructor
    ~EventThread(void);

    //run
    void run();

private:
	QOSG::CoreWindow* window;
	Mouse3dDevice* mouseDevice;
};
}
#endif
