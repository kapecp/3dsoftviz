#pragma once

#include <QApplication>
#include <QtGlobal>
#include <QAbstractEventDispatcher>

//#include <QDebug>

#ifdef Q_OS_LINUX
    //#include <X11/Xlib.h>
    //#include <X11/Xutil.h>
    //#include <X11/Xos.h>
    //#include <X11/Xatom.h>
    //#include <X11/keysym.h>
#endif

namespace App {

class Application : public QApplication {
	   Q_OBJECT

public:
   Application(int &argc, char **argv);

  ~Application();

#if defined(Q_OS_LINUX)

   static bool customEventFilter(void *message);

   virtual bool x11EventFilter(XEvent *event);

   bool getMyEvent(XEvent *event);

private:
   XEvent *event;
   QAbstractEventDispatcher *evntDisp;

#endif

};
} //App
