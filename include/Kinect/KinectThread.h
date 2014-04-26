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

	/**
	 * @brief inicialize openni and nite for handtracking and add device
	 */
	bool inicializeKinect();

	/**
	 * @brief pause,stop calculate
	 */
	void closeActionOpenni();

signals:

	/**
	 * @brief send coordinate for controlling graph
	 * @param x x coordinate of hand
	 * @param y y coordinate of hand
	 * @param distance distance from Kinect
	 */
	void sendSliderCoords( float x, float y, float distance);

	/**
	 * @brief send picture for Kinect window
	 * @param image picture from Kinect
	 */
	void pushImage(cv::Mat image);


	/**
	 * @brief send picture for Kinect window
	 * @param image picture from Kinect
	 */
	void pushImageQ(QImage qimage);

public slots:

	/**
	 * @brief start funkcionality Kinect thread
	 * @param set true if start, false for stop
	 */
	void setCancel(bool set);

	/**
	 * @brief function for start,pause calculate HandTracking
	 * @param set true for calculate and sending picture, false for pause
	 */
	void setImageSend(bool set);

	void pause();

	/**
	 * @brief control for mouse movement
	 * @param set true for start,false for stop
	 */
	void setCursorMovement(bool set);

	/**
	 * @brief speed of reaction from Kinect gesture
	 * @param set base 1.0 for normal speed, <1 slower, >1 faster
	 */
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

	openni::VideoStream  color;
	openni::VideoStream  m_depth;


};
}

#endif // KINECTTHREAD_H
