#ifndef LEAPCAMERAMANIPULATOR_H
#define LEAPCAMERAMANIPULATOR_H
#include "LeapLib/LeapExport.h"

namespace Leap {

class LEAPLIB_EXPORT LeapCameraManipulator
{
public:
	virtual ~LeapCameraManipulator( void ) {}

	enum Movement {
		RIGHT, LEFT, UP, DOWN, FORWARD, BACKWARD
	};

	virtual void enableCameraMovement(Movement direction) = 0;
	virtual void disableCameraMovement() = 0;
	virtual void rotateCamera(float py0, float px0, double throwScale, float py1, float px1) = 0;
};
}
#endif
