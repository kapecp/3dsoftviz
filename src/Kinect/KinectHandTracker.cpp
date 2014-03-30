#include "Kinect/KinectHandTracker.h"

#include "QDebug"

Kinect::KinectHandTracker::KinectHandTracker(openni::Device *device)
{
	m_pHandTracker.create(device);
	m_pHandTracker.startGestureDetection(nite::GESTURE_WAVE);
	m_pHandTracker.startGestureDetection(nite::GESTURE_CLICK);
	isClick=false;
	mouse = new Kinect::MouseControl();
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
				mouse->moveCursorMouse(user.getPosition().x,user.getPosition().y,isClick);
			}

			// If two hands have been found get the position of the rectangle
			if (hands.getSize() == 2)
			{
				printf("two hands found\n");
				// get positions for both hands
				for (int j = 0; j < hands.getSize(); ++j)
				{
					if (j < 1)
					{
						this->getArrayHands[i][j] = user.getPosition().x;
						printf("%d ruka x: %f\n", i+1, getArrayHands[i][j]);
					}
					else
					{
						this->getArrayHands[i][j] = 0 - user.getPosition().y;
						printf("%d ruka y: %f\n", i+1, getArrayHands[i][j]);
					}
				}
				// reconstruct the rect parameters and get distance
				if (i == 1)
				{
					// left hand is stored in handX
					this->getDistance[0] = getArrayHands[0][0]-getArrayHands[1][0];
					if (getDistance[0] < 0)
					{
						this->handX = getArrayHands[0][0];
						this->handY = getArrayHands[0][1];
					}
					else
					{
						this->handX = getArrayHands[1][0];
						this->handY = getArrayHands[1][1];
					}
					printf("rect x: %f\n",handX);
					printf("rect y: %f\n",handY);
					this->getDistance[1] = getArrayHands[0][1]-getArrayHands[1][1];
				}
			}
		}
	}
}
