#include "Viewer/MouseControl.h"
#include "Core/Core.h"

#include  "QDebug"

Vwr::MouseControl::MouseControl()
{
	window = AppCore::Core::getInstance()->getCoreWindow();
	viewer = AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
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

	mIsClicAruco = false;
}

Vwr::MouseControl::~MouseControl()
{
}

void Vwr::MouseControl::moveMouseAruco(double positionX,double positionY,bool isClick, Qt::MouseButton button )
{
	qDebug() << positionX << "  " << positionY << "         " << isClick;

	float wieverX = (float) (positionX * viewer->width());
	float wieverY = (float) (positionY * viewer->height());
	int windowX = (int) positionX * viewer->width()  + viewer->x() + 8;
	int windowY = (int) positionY * viewer->height() + viewer->y() + 28;
	int screenX = (int) positionX * viewer->width()  + viewer->x() + window->x() + 8;
	int screenY = (int) positionY * viewer->height() + viewer->y() + window->y() + 28;


	viewer->cursor().setPos(screenX, screenY);


	wieverY =  ((float) viewer->height() - wieverY);

	if( isClick != mIsClicAruco){
		mIsClicAruco = isClick;

		if(isClick) {
			viewer->getEventQueue()->mouseButtonPress(wieverX, wieverY,button);
			viewer->getEventQueue()->mouseMotion(wieverX, wieverY);
		} else {
			viewer->getEventQueue()->mouseButtonRelease(wieverX, wieverY, button);
			return;
		}
	}
	viewer->getEventQueue()->mouseMotion(wieverX, wieverY);


}

void Vwr::MouseControl::moveMouse(float positionX, float positionY)
{
	// add event to Event Queue - osgGa eventy
	viewer->getEventQueue()->mouseMotion(positionX,positionY);
}

void Vwr::MouseControl::clickPressMouse(Qt::MouseButton button)
{
	//corection of cursor position for VieverQT
	this->corectionMousePosition(viewer->cursor().pos().x(),viewer->cursor().pos().y());
	//add press action
	viewer->getEventQueue()->mouseButtonPress(clickX,clickY,button);
	//register movement of action
	this->moveMouse(clickX,clickY);
}

void Vwr::MouseControl::clickPressMouse(float positionX,float positionY,Qt::MouseButton button)
{
	viewer->getEventQueue()->mouseButtonPress(positionX,positionY,button);
	this->moveMouse(positionX,positionY);
}

void Vwr::MouseControl::releasePressMouse(Qt::MouseButton button)
{
	//corection of position
	this->corectionMousePosition(viewer->cursor().pos().x(),viewer->cursor().pos().y());
	//release of button
	viewer->getEventQueue()->mouseButtonRelease(clickX,clickY,button);
}

void Vwr::MouseControl::releasePressMouse(float positionX, float positionY, Qt::MouseButton button)
{
	viewer->getEventQueue()->mouseButtonRelease(positionX,positionY,button);
}

void Vwr::MouseControl::clickMouseLeftButton()
{
	this->clickPressMouse(Qt::LeftButton);
	this->moveMouse(clickX,clickY);
	this->releasePressMouse(Qt::LeftButton);
}

void Vwr::MouseControl::corectionMousePosition(int positionX, int positionY)
{
	// VieverQT is shiffted from start X,Y
	clickX=(float)(positionX-mWindowStartX);
	//reverted Y + shifted position + constant for corection
	clickY=(float)(mWindowHeight+mWindowStartY-positionY+20);
}


void Vwr::MouseControl::moveCursorMouse(double positionX, double positionY, bool isClick)
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


void Vwr::MouseControl::moveCursorWorldCoordinates(double positionX, double positionY, bool isClick)
{
	int newPositionX=(int) (positionX*mRatioX*mSpeedMoving);
	int newPositionY=(int) (positionY*mRatioY*mSpeedMoving);
	viewer->cursor().setPos(newPositionX,newPositionY);
	if(isClick)
	{
		this->corectionMousePosition(viewer->cursor().pos().x(),viewer->cursor().pos().y());
		this->moveMouse(clickX,clickY);
	}

}

void Vwr::MouseControl::setSpeedUpMoving(double speed)
{
	mSpeedMoving=speed;
}
