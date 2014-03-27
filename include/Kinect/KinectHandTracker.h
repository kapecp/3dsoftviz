#include "Kinect/HistoryBuffer.h"
#include "OpenNI.h"
#include "NiTE.h"
#include "Kinect/KinectRecognition.h"

#include "QOSG/ViewerQT.h"

#ifndef KINECTHANDTRACKER_H
#define KINECTHANDTRACKER_H

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

	float handX;
	float handY;

	void getAllGestures();
	void getAllHands();


	nite::HandTracker m_pHandTracker;
	std::map<int, HistoryBuffer<20> *> g_histories;

	nite::HandTrackerFrameRef handTrackerFrame;

private:
	bool isClick;
	QOSG::ViewerQT *viewer;

};
}
#endif // KINECTHANDTRACKER_H
