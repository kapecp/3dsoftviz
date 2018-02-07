#pragma once

#include <QObject>
//#include <QWidget>

#include "Application/Application.h"
#include "QOSG/CoreWindow.h"

class Mouse3dUnixDevice : public QObject
{
	Q_OBJECT

public:
	Mouse3dUnixDevice( /*QWidget *window*/ QOSG::CoreWindow* window );

	~Mouse3dUnixDevice();

	void initMouse3d();

public slots:

#if QT_VERSION >= 0x050000
	//todo
#elif QT_VERSION < 0x050000
	void translateX11Event( XEvent* event );
#endif

	void translateDummy();

private:
	//QWidget *window;
	QOSG::CoreWindow* window;
#if QT_VERSION >= 0x050000
	//todo
#elif QT_VERSION < 0x050000
	Display* display;
#endif
};
