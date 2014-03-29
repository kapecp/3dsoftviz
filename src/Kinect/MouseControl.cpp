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
