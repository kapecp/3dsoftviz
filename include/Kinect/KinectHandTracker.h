#ifndef KINECTHANDTRACKER_H
#define KINECTHANDTRACKER_H

#include "Kinect/HistoryBuffer.h"
#include "OpenNI.h"
#include "NiTE.h"

#define NUM_HANDS 2

namespace Vwr{
class MouseControl;
}

namespace Kinect{

/**
 * @author Patrik Hlavac
 * @brief The KinectHandTracker class
 */

class KinectRecognition;



class KinectHandTracker{

public:
	KinectHandTracker(openni::Device *device,openni::VideoStream  *m_depth);
	~KinectHandTracker();

	//const nite::Array<nite::GestureData>& gestures;
	//const nite::Array<nite::GestureData>& hands;

	float handZ[2];

	// both hands position
	float getArrayHands[NUM_HANDS][NUM_HANDS];
	bool isTwoHands;

	float slidingHand_x;
	float slidingHand_y;
	float slidingHand_z;
	char * slidingHand_type;

	void getAllGestures();
	void getAllHands();
	void getRotatingMove();

	void setCursorMovement(bool set);

	void setSpeedMovement(double set);

	nite::HandTracker m_pHandTracker;
	std::map<int, HistoryBuffer<20> *> g_histories;

	nite::HandTrackerFrameRef handTrackerFrame;

private:
	bool isClick;
	bool isCursorMovementEnable;

	Vwr::MouseControl *mouse;
	openni::VideoStream  *mDepth;

	openni::CoordinateConverter coordinateConverter;
	float mDepthX;
	float mDepthY;
	float mDepthZ;

	double mSpeed;

};
}
#endif // KINECTHANDTRACKER_H
