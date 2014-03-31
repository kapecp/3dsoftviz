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

	void clickMouseLeftButton();

	void clickPressMouse(Qt::MouseButton button);

	void clickPressMouse(float positionX, float positionY, Qt::MouseButton button);

	void releasePressMouse(Qt::MouseButton button);

	void releasePressMouse(float positionX,float positionY,Qt::MouseButton button);

	void moveMouse(float positionX, float positionY);

	void moveCursorMouse(double positionX, double positionY, bool isClick);

	void moveCursorWorldCoordinates(double positionX,double positionY,bool isClick);

private:

	void corectionMousePosition(int positionX,int positionY);

	QOSG::ViewerQT *viewer;

	int mWindowWidth;
	int mWindowHeight;
	int mWindowStartX;
	int mWindowStartY;
	int mWindowEndX;
	int mWindowEndY;

	float clickX;
	float clickY;

};
}
#endif // MOUSECONTROL_H
