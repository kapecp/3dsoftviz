#include "Kinect/KinectHandTracker.h"

#include "Core/Core.h"
#include "QDebug"

Kinect::KinectHandTracker::KinectHandTracker(openni::Device *device)
{
	m_pHandTracker.create(device);
	m_pHandTracker.startGestureDetection(nite::GESTURE_WAVE);
	m_pHandTracker.startGestureDetection(nite::GESTURE_CLICK);
	isClick=false;
	viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
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
			if(gestures[i].getType()==nite::GESTURE_CLICK)
			{
				if(isClick)
				{
					//TODO real click and release
					isClick=false;
					printf("Release\n");
					printf("at %d a %d\n",viewer->cursor().pos().x(),viewer->cursor().pos().y());
				}
				else
				{
					isClick=true;
					printf("Click");
					printf("at %d a %d\n",viewer->cursor().pos().x(),viewer->cursor().pos().y());
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
			printf("Lost hand %d\n", user.getId());
			nite::HandId id = user.getId();
			HistoryBuffer<20>* pHistory = this->g_histories[id];
			this->g_histories.erase(this->g_histories.find(id));
			delete pHistory;
		}
		else
		{
			if (user.isNew())
			{
				printf("New hand id %d\n", user.getId());
				this->g_histories[user.getId()] = new HistoryBuffer<20>;
			}
			// Hand evidence in Buffer
			HistoryBuffer<20>* pHistory = this->g_histories[user.getId()];
			pHistory->AddPoint(user.getPosition());
			// Data for mouse
			printf("user %d %.2lf %.2lf\n", user.getId(), user.getPosition().x, user.getPosition().y);
			this->handX = user.getPosition().x;
			//revert Y position
			//TODO test
			this->handY = 0-user.getPosition().y;
		}
	}
}
