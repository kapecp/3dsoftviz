#include "Application/Application.h"

#if defined(Q_OS_LINUX)

	#include <X11/Xlib.h>

#endif

namespace App {

Application::Application(int &argc, char **argv) : QApplication( argc, argv ) {
#if defined(Q_OS_LINUX)

	XInitThreads();

#endif
}

Application::~Application() {
}

#if defined(Q_OS_LINUX)

bool Application::x11EventFilter(XEvent *event) {
	return false;
}

#endif

} //App
