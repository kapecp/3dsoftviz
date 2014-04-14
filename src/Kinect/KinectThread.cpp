
#include "Kinect/KinectThread.h"
#include "Kinect/KinectCore.h"
#include "Kinect/KinectRecognition.h"
#include "Kinect/KinectHandTracker.h"

#include "QDebug"

using namespace Kinect;

using namespace cv;

Kinect::KinectThread::KinectThread(QObject *parent) : QThread(parent)
{
	mCancel=false;
	mSpeed=1.0;
	isCursorEnable=true;
	isOpen=false;
	mSetImageEnable=true;
}

Kinect::KinectThread::~KinectThread(void)
{
}

void Kinect::KinectThread::setCancel(bool set)
{
	mCancel=set;
}

void Kinect::KinectThread::setImageSend(bool set)
{
	mSetImageEnable=set;
}

void Kinect::KinectThread::pause()
{
	mCancel=true;
}

void Kinect::KinectThread::setCursorMovement(bool set)
{
	isCursorEnable=set;
}
void Kinect::KinectThread::setSpeedKinect(double set)
{
	mSpeed=set;
}

// SIGNAL 1
//void Kinect::KinectThread::sendSliderCoords(float _t1, float _t2, float _t3)
//{
//	void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
//	QMetaObject::activate(this, &staticMetaObject, 1, _a);
//}

void Kinect::KinectThread::run()
{

	mCancel=false;
	/////Class kinect recognition
	mKinect = new Kinect::KinectRecognition();
	isOpen=mKinect->isOpenOpenni();
	openni::VideoStream  color;
	openni::VideoStream  m_depth;
	openni::CoordinateConverter coordinateConverter;
	// convert milimeters to pixels
	float pDepth_x;
	float pDepth_y;
	float pDepth_z;
	float pDepth_x2;
	float pDepth_y2;
	float pDepth_z2;
	/////////end////////////

	cv::Mat frame;

	//////////////Kinect start color and Hand tracking ///////
	if(isOpen)
	{
		color.create(mKinect->device, openni::SENSOR_COLOR);
		color.start();
		m_depth.create(mKinect->device, openni::SENSOR_DEPTH);
		m_depth.start();

		kht = new KinectHandTracker(&mKinect->device,&m_depth);
	}


	while(!mCancel && isOpen)
	{
		if(mSetImageEnable)
		{
			color.readFrame(&colorFrame);
			frame=mKinect->colorImageCvMat(colorFrame);

			//set parameters for changes movement and cursor
			kht->setCursorMovement(isCursorEnable);
			kht->setSpeedMovement(mSpeed);
			// cita handframe, najde gesto na snimke a vytvori mu "profil"
			kht->getAllGestures();
			kht->getAllHands();
			//////////////End/////////////

			//	cap >> frame; // get a new frame from camera
			cv::cvtColor(frame, frame, CV_BGR2RGB);

			if (kht->isTwoHands == true)
			{
				coordinateConverter.convertWorldToDepth(m_depth, kht->getArrayHands[0][0], kht->getArrayHands[0][1], kht->handZ[0], &pDepth_x, &pDepth_y, &pDepth_z);
				coordinateConverter.convertWorldToDepth(m_depth, kht->getArrayHands[1][0], kht->getArrayHands[1][1], kht->handZ[1], &pDepth_x2, &pDepth_y2, &pDepth_z2);

				pDepth_y = kht->handTrackerFrame.getDepthFrame().getHeight() - pDepth_y;
				pDepth_y2 = kht->handTrackerFrame.getDepthFrame().getHeight() - pDepth_y2;

				printf("depth X, Y, Z: %f %f %f\n",pDepth_x,pDepth_y,pDepth_z);

				Rect hand_rect;

				if (pDepth_x < pDepth_x2) hand_rect.x = pDepth_x;
				else hand_rect.x = pDepth_x2;
				if (pDepth_y < pDepth_y2) hand_rect.y = pDepth_y;
				else hand_rect.y = pDepth_y2;

				hand_rect.width = abs(pDepth_x - pDepth_x2);
				hand_rect.height = abs(pDepth_y - pDepth_y2);//kht->handY[1] - kht->handY[0];

				rectangle(frame, hand_rect, CV_RGB(0, 255,0), 3);
			}

			//sliding
			kht->getRotatingMove();
			line(frame, Point2i( 30, 30), Point2i( 30, 30), Scalar( 0, 0, 0 ), 5 ,8 );

			char * text;
			text = kht->slidingHand_type;
			if((int)kht->slidingHand_x != 0){
				putText(frame, text, cvPoint((int)kht->slidingHand_x,(int)kht->slidingHand_y), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(0,0,250), 1, CV_AA);
				//signal pre
				emit sendSliderCoords(  (kht->slidingHand_x/kht->handTrackerFrame.getDepthFrame().getWidth()-0.5)*200,
										(kht->slidingHand_y/kht->handTrackerFrame.getDepthFrame().getHeight()-0.5)*200,
										(kht->slidingHand_z/kht->handTrackerFrame.getDepthFrame().getHeight()-0.5)*200);
				printf("%.2lf %.2lf z %.2lf -  %.2lf slider \n", (kht->slidingHand_x/kht->handTrackerFrame.getDepthFrame().getWidth()-0.5)*200,
					   (kht->slidingHand_y/kht->handTrackerFrame.getDepthFrame().getHeight()-0.5)*200, (kht->slidingHand_z/kht->handTrackerFrame.getDepthFrame().getHeight()-0.5)*200, kht->slidingHand_z);
			}

			cv::resize(frame, frame,cv::Size(320,240),0,0,cv::INTER_LINEAR);
			emit pushImage( frame );
			msleep(20);
		}

	}
	//delete(mKinect);
	//delete(this);
}
