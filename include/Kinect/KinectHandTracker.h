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

	float handZ[2];

	// both hands position
	float getArrayHands[NUM_HANDS][NUM_HANDS];
	bool isTwoHands;

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
