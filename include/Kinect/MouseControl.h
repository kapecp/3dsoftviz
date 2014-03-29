#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H

#include <QtGui/QKeyEvent>
#include "QOSG/ViewerQT.h"


namespace Kinect
{

/**
	* @brief The MouseControl class
	*@author Matej Marconak
 */
class MouseControl
{

public:

	MouseControl();
	~MouseControl();

	void clickMouse();

	void clickMouse(Qt::MouseButton);

	void clickMouse(int positionX,int positionY,Qt::MouseButton);

	void releaseClickMouse();

	void releaseClickMouse(Qt::MouseButton);

	void releaseClickMouse(int positionX,int positionY,Qt::MouseButton);

	void moveMouse(int positionX,int positionY);

	void moveCursorMouse(int positionX,int positionY,bool isClick);

	void corectionMousePosition(int positionX,int positionY);

private:

	QOSG::ViewerQT *viewer;

	int mWindowWidth;
	int mWindowHeight;
	int mWindowStartX;
	int mWindowStartY;
	int mWindowEndX;
	int mWindowEndY;

	int clickX;
	int clickY;

};
}
#endif // MOUSECONTROL_H
