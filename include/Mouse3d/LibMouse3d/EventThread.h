#ifndef EVEBTTHREAH_H
#define EVEBTTHREAH_H

#include <QThread>

//using namespace System::Windows::Forms;
namespace QOSG {
class CoreWindow;
}
namespace LibMouse3d {
class Mouse3dDevices;

/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief LibCore class responsible for catching device activity.
*/

//TODO class MOUSE3DLIB_EXPORT EventThread : public QThread{
class EventThread : public QThread
{
	Q_OBJECT

public:
	//constructor
	EventThread( QOSG::CoreWindow* window, QObject* parent = NULL );

	//destructor
	~EventThread( void );

	//run
	void run();

	static void msleep( unsigned long msecs );

private:
	QOSG::CoreWindow* window;
	Mouse3dDevices* mouseDevice;
};
}
#endif
