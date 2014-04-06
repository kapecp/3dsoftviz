#ifndef KINECTTHREAD_H
#define KINECTTHREAD_H

#include <QThread>
#include <QImage>

#include "OpenNI.h"
#include "NiTE.h"

#include <opencv2/core/core.hpp>

namespace Kinect{

	class KinectThread;
	class KinectHandTracker;
	class KinectRecognition;

class KinectThread : public QThread
{
Q_OBJECT

public:

	KinectThread(QObject *parent=0);

	~KinectThread(void);

	void run();


signals:

	void sendSliderCoords( float x, float y, float distance);

	void pushImage(cv::Mat image);

	void pushImageQ(QImage qimage);

public slots:

	void setCancel(bool set);

	void setImageSend(bool set);

	void pause();

	void setCursorMovement(bool set);

	void setSpeedKinect(double set);

private:

	bool mCancel;
	bool mSetImageEnable;
	bool isOpen;
	bool isCursorEnable;

	double mSpeed;

	openni::VideoFrameRef colorFrame;
	KinectHandTracker *kht;
	KinectRecognition *mKinect;


};
}

#endif // KINECTTHREAD_H
