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
