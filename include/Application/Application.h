#pragma once

#include <QtGlobal>

#include <QApplication>

#ifdef Q_OS_LINUX
	//#include <QX11Info>
	#include "Mouse3d/LibMouse3d/Mouse3d.h"
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

   void testPass();

private:
   bool x11Connected;

#endif

};
} //App
