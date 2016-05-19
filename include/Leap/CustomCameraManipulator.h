#ifndef CUSTOMCAMERAMANIPULATOR_H
#define CUSTOMCAMERAMANIPULATOR_H

#include "LeapLib/LeapCameraManipulator.h"
#include "Viewer/CameraManipulator.h"
#include "Viewer/CoreGraph.h"
#include "Layout/LayoutThread.h"
#include "Layout/FRAlgorithm.h"

namespace Leap {

class CustomCameraManipulator : public LeapCameraManipulator
{
public:
    CustomCameraManipulator(Vwr::CameraManipulator* cameraManipulator,Layout::LayoutThread* layout,
                            Vwr::CoreGraph* coreGraph);
	~CustomCameraManipulator( void );

	void enableCameraMovement(Movement direction);
	void disableCameraMovement();
	void rotateCamera(float py0, float px0, double throwScale, float py1, float px1);

    //jurik
    void graphRotateSwipe(int swipeDirection);
    void scaleNodes(bool scaleUp);
    void rotateArucoLeft();
    void rotateArucoRight();
    void scaleEdgesUp();
    void scaleEdgesDown();
    //*****


    Vwr::CameraManipulator* cameraManipulator;
    Layout::LayoutThread* layout;
    Vwr::CoreGraph* coreGraph;
};
}
#endif
