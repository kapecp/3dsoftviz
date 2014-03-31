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
		mWindowEndY=mWindowEndY+mWindowHeight;
	}
	else
	{
		qDebug() << "[MouseControl] viewer is not open";
	}
}

void Kinect::MouseControl::moveMouse(float positionX, float positionY)
{
	viewer->getEventQueue()->mouseMotion(positionX,positionY);
}

void Kinect::MouseControl::clickPressMouse(Qt::MouseButton button)
{
	this->corectionMousePosition(viewer->cursor().pos().x(),viewer->cursor().pos().y());
	viewer->getEventQueue()->mouseButtonPress(clickX,clickY,button);
	//TODO test action - click,move,release or click/release
	this->moveMouse(clickX,clickY);
}

void Kinect::MouseControl::clickPressMouse(float positionX,float positionY,Qt::MouseButton button)
{
	viewer->getEventQueue()->mouseButtonPress(positionX,positionY,button);
	//TODO  test action - click,move,release or click/release
	this->moveMouse(positionX,positionY);
}

void Kinect::MouseControl::releasePressMouse(Qt::MouseButton button)
{
	this->corectionMousePosition(viewer->cursor().pos().x(),viewer->cursor().pos().y());
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
	clickX=(float)(positionX-mWindowStartX);
	//temporary corection
	clickY=(float)(mWindowHeight+mWindowStartY-positionY+20);
}


void Kinect::MouseControl::moveCursorMouse(double positionX, double positionY, bool isClick)
{
	double curentCursorX=(double)viewer->cursor().pos().x();
	double curentCursorY=(double)viewer->cursor().pos().y();

	static double subPixelX=0;
	static double subPixelY=0;

	static double prevX=(double) positionX;
	static double prevY= (double) positionY;
	double deltaX=(double) ((double)(positionX)-prevX);
	double deltaY=(double) ((double)(positionY)-prevY);

	double speed = sqrt(deltaX*deltaX + deltaY*deltaY);
	//acceleration
	speed = 1.0*speed + 0.09*speed*speed;

	int newPositionX=(int) (curentCursorX + subPixelX+deltaX*speed);
	int newPositionY=(int) (curentCursorY + subPixelY+deltaY*speed);

	//TODO add constraint for position
	viewer->cursor().setPos(newPositionX,newPositionY);
	if(isClick)
	{
		this->corectionMousePosition(viewer->cursor().pos().x(),viewer->cursor().pos().y());
		this->moveMouse(clickX,clickY);
	}

	subPixelX=(deltaX*speed - (int)(deltaX*speed));
	subPixelX=(deltaY*speed - (int)(deltaY*speed));

	 prevX=(double) positionX;
	 prevY= (double) positionY;

}


void Kinect::MouseControl::moveCursorWorldCoordinates(double positionX, double positionY, bool isClick)
{
	//viewer->cursor().pos();
	viewer->cursor().setPos(positionX*1.5,positionY*1.5);
	if(isClick)
	{
		this->corectionMousePosition(viewer->cursor().pos().x(),viewer->cursor().pos().y());
		this->moveMouse(clickX,clickY);
	}

}




















