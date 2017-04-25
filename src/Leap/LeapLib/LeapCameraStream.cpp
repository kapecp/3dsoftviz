#include "LeapLib/LeapCameraStream.h"
#include <easylogging++.h>
#include <string>

Leap::LeapCameraStream::LeapCameraStream( ) :
	osg::Image()
{
    this->tracker = new OpenCV::HandTracker();
	allocateImage( 640, 240, 1, GL_INTENSITY8, GL_LUMINANCE, 1 );
	setPixelBufferObject( new osg::PixelBufferObject( this ) );
	setImage( 640, 240, 1, GL_INTENSITY8, GL_LUMINANCE, GL_UNSIGNED_BYTE,
			  nullptr, osg::Image::NO_DELETE, 1 );
}

Leap::LeapCameraStream::~LeapCameraStream() {}

int Leap::LeapCameraStream::getImageWidth() {
    return 640;
}

int Leap::LeapCameraStream::getImageHeight() {
    return 240;
}

void Leap::LeapCameraStream::updateBackgroundImage( unsigned char* buffer )
{
    cv::Mat matrix(240, 640, CV_8UC1, &buffer[0]);
    this->contourPointList = this->tracker->findHand(matrix);
    setImage( 640, 240, 1, GL_INTENSITY8, GL_LUMINANCE, GL_UNSIGNED_BYTE,
              buffer,osg::Image::NO_DELETE, 1 );
}


