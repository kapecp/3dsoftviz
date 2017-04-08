#include "OpenCV/CameraStream.h"
#include <QDebug>
#include <opencv2/highgui/highgui.hpp>
#include "Util/ApplicationConfig.h"
#include <easylogging++.h>
#include <thread>

namespace OpenCV {

CameraStream::CameraStream( ) :
	QObject(),
    osg::Image(),
    threadCount(0)
{
#ifdef WIN32
    this->image = cv::Mat( 480,640, CV_8UC3, CV_RGB( 0,0,0 ) ); // Black on Win
#endif

    this->tracker = new OpenCV::HandTracker();
    updateBackgroundImage( this->image , false);

}

CameraStream::~CameraStream() {}

void CameraStream::updateBackgroundImage( cv::Mat cvImg , bool trackHands)
{

	if ( cvImg.empty() ) {
		qDebug() << "CameraStream::updateBackgroundImage(): warning, cvImg is empty!";
		return;
    }

    if (trackHands) {
        this->trackMutex.lock();
        threadCount++;
        //LOG (INFO) << "this->image " + std::to_string(this->image.channels()) + " cvImage: " + std::to_string(cvImg.channels());
        this->image = cvImg.clone();
        this->image = this->tracker->findHand(this->image, 0);

        setImage( this->image.cols, this->image.rows,
              this->image.channels(), GL_INTENSITY8, GL_LUMINANCE, GL_UNSIGNED_BYTE,
              this->image.data,
              osg::Image::NO_DELETE, 1 );

        //LOG (INFO) << "Num of threads: " + std::to_string(threadCount);
        threadCount--;
        this->trackMutex.unlock();
    }
    else {
        this->image = cvImg.clone();
        setImage( this->image.cols, this->image.rows,
              this->image.channels(), GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,
              this->image.data,
              osg::Image::NO_DELETE, 1 );
    }
    cvImg.~Mat();



    dirty();
}

} // namespace OpenCV
