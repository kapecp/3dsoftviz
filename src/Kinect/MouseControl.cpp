#include "Kinect/MouseControl.h"
#include "Core/Core.h"

#include  "QDebug"

Kinect::MouseControl::MouseControl()
{
	viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
	if(viewer!=NULL)
	{
		mWindowStartX=viewer->pos().x();
		mWindowStartY=viewer->pos().y();
		mWindowWidth=viewer->width();
		mWindowHeight=viewer->height();
		mWindowEndX=mWindowStartX+mWindowWidth;
		mWindowEndY=mWindowStartX+mWindowHeight;

		//primary speed of movement
		mSpeedMoving=1.0;
		//from Kinect World coordinates are 640x480
		mRatioX = mWindowEndX/640.0;
		mRatioY=mWindowEndY/480.0;

	}
	else
	{
		qDebug() << "[MouseControl] viewer is not open";
	}
}

void Kinect::MouseControl::moveMouse(float positionX, float positionY)
{
	// add event to Event Queue - osgGa eventy
	viewer->getEventQueue()->mouseMotion(positionX,positionY);
}

void Kinect::MouseControl::clickPressMouse(Qt::MouseButton button)
{
	//corection of cursor position for VieverQT
	this->corectionMousePosition(viewer->cursor().pos().x(),viewer->cursor().pos().y());
	//add press action
	viewer->getEventQueue()->mouseButtonPress(clickX,clickY,button);
	//register movement of action
	this->moveMouse(clickX,clickY);
}

void Kinect::MouseControl::clickPressMouse(float positionX,float positionY,Qt::MouseButton button)
{
	viewer->getEventQueue()->mouseButtonPress(positionX,positionY,button);
	this->moveMouse(positionX,positionY);
}

void Kinect::MouseControl::releasePressMouse(Qt::MouseButton button)
{
	//corection of position
	this->corectionMousePosition(viewer->cursor().pos().x(),viewer->cursor().pos().y());
	//release of button
	viewer->getEventQueue()->mouseButtonRelease(clickX,clickY,button);
}

void Kinect::MouseControl::releasePressMouse(float positionX, float positionY, Qt::MouseButton button)
{
	viewer->getEventQueue()->mouseButtonRelease(positionX,positionY,button);
}

void Kinect::MouseControl::clickMouseLeftButton()
{
	this->clickPressMouse(Qt::LeftButton);
	this->moveMouse(clickX,clickY);
	this->releasePressMouse(Qt::LeftButton);
}

void Kinect::MouseControl::corectionMousePosition(int positionX, int positionY)
{
	// VieverQT is shiffted from start X,Y
	clickX=(float)(positionX-mWindowStartX);
	//reverted Y + shifted position + constant for corection
	clickY=(float)(mWindowHeight+mWindowStartY-positionY+20);
}


void Kinect::MouseControl::moveCursorMouse(double positionX, double positionY, bool isClick)
{
	double curentCursorX=(double)viewer->cursor().pos().x();
	double curentCursorY=(double)viewer->cursor().pos().y();

	//data if previous position
	static double subPixelX=0;
	static double subPixelY=0;

	static double prevX=(double) positionX;
	static double prevY= (double) positionY;
	double deltaX=(double) ((double)(positionX)-prevX);
	double deltaY=(double) ((double)(positionY)-prevY);

	//based on formula
	double speed = sqrt(deltaX*deltaX + deltaY*deltaY);
	//acceleration
	speed = 1.0*speed + 0.09*speed*speed;

	int newPositionX=(int) (curentCursorX + subPixelX+deltaX*speed);
	int newPositionY=(int) (curentCursorY + subPixelY+deltaY*speed);

	viewer->cursor().setPos(newPositionX,newPositionY);
	// if button is pressed, need for movement osgGA
	if(isClick)
	{
		this->corectionMousePosition(viewer->cursor().pos().x(),viewer->cursor().pos().y());
		this->moveMouse(clickX,clickY);
	}
	//for previous position
	subPixelX=(deltaX*speed - (int)(deltaX*speed));
	subPixelX=(deltaY*speed - (int)(deltaY*speed));

	prevX=(double) positionX;
	prevY= (double) positionY;

}


void Kinect::MouseControl::moveCursorWorldCoordinates(double positionX, double positionY, bool isClick)
{
	int newPositionX=positionX*mRatioX*mSpeedMoving;
	int newPositionY=positionY*mRatioY*mSpeedMoving;

	viewer->cursor().setPos(newPositionX,newPositionY);
	if(isClick)
	{
		this->corectionMousePosition(viewer->cursor().pos().x(),viewer->cursor().pos().y());
		this->moveMouse(clickX,clickY);
	}

}

void Kinect::MouseControl::setSpeedUpMoving(double speed)
{
	mSpeedMoving=speed;
}
