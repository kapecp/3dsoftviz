#include "Kinect/KinectRecognition.h"
#include "QDebug"

using namespace Kinect;

Kinect::KinectRecognition::KinectRecognition()
{
	mOpenniStatus=openni::OpenNI::initialize();
	if(mOpenniStatus != openni::STATUS_OK)
	{
		qDebug() << "Openni initialization failed";
	}
	mOpenniStatus=device.open(openni::ANY_DEVICE);
	if(mOpenniStatus != openni::STATUS_OK)
	{
		qDebug() << "Openni Device not Open";
	}

}

Kinect::KinectRecognition::~KinectRecognition()
{
	openni::OpenNI::shutdown();
}

cv::Mat Kinect::KinectRecognition::colorImage(openni::VideoFrameRef &normalFrame)
{
	openni::VideoMode videoMode = normalFrame.getVideoMode();
	cv::Mat frame = cv::Mat( videoMode.getResolutionY(),
					 videoMode.getResolutionX(),
					 CV_8UC3,
					 (short*) normalFrame.getData() );
	cv::cvtColor(frame,frame,CV_BGR2RGB);
return frame;
}

cv::Mat Kinect::KinectRecognition::depthImage(nite::HandTrackerFrameRef &handFrame)
{
	openni::VideoFrameRef colorFrame=handFrame.getDepthFrame();
	openni::VideoMode videoMode = colorFrame.getVideoMode();
	cv::Mat frame = cv::Mat( videoMode.getResolutionY(),
											 videoMode.getResolutionX(),
											 CV_16SC1,
											 (short*) colorFrame.getData() );

						frame.convertTo(frame, CV_8UC1, 255.0/10000);
						cv::cvtColor(frame, frame, CV_GRAY2BGR);
	return frame;


}
