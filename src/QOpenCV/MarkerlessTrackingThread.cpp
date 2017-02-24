#include "QOpenCV/MarkerlessTrackingThread.h"


#include "OpenCV/CapVideo.h"
#include "QDebug"

QOpenCV::MarkerlessTrackingThread::MarkerlessTrackingThread( QObject* parent )
	: QThread( parent )
{
	qDebug() << "QOpenCV::MarkerlessTrackingThread::Constructor";
	mCapVideo	= NULL;
	mCancel		= false;
}

QOpenCV::MarkerlessTrackingThread::~MarkerlessTrackingThread( void )
{
}

void QOpenCV::MarkerlessTrackingThread::run()
{
	qDebug() << "QOpenCV::MarkerlessTrackingThread::run";
	mCancel = false;
	cv::Mat image;

	if ( mCapVideo == NULL ) {
		qDebug() << "QOpenCV::MarkerlessTrackingThread::run Camera is NULL";
		return;
	}

	while ( !mCancel ) {

		image = mCapVideo->queryFrame();
		cv::cvtColor( image, image, CV_BGR2RGB );
		cv::flip( image, image, 1 );

		if ( !image.empty() && image.data ) {
			emit pushImage( image.clone() );
		}

		msleep(40);

	}

	mCapVideo->release();
	mCapVideo = NULL;
}

void QOpenCV::MarkerlessTrackingThread::setCancel( bool set )
{
	qDebug() << "QOpenCV::MarkerlessTrackingThread::setCancel" << set;
	mCancel = set;
}

void QOpenCV::MarkerlessTrackingThread::setCapVideo( OpenCV::CapVideo *capVideo )
{
	qDebug() << "QOpenCV::MarkerlessTrackingThread::setCapVideo";
	mCapVideo = capVideo;
}
