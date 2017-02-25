#include "QOpenCV/MarkerlessTrackingThread.h"

#include "easylogging++.h"
#include "OpenCV/CapVideo.h"
#include "OpenCV/MarkerlessTracker.h"

QOpenCV::MarkerlessTrackingThread::MarkerlessTrackingThread()
{
	mCapVideo	= nullptr;
	mCancel		= false;
}

QOpenCV::MarkerlessTrackingThread::~MarkerlessTrackingThread( void )
{
}

void QOpenCV::MarkerlessTrackingThread::run()
{
	LOG( INFO ) << "QOpenCV::MarkerlessTrackingThread::run";

	//initialization
	mCancel = false;
	cv::Mat image;
	OpenCV::MarkerlessTracker* markerlessTracker = new OpenCV::MarkerlessTracker();

	//no camera was set
	if ( mCapVideo == NULL ) {
		LOG( INFO ) << "QOpenCV::MarkerlessTrackingThread::run Camera is NULL";
		return;
	}

	//while cancel not requested
	while ( !mCancel ) {

		//pull new image from camera
		image = mCapVideo->queryFrame();

		cv::cvtColor( image, image, CV_BGR2RGB );
		cv::flip( image, image, 1 );

		//track on the current image
		markerlessTracker->track( image );

		//push the image
		if ( !image.empty() && image.data ) {
			emit pushImage( image.clone() );
		}

		//zZz
		msleep( 40 );

	}

	//cleanup
	mCapVideo->release();
	mCapVideo = NULL;
	delete markerlessTracker;
}

void QOpenCV::MarkerlessTrackingThread::setCancel( bool set )
{
	mCancel = set;
}

void QOpenCV::MarkerlessTrackingThread::setCapVideo( OpenCV::CapVideo* capVideo )
{
	mCapVideo = capVideo;
}
