#pragma once

#include <QApplication>

namespace App {

class Application : public QApplication {
	   Q_OBJECT

public:
   Application(int &argc, char **argv);

  ~Application();

#if defined(Q_OS_LINUX)

  virtual bool x11EventFilter(XEvent *event);

#endif

};
} //App
