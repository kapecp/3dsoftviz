#include "OpenCV/CamSelectCore.h"

#include "Core/Core.h"
#include <opencv2/highgui/highgui.hpp>
#include "QOpenCV/CamSelectWindow.h"
#include "OpenCV/CapVideo.h"

#include <vector>

OpenCV::CamSelectCore* OpenCV::CamSelectCore::mCamSelectCore;

//constructor search automatically for number of active cameras and store them
OpenCV::CamSelectCore::CamSelectCore()
{
	mCamSelectCore = this;
	//this->app=app; // wtf???
	int max=this->countCameras();
	for ( int i=0; i<max; i++ ) {
		camlist.push_back( new OpenCV::CapVideo( i,0,0 ) );
	}
}
OpenCV::CamSelectCore::~CamSelectCore()
{
	for ( unsigned int i=0; i<camlist.size(); i++ ) {
		delete camlist[i];
	}
}

OpenCV::CapVideo* OpenCV::CamSelectCore::selectCamera()
{
	// Vytvorenie stringu pre vypis dat o kamere
	QString data;
	for ( unsigned int i=0; i < camlist.size(); i++ ) {
		data.append( QString::number( camlist[i]->getDeviceId() ) );
		data.append( "," );
		if ( camlist[i]->isOpened() ) {
			data.append( "yes" );
			data.append( "," );
			data.append( QString::number( camlist[i]->getWidth() ) );
			data.append( "," );
			data.append( QString::number( camlist[i]->getHeight() ) );
		}
		else {
			data.append( "no" );
			data.append( "," );
			data.append( "0" );
			data.append( "," );
			data.append( "0" );
		}
		if ( i<camlist.size()-1 ) {
			data.append( ";" );
		}
	}
	OpenCV::CamSelectWindow csw( AppCore::Core::getInstance( qApp )->getCoreWindow(),
								 qApp,
								 data );

	if ( csw.exec() ) {
		if ( camlist[device_id]->isOpened() ) {
			return camlist[device_id];
		}
	}

	return NULL;

}

// new instance of singleton
OpenCV::CamSelectCore* OpenCV::CamSelectCore::getInstance()
{
	if ( mCamSelectCore == NULL ) {
		mCamSelectCore = new OpenCV::CamSelectCore();
	}
	return mCamSelectCore;
}

// get the number of active cameras
int OpenCV::CamSelectCore::countCameras()
{
	int max = 10;
	for ( int i = 0; i < max; i++ ) {
		cv::VideoCapture temp( i );
		if ( !temp.isOpened() ) {
			temp.release();
			temp.~VideoCapture();
			return i;
		}
		temp.release();
		temp.~VideoCapture();
	}
	return max;
}

void OpenCV::CamSelectCore::setCam( std::vector<OpenCV::CapVideo*>::size_type dev_id, int width, int height )
{
	//camlist[dev_id]->startCamera( width,height );

	//jma tmp workaround
	camlist[dev_id]->startCamera( 640,480 );
	this->device_id=dev_id;
}
