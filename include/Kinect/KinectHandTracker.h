#include "Kinect/HistoryBuffer.h"
#include "OpenNI.h"
#include "NiTE.h"
#include "Kinect/KinectRecognition.h"
#include "Kinect/MouseControl.h"


#ifndef KINECTHANDTRACKER_H
#define KINECTHANDTRACKER_H

#define NUM_HANDS 2

namespace Kinect{

/**
 * @author Patrik Hlavac
 * @brief The KinectHandTracker class
 */

class KinectHandTracker{

public:
	KinectHandTracker(openni::Device *device);
	~KinectHandTracker();

	//const nite::Array<nite::GestureData>& gestures;
	//const nite::Array<nite::GestureData>& hands;

	// left hand position
	float handX;
	float handY;

	// both hands position and distance/direction to determine left hand
	float getArrayHands[NUM_HANDS][NUM_HANDS];
	int getDistance[NUM_HANDS];
	//bool direction;

	void getAllGestures();
	void getAllHands();


	nite::HandTracker m_pHandTracker;
	std::map<int, HistoryBuffer<20> *> g_histories;

	nite::HandTrackerFrameRef handTrackerFrame;

private:
	bool isClick;
	MouseControl *mouse;

};
}
#endif // KINECTHANDTRACKER_H
