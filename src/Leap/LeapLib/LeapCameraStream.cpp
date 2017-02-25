#include "LeapLib/LeapCameraStream.h"
#include <easylogging++.h>
#include <string>

Leap::LeapCameraStream::LeapCameraStream( ) :
	osg::Image()
{
    default_buffer = nullptr;
    setImage( 640, 240, 1, GL_INTENSITY8, GL_LUMINANCE, GL_UNSIGNED_BYTE,
			  nullptr,osg::Image::NO_DELETE, 1 );
}

Leap::LeapCameraStream::~LeapCameraStream() {}

void Leap::LeapCameraStream::updateBackgroundImage( unsigned char* buffer )
{
	if ( default_buffer == nullptr ) {
		default_buffer = buffer;
	}
	setImage( 640, 240, 1, GL_INTENSITY8, GL_LUMINANCE, GL_UNSIGNED_BYTE,
			  default_buffer,osg::Image::NO_DELETE, 1 );
}


