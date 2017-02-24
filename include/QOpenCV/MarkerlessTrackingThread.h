#ifndef MARKERLESSTRACKINGTHREAD_H
#define MARKERLESSTRACKINGTHREAD_H

#include <QThread>

#include "opencv2/highgui/highgui.hpp" //boh vie naco toto je

namespace OpenCV {
class CapVideo;
}

namespace QOpenCV
{


class MarkerlessTrackingThread : public QThread
{
	Q_OBJECT

public:

	MarkerlessTrackingThread( QObject* parent = 0 );
	~MarkerlessTrackingThread( void );

	void run();

signals:

	void pushImage( cv::Mat Image );

public slots:

	void setCancel( bool );

	void setCapVideo( OpenCV::CapVideo* capVideo );

private:

	OpenCV::CapVideo*	mCapVideo;
	bool				mCancel;


};
}

#endif //MARKERLESSTRACKINGTHREAD_H
