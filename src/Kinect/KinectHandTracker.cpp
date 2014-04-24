#include "Kinect/KinectHandTracker.h"
#include "Viewer/MouseControl.h"
#include "QDebug"

Kinect::KinectHandTracker::KinectHandTracker(openni::Device *device, openni::VideoStream *m_depth)
{
	m_pHandTracker.create(device);
	m_pHandTracker.startGestureDetection(nite::GESTURE_WAVE);
	m_pHandTracker.startGestureDetection(nite::GESTURE_CLICK);
	isClick=false;
	isCursorMovementEnable=true;
	mSpeed=1.0;
	mouse = new Vwr::MouseControl();
	mDepth=m_depth;
}


Kinect::KinectHandTracker::~KinectHandTracker()
{
}

void Kinect::KinectHandTracker::setCursorMovement(bool set)
{
	isCursorMovementEnable=set;
}

void Kinect::KinectHandTracker::setSpeedMovement(double set)
{
	mSpeed=set;
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

	//printf("%d hands\n", hands.getSize());
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
			if(i==0 && isCursorMovementEnable)
			{
				mouse->setSpeedUpMoving(mSpeed);
				//mouse->moveCursorMouse(user.getPosition().x/2,-1.0*user.getPosition().y/2,isClick);
				coordinateConverter.convertWorldToDepth(*mDepth, user.getPosition().x, user.getPosition().y, user.getPosition().z, &mDepthX, &mDepthY, &mDepthZ);
				mouse->moveCursorWorldCoordinates(mDepthX,mDepthY,isClick);
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

void Kinect::KinectHandTracker::getRotatingMove()
{
	// List of hands evidence
	// If hand matches old positions, previous ID is assigned again
	const nite::Array<nite::HandData>& hands= this->handTrackerFrame.getHands();

	this->slidingHand_x = 0;
	this->slidingHand_y = 0;
	this->slidingHand_z = 0;
	this->slidingHand_type = "";
	//strcpy(this->slidingHand_type, "");

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
				this->g_histories[user.getId()] = new HistoryBuffer<20>;
			}
			// Hand evidence in Buffer
			HistoryBuffer<20>* pHistory = this->g_histories[user.getId()];
			pHistory->AddPoint(user.getPosition());


			coordinateConverter.convertWorldToDepth(*mDepth, user.getPosition().x, user.getPosition().y, user.getPosition().z, &this->slidingHand_x, &this->slidingHand_y, &this->slidingHand_z);

			// printf("%lf %lf \n", this->slidingHand_x,this->slidingHand_y );


			float koordy[60] = {0};

			bool gesto_dolava = false;
			bool gesto_doprava = false;
			bool gesto_hore = false;
			bool gesto_dole = false;


			if(pHistory->GetSize() == 20)
			{ // ak je historia naplnena
				const nite::Point3f& position1 = pHistory->operator[](0);
				this->m_pHandTracker.convertHandCoordinatesToDepth(position1.x, position1.y, position1.z, &koordy[0], &koordy[1]);
				const nite::Point3f& position2 = pHistory->operator[](19);
				this->m_pHandTracker.convertHandCoordinatesToDepth(position2.x, position2.y, position2.z, &koordy[2], &koordy[3]);


				//check for output
				if(abs(koordy[0] - koordy[2]) > 100.0)
				{
					if(koordy[0] > koordy[2])
					{
						gesto_dolava = false;
						gesto_doprava = true;
					}
					else
					{
						gesto_dolava = true;
						gesto_doprava = false;
					}
				}
				if(abs(koordy[1] - koordy[3]) > 100.0)
				{
					if(koordy[1] < koordy[3])
					{
						gesto_dole = false;
						gesto_hore = true;
					}
					else
					{
						gesto_dole = true;
						gesto_hore = false;
					}
				}
				if(gesto_dolava) this->slidingHand_type = "scroll left";
				else if(gesto_doprava) this->slidingHand_type = "scroll right";
			}
		}
	}
}
