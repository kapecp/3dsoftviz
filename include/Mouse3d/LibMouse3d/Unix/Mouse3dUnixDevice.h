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
	void translateX11Event( XEvent* event );

	void translateDummy();

private:
	//QWidget *window;
	QOSG::CoreWindow* window;
	Display* display;
};
