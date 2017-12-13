#include "Mouse3d/LibMouse3d/Mouse3dDevices.h"

namespace LibMouse3d {

Mouse3dDevices::Mouse3dDevices( QOSG::CoreWindow* window ) :
	window( window )
{
#if defined(Q_OS_WIN)

	this->mouse = new Mouse3DInput( window );
	QObject::connect( mouse, SIGNAL( Move3d( std::vector<float>& ) ), window, SLOT( OnMove( std::vector<float>& ) ) );

#elif defined(Q_WS_X11) || defined(Q_OS_LINUX)
#if QT_VERSION >= 0x050000
	//todo
#elif QT_VERSION < 0x050000
	this->mouse = new Mouse3dUnixDevice( window );
	mouse->initMouse3d();
#endif

#elif defined(Q_OS_MAC)

	this->mouse = new Mouse3dMacDevice( window );
	QObject::connect( mouse, SIGNAL( PassMotion( std::vector<float>& ) ), window, SLOT( OnMove( std::vector<float>& ) ) );

#endif
}

Mouse3dDevices::~Mouse3dDevices()
{
	delete mouse;
}
}
