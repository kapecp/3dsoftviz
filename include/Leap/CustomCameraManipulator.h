#ifndef CUSTOMCAMERAMANIPULATOR_H
#define CUSTOMCAMERAMANIPULATOR_H

#include "LeapLib/LeapCameraManipulator.h"
#include "Viewer/CameraManipulator.h"

namespace Leap {

class CustomCameraManipulator : public LeapCameraManipulator
{
public:
	CustomCameraManipulator(Vwr::CameraManipulator* cameraManipulator);
	~CustomCameraManipulator( void );

	void enableCameraMovement(Movement direction);
	void disableCameraMovement();
	void rotateCamera(float py0, float px0, double throwScale, float py1, float px1);

	Vwr::CameraManipulator* cameraManipulator;
};
}
#endif
