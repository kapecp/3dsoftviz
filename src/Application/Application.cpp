#include "Application/Application.h"

namespace App {

Application::Application(int &argc, char **argv) : QApplication( argc, argv ) {
#if defined(Q_OS_LINUX)
    //XInitThreads();
    evntDisp = QAbstractEventDispatcher::instance();
    evntDisp->setEventFilter((QAbstractEventDispatcher::EventFilter)customEventFilter);
#endif
}

Application::~Application() {
}

#ifdef Q_OS_LINUX

bool Application::customEventFilter(void *message) {
    XEvent *event;
    event = (XEvent *) message;
    return false;
}

bool Application::x11EventFilter(XEvent *event) {
    //QApplication::x11EventFilter( event );
    this->event = event;
    return false;
}

bool Application::getMyEvent(XEvent *event){
    event = this->event;
    return false;
}

#endif

} //App
