#include "Viewer/MouseControl.h"
#include "Core/Core.h"

#include  "QDebug"

Vwr::MouseControl::MouseControl() :
	// Primary speed of movement
	mSpeedMoving( 1.0 ),
	clickX( 0 ),
	clickY( 0 ),
	mIsClicAruco( false ),
	mouseHistory( new QList< osg::Vec2 > )

{
	window = AppCore::Core::getInstance()->getCoreWindow();
	viewer = AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
	if ( viewer ) {
		mWindowStartX = viewer->pos().x();
		mWindowStartY = viewer->pos().y();
		mWindowWidth = viewer->width();
		mWindowHeight = viewer->height();
		mWindowEndX = mWindowStartX+mWindowWidth;
		mWindowEndY = mWindowStartX+mWindowHeight;

		//from Kinect World coordinates are 640x480
		mRatioX = mWindowEndX/640.0;
		mRatioY = mWindowEndY/480.0;
	}
	else {
		qDebug() << "[MouseControl] viewer is not open";
	}
}

Vwr::MouseControl::~MouseControl()
{
}

void Vwr::MouseControl::moveMouse( float positionX, float positionY )
{
	// add event to Event Queue - osgGa eventy
	viewer->getEventQueue()->mouseMotion( positionX,positionY );
}

void Vwr::MouseControl::clickPressMouse( Qt::MouseButton button )
{
	//corection of cursor position for VieverQT
	this->corectionMousePosition( viewer->cursor().pos().x(),viewer->cursor().pos().y() );
	//add press action
	viewer->getEventQueue()->mouseButtonPress( clickX,clickY,button );
	//register movement of action
	this->moveMouse( clickX,clickY );
}

void Vwr::MouseControl::clickPressMouse( float positionX,float positionY,Qt::MouseButton button )
{
	viewer->getEventQueue()->mouseButtonPress( positionX,positionY,button );
	this->moveMouse( positionX,positionY );
}

void Vwr::MouseControl::releasePressMouse( Qt::MouseButton button )
{
	//corection of position
	this->corectionMousePosition( viewer->cursor().pos().x(),viewer->cursor().pos().y() );
	//release of button
	viewer->getEventQueue()->mouseButtonRelease( clickX,clickY,button );
}

void Vwr::MouseControl::releasePressMouse( float positionX, float positionY, Qt::MouseButton button )
{
	viewer->getEventQueue()->mouseButtonRelease( positionX,positionY,button );
}

void Vwr::MouseControl::clickMouseLeftButton()
{
	this->clickPressMouse( Qt::LeftButton );
	this->moveMouse( clickX,clickY );
	this->releasePressMouse( Qt::LeftButton );
}

void Vwr::MouseControl::corectionMousePosition( int positionX, int positionY )
{
	// VieverQT is shiffted from start X,Y
	clickX=static_cast<float>( positionX-mWindowStartX );
	//reverted Y + shifted position + constant for corection
	clickY=static_cast<float>( mWindowHeight+mWindowStartY-positionY+20 );
}


void Vwr::MouseControl::moveCursorMouse( double positionX, double positionY, bool isClick )
{
	double curentCursorX=static_cast<double>( viewer->cursor().pos().x() );
	double curentCursorY=static_cast<double>( viewer->cursor().pos().y() );

	//data if previous position
	static double subPixelX=0;
	static double subPixelY=0;

	static double prevX= positionX;
	static double prevY=  positionY;
	double deltaX= ( ( positionX )-prevX );
	double deltaY= ( ( positionY )-prevY );

	//based on formula
	double speed = sqrt( deltaX*deltaX + deltaY*deltaY );
	//acceleration
	speed = 1.0*speed + 0.09*speed*speed;

	int newPositionX= static_cast<int>( curentCursorX + subPixelX+deltaX*speed );
	int newPositionY= static_cast<int>( curentCursorY + subPixelY+deltaY*speed );

	viewer->cursor().setPos( newPositionX,newPositionY );
	// if button is pressed, need for movement osgGA
	if ( isClick ) {
		this->corectionMousePosition( viewer->cursor().pos().x(),viewer->cursor().pos().y() );
		this->moveMouse( clickX,clickY );
	}
	//for previous position
	subPixelX=( deltaX*speed - static_cast<int>( deltaX*speed ) );
	subPixelY=( deltaY*speed - static_cast<int>( deltaY*speed ) );

	prevX = positionX;
	prevY = positionY;

}


void Vwr::MouseControl::moveCursorWorldCoordinates( double positionX, double positionY, bool isClick )
{
	int newPositionX= static_cast<int>( positionX*mRatioX*mSpeedMoving );
	int newPositionY= static_cast<int>( positionY*mRatioY*mSpeedMoving );
	// get avg position from history buffer // smoother moving
	osg::Vec2 newPos = getMouseAvgPosition( osg::Vec2( osg::Vec2f::value_type( newPositionX ), osg::Vec2f::value_type( newPositionY ) ), 10 );

	viewer->cursor().setPos( static_cast<int>( newPos[0] ), static_cast<int>( newPos[1] ) );
	if ( isClick ) {
		this->corectionMousePosition( viewer->cursor().pos().x(),viewer->cursor().pos().y() );
		this->moveMouse( clickX,clickY );
	}

}

void Vwr::MouseControl::setSpeedUpMoving( double speed )
{
	mSpeedMoving=speed;
}

osg::Vec2 Vwr::MouseControl::getMouseAvgPosition( osg::Vec2 lastPos, int max )
{
	// add last position
	this->mouseHistory->append( lastPos );

	// while history is larger than max positions, remove oldest
	while ( this->mouseHistory->size() > max ) {
		this->mouseHistory->removeFirst();
	}

	int avgX = 0;
	int avgY = 0;
	for ( QList<osg::Vec2>::const_iterator iter = mouseHistory->begin(); iter != mouseHistory->end(); ++iter ) {
		avgX += static_cast<int>( ( *iter )[0] );
		avgY += static_cast<int>( ( *iter )[1] );
	}
	avgX /= mouseHistory->size();
	avgY /= mouseHistory->size();

	return osg::Vec2( static_cast<osg::Vec2f::value_type>( avgX ), static_cast<osg::Vec2f::value_type>( avgY ) );
}
