#include "LeapLib/LeapCameraStream.h"
#include "HandModule/HandTrackerAdapter.h"
#include <easylogging++.h>
#include <string>

Leap::LeapCameraStream::LeapCameraStream( ) :
	osg::Image()
{
	allocateImage( 640, 240, 1, GL_INTENSITY8, GL_LUMINANCE, 1 );
	setPixelBufferObject( new osg::PixelBufferObject( this ) );
	setImage( 640, 240, 1, GL_INTENSITY8, GL_LUMINANCE, GL_UNSIGNED_BYTE,
			  nullptr, osg::Image::NO_DELETE, 1 );
}

Leap::LeapCameraStream::~LeapCameraStream() {}

void Leap::LeapCameraStream::updateBackgroundImage( unsigned char* buffer )
{
    HandTrackerAdapter *adapter = new HandTrackerAdapter();
    cv::Mat mat = adapter->convertBuffer(buffer);
    setImage( 640, 240, 1, GL_INTENSITY8, GL_LUMINANCE, GL_UNSIGNED_BYTE,
              mat.data,osg::Image::NO_DELETE, 1 );
}


