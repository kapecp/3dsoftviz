#ifndef MOUSE3D_H
#define MOUSE3D_H

#include <QtGlobal>

#include <QApplication>

#include "QOSG/CoreWindow.h"
#include "Application/Application.h"

#if defined(Q_OS_WIN)
#include "Mouse3d/LibMouse3d/Windows/Mouse3DInput.h"
#elif defined(Q_WS_X11) || defined(Q_OS_LINUX)
#include "Mouse3d/LibMouse3d/Unix/Mouse3dUnixDevice.h"
#elif defined(Q_OS_MAC)
#include "Mouse3d/LibMouse3d/Mac/Mouse3dMacDevice.h"
#endif

namespace QOSG {
class CoreWindow;
}

namespace LibMouse3d {

/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief Class using system specific solutions for 3dMouse.
*/

class Mouse3dDevices : public QObject
{
	Q_OBJECT

public:
	Mouse3dDevices( QOSG::CoreWindow* window );
	~Mouse3dDevices();

private:
	QOSG::CoreWindow* window;

#if defined(Q_OS_WIN)

	Mouse3DInput* mouse;

#elif defined(Q_WS_X11) || defined(Q_OS_LINUX)

	Mouse3dUnixDevice* mouse;

#elif defined(Q_OS_MAC)

	Mouse3dMacDevice* mouse;

#endif

};
}

#endif

