#ifndef MOUSECONTROL_H
#define MOUSECONTROL_H

#include <QtGui/QKeyEvent>

namespace QOSG
{
class ViewerQT;
class CoreWindow;
}

namespace Vwr
{

/**
	* @brief Control of mouse action and movement for other way of controling mouse
	*@author Matej Marconak
 */
class MouseControl
{

public:

	MouseControl();
	~MouseControl();

	/**
	 *@brief click of left button of mouse at position of cursor
	 */
	void clickMouseLeftButton();

	/**
	 * @brief click and press of button at position os cursor
	 * @param button type of mouse button
	 */
	void clickPressMouse(Qt::MouseButton button);

	/**
	 * @brief click and press of button at position os cursor
	 * @param positionX position X of click
	 * @param positionY position Y of click
	 * @param button type of mouse button
	 */
	void clickPressMouse(float positionX, float positionY, Qt::MouseButton button);

	/**
	 * @brief release pressed button of mouse at position of cursor
	 * @param button type of mouse button
	 */
	void releasePressMouse(Qt::MouseButton button);

	/**
	 * @brief release pressed button of mouse at concrete position
	 * @param positionX position x of release pressed button
	 * @param positionY	position y of release pressed button
	 * @param button type of mouse button
	 */
	void releasePressMouse(float positionX,float positionY,Qt::MouseButton button);

	/**
	 * @brief move mouse position of GUI listener
	 * @param positionX position x of movement
	 * @param positionY position y of movement
	 */
	void moveMouse(float positionX, float positionY);

	/**
	 * @brief move cursor of mouse at concrete place of screen based on old position of mouse
	 * @param positionX new position x of cursor
	 * @param positionY new positino y of cursor
	 * @param isClick is press any button
	 */
	void moveCursorMouse(double positionX, double positionY, bool isClick);

	/**
	 * @brief move cursor of based on Kinect World position of Hand
	 * @param positionX position x
	 * @param positionY position y
	 * @param isClick is press any button
	 */
	void moveCursorWorldCoordinates(double positionX,double positionY,bool isClick);

	/**
	 * @brief set for speed up of movement of cursor
	 * @param speed speed up of movement
	 */
	void setSpeedUpMoving(double speed);

	void moveMouseAruco(double positionX,double positionY,bool isClick, Qt::MouseButton button );

private:

	/**
	 * @brief corection of cursor position for VieverQT
	 * @param positionX position of cursor x
	 * @param positionY position of cursor y
	 */
	void corectionMousePosition(int positionX,int positionY);

	QOSG::ViewerQT		*viewer;
	QOSG::CoreWindow	*window;

	double mSpeedMoving;
	double mRatioX;
	double mRatioY;

	int mWindowWidth;
	int mWindowHeight;
	int mWindowStartX;
	int mWindowStartY;
	int mWindowEndX;
	int mWindowEndY;

	float clickX;
	float clickY;

	bool mIsClicAruco;

};
}
#endif // MOUSECONTROL_H
