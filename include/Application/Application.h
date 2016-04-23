#pragma once

#include <QApplication>
#include <QObject>
#include <QtGlobal>

#include <QDebug>

#ifdef Q_OS_LINUX
	#include <QX11Info>
	#include "Mouse3d/LibMouse3d/Mouse3d.h"

	#include <X11/X.h>
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/Xatom.h>
	#include <X11/keysym.h>

	#undef Unsorted
	#undef None
	#undef Bool
	#undef KeyPress
	#undef KeyRelease
	#undef Above
	#undef Below
	#undef FocusIn
	#undef FocusOut
	#undef Always
	#undef Success
	#undef GrayScale
	#undef Status
	#undef CursorShape
	#undef FontChange
	#undef NormalState
	#undef index
	#undef rindex
#endif

namespace App {

class Application : public QApplication {
	Q_OBJECT

public:
   Application( int &argc, char **argv );

  ~Application();

#if defined(Q_OS_LINUX)

   bool x11EventFilter( XEvent *event );

   void x11InitConnection( LibMouse3d::Mouse3dDevice *mouse3d );

signals:
   void x11PassEvent( XEvent &event );

   void testPass( int i );

private:
   bool x11Connected;

#endif

};
} //App
