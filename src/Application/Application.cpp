#include "Application/Application.h"

namespace App {

Application::Application(int &argc, char **argv) : QApplication( argc, argv ) {
#if defined(Q_OS_LINUX)
    //XInitThreads();
    //XSelectInput(QX11Info::display(), DefaultRootWindow(QX11Info::display()), SubstructureNotifyMask);
#endif
}

Application::~Application() {
}

#ifdef Q_OS_LINUX

bool Application::x11EventFilter(XEvent *event) {
    this->event = *event;
    //qDebug() << "Event Type = " << this->event->type;
    return false;
}

void Application::getX11Event(XEvent &event){
    //qDebug() << "Event Type = " << this->event->type;
    event = this->event;
}

#endif

} //App
