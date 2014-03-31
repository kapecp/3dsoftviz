#include "Kinect/KinectHandTracker.h"

#include "QDebug"

Kinect::KinectHandTracker::KinectHandTracker(openni::Device *device, openni::VideoStream *m_depth)
{
	m_pHandTracker.create(device);
	m_pHandTracker.startGestureDetection(nite::GESTURE_WAVE);
	m_pHandTracker.startGestureDetection(nite::GESTURE_CLICK);
	isClick=false;
	mouse = new Kinect::MouseControl();
	mDepth=m_depth;
}


Kinect::KinectHandTracker::~KinectHandTracker()
{
}

void Kinect::KinectHandTracker::getAllGestures()
{
	this->m_pHandTracker.readFrame(&this->handTrackerFrame);

	const nite::Array<nite::GestureData>& gestures = this->handTrackerFrame.getGestures();
	for (int i = 0; i < gestures.getSize(); ++i)
	{
		if (gestures[i].isComplete())
		{
			printf("completed gesture\n");
			if(gestures[i].getType()==nite::GESTURE_CLICK)
			{
				printf("gesture click is on\n");
				if(isClick)
				{
					isClick=false;
					printf("Release");
					mouse->releasePressMouse(Qt::LeftButton);
				}
				else
				{
					isClick=true;
					printf("Click");
					mouse->clickPressMouse(Qt::LeftButton);
				}

			}
			const nite::Point3f& position = gestures[i].getCurrentPosition();
			printf("Gesture %d at (%f,%f,%f)\n", gestures[i].getType(), position.x, position.y, position.z);

			nite::HandId newId;
			this->m_pHandTracker.startHandTracking(gestures[i].getCurrentPosition(), &newId);
		}
	}
}

void Kinect::KinectHandTracker::getAllHands()
{
	// List of hands evidence
	// If hand matches old positions, previous ID is assigned again
	const nite::Array<nite::HandData>& hands= this->handTrackerFrame.getHands();

	printf("%d hands\n", hands.getSize());
	this->isTwoHands = false;

	for (int i = 0; i < hands.getSize(); ++i)
	{
		const nite::HandData& user = hands[i];

		if (!user.isTracking())
		{
			//printf("Lost hand %d\n", user.getId());
			nite::HandId id = user.getId();
			HistoryBuffer<20>* pHistory = this->g_histories[id];
			this->g_histories.erase(this->g_histories.find(id));
			delete pHistory;
		}
		else
		{
			if (user.isNew())
			{
				//printf("New hand id %d\n", user.getId());
				this->g_histories[user.getId()] = new HistoryBuffer<20>;
			}
			// Hand evidence in Buffer
			HistoryBuffer<20>* pHistory = this->g_histories[user.getId()];
			pHistory->AddPoint(user.getPosition());

			// Data for mouse
			//first find HAND = MOUSE
			if(i==0)
			{
				//mouse->moveCursorMouse(user.getPosition().x/2,-1.0*user.getPosition().y/2,isClick);
				//printf("%lf %lf",user.getPosition().x,-1,0*user.getPosition().y);
				/// new version
				coordinateConverter.convertWorldToDepth(*mDepth, user.getPosition().x, user.getPosition().y, user.getPosition().z, &mDepthX, &mDepthY, &mDepthZ);
				mouse->moveCursorWorldCoordinates(mDepthX,mDepthY,isClick);
				/////////////

			}

			// If two hands have been found get the position of the rectangle
			if (hands.getSize() == 2)
			{
				printf("two hands found\n");
				// get positions for both hands

				// TODO - further implementation should include depth information in pixels
				this->handZ[i] = user.getPosition().z;

				this->getArrayHands[i][0] = user.getPosition().x;
				this->getArrayHands[i][1] = 0 - user.getPosition().y;

				this->isTwoHands = true;
			}
		}
	}
}
