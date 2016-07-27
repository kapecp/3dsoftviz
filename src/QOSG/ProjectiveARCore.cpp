#include "QOSG/ProjectiveARCore.h"

#include "Core/Core.h"
#include "QOSG/ProjectiveARWindow.h"
#include "QOSG/ViewerQT.h"

QOSG::ProjectiveARCore* QOSG::ProjectiveARCore::mProjectiveARCore;

QOSG::ProjectiveARCore::ProjectiveARCore( QApplication* app, QWidget* parent )
{
	mProjectiveARCore		= this;
	mApp                    = app;
	mParent                 = parent;

	qDebug() << "Creating ProjectiveAR Window";
	mProjectiveARWindow =  new QOSG::ProjectiveARWindow( mParent, mApp );
	mProjectiveARViewer = mProjectiveARWindow->getViewerWidget();

}
QOSG::ProjectiveARCore::~ProjectiveARCore( void )
{

}

void QOSG::ProjectiveARCore::init()
{
	if ( !mProjectiveARWindow ) {
		qDebug() << "Creating ProjectiveAR Window";
		mProjectiveARWindow =  new QOSG::ProjectiveARWindow( mParent, mApp );
		mProjectiveARViewer = mProjectiveARWindow->getViewerWidget();
	}

	mProjectiveARWindow->show();
}

QOSG::ProjectiveARCore* QOSG::ProjectiveARCore::getInstance( QApplication* app, QWidget* parent )
{
	if ( mProjectiveARCore == NULL ) {
		mProjectiveARCore = new QOSG::ProjectiveARCore( app, parent );
	}
	return mProjectiveARCore;
}
