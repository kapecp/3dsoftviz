#ifndef LEAPCAMERASTREAM_H
#define LEAPCAMERASTREAM_H

#include <osg/Image>
#include <osg/Geometry>

namespace Leap {

class LeapCameraStream : public osg::Image
{
public:
	LeapCameraStream( );
	~LeapCameraStream();

public:
	void updateBackgroundImage( unsigned char* buffer );
};
}
#endif
