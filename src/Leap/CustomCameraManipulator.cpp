#include "Leap/CustomCameraManipulator.h"

Leap::CustomCameraManipulator::CustomCameraManipulator(Vwr::CameraManipulator* cameraManipulator)
{
	this->cameraManipulator = cameraManipulator;
}

Leap::CustomCameraManipulator::~CustomCameraManipulator( void )
{
	//
}

void Leap::CustomCameraManipulator::enableCameraMovement(Movement direction){
	this->cameraManipulator->enableCameraMovement(static_cast<Vwr::CameraManipulator::Movement>(direction));
}

void Leap::CustomCameraManipulator::disableCameraMovement(){
	this->cameraManipulator->disableCameraMovement();
}

void Leap::CustomCameraManipulator::rotateCamera(float py0, float px0, double throwScale, float py1, float px1){
	this->cameraManipulator->rotateCamera(py0,px0,throwScale,py1, px1);
}
