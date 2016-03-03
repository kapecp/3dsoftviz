#include "Fglove/FgloveThread.h"

#include "Core/Core.h"
#include "QtCore"
#include "QDebug"

Fglove::FgloveThread::FgloveThread( QObject* parent ) : QThread( parent )
{

	// variable for last recognized gesture
	pGlove = NULL;
	mLastGesture = -2;
	mNodeSelected = false;


	qDebug() << "fgloveThread created";
}

Fglove::FgloveThread::~FgloveThread( void )
{
	qDebug() << "fgloveThread destroyed";
}

void Fglove::FgloveThread::run()
{
	pGlove = NULL;

	// initialize gloves on port 0
	pGlove = fdOpen( "USB0" );

	if ( NULL == ( pGlove = fdOpen( "USB0" ) ) ) {
		qDebug() << "failed initGloves";
		return;
	}
	else {
		qDebug() << "initGloves succeeded";

		// recognizing gestures
		while ( 1 ) {

			// get gesture number
			switch ( fdGetGesture( pGlove ) ) {

				// case index finger pointed
				case 1: {
					qDebug() << "command: select left side";
					AppCore::Core::getInstance()->getCoreWindow()->multiSelectClicked( true );
					QOSG::ViewerQT* viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();

					if ( viewer!=NULL ) {
						int mWindowWidth=viewer->width();
						int mWindowHeight=viewer->height();
						viewer->getEventQueue()->mouseButtonPress( 0,0,Qt::LeftButton );
						viewer->getEventQueue()->mouseMotion( mWindowWidth/2,mWindowHeight );
						viewer->getEventQueue()->mouseButtonRelease( mWindowWidth/2,mWindowHeight,Qt::LeftButton );
					}

					mLastGesture = 1;
					mNodeSelected = true;
				}
				break;

				// case little finger pointed
				case 8: {
					qDebug() << "command: select right side";
					AppCore::Core::getInstance()->getCoreWindow()->multiSelectClicked( true );
					QOSG::ViewerQT* viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();

					if ( viewer!=NULL ) {
						int mWindowWidth=viewer->width();
						int mWindowHeight=viewer->height();
						viewer->getEventQueue()->mouseButtonPress( mWindowWidth/2,0,Qt::LeftButton );
						viewer->getEventQueue()->mouseMotion( mWindowWidth,mWindowHeight );
						viewer->getEventQueue()->mouseButtonRelease( mWindowWidth,mWindowHeight,Qt::LeftButton );
					}

					mLastGesture = 8;
					mNodeSelected = true;
				}
				break;

				// case index and little finger pointed
				case 9: {
					qDebug() << "command: select all nodes";
					AppCore::Core::getInstance()->getCoreWindow()->multiSelectClicked( true );
					QOSG::ViewerQT* viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();

					if ( viewer!=NULL ) {
						int mWindowWidth=viewer->width();
						int mWindowHeight=viewer->height();
						viewer->getEventQueue()->mouseButtonPress( 0,0,Qt::LeftButton );
						viewer->getEventQueue()->mouseMotion( mWindowWidth,mWindowHeight );
						viewer->getEventQueue()->mouseButtonRelease( mWindowWidth,mWindowHeight,Qt::LeftButton );
					}

					mLastGesture = 9;
					mNodeSelected = true;
				}
				break;

				// case index and middle finger
				case 3: {
					qDebug() << "command: clear screen";
					AppCore::Core::getInstance()->getCoreWindow()->multiSelectClicked( false );
					QOSG::ViewerQT* viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();

					if ( viewer!=NULL ) {
						viewer->getEventQueue()->mouseButtonPress( 0,0,Qt::LeftButton );
					}

					mLastGesture = 3;
					mNodeSelected = false;
				}
				break;
				// case fist
				case 0: {
					if ( mLastGesture != 0 && mNodeSelected ) {
						qDebug() << "command: sphere";
						AppCore::Core::getInstance()->getCoreWindow()->setRestriction_Sphere();
						mLastGesture = 0;
					}
				}
				break;
				// case flat hand
				case 15: {
					if ( mLastGesture != 15 ) {
						qDebug() << "command: unset restrictions";
						AppCore::Core::getInstance()->getCoreWindow()->unsetRestriction();
						mLastGesture = 15;
					}
				}
				break;

				default:
					break;

			}
			Sleep( 45 );
		}
	}


	qDebug() << "[FgloveThread::run()]";
	return;
}
