#ifndef KINECTTHREAD_H
#define KINECTTHREAD_H

#include <QThread>
#include <QImage>
#include <QTimer>

#ifdef OPENNI2_FOUND
#include "OpenNI.h"
#endif

#ifdef NITE2_FOUND
#include "NiTE.h"
#endif

#include <opencv2/core/core.hpp>
#include "Viewer/GraphNavigation.h"
#include "Viewer/MouseControl.h"
#include "OpenCV/MarkerlessTracker.h"

namespace Kinect {

class KinectThread;
class KinectHandTracker;
class KinectRecognition;

/**
 * @author Matej Marconak
 * @brief The KinectThread class
 * @brief Thread for processing Openni and Nite data
 */
class KinectThread : public QThread
{
	Q_OBJECT

public:

	explicit KinectThread( QObject* parent=0 );

	~KinectThread( void );

	void run();

signals:
	// Marak start
	void signalClickTimerStart();
	void signalClickTimerStop();
	// Marak end
	/**
	 * @brief send coordinate for controlling graph
	 * @param x x coordinate of hand
	 * @param y y coordinate of hand
	 * @param distance distance from Kinect
	 */
	void sendSliderCoords( float x, float y, float distance );

	/**
	 * @brief send picture for OpenCV window
	 * @param image picture from Kinect
	 */
	void pushImage( cv::Mat image );

	/**
	 * @brief send picture to Aruco Marker Detection
	 * @param image picture from Kinect
	 */
	void pushImageToMarkerDetection( cv::Mat image );


	/**
	 * @brief send picture for Kinect window
	 * @param image picture from Kinect
	 */
	void pushImageQ( QImage qimage );

public slots:
	// Marak start
	/**
	 *	\fn public timerTimeout
	 *	\brief called when timer for removeLastSelection gesture times out
	 */
	void clickTimerStart();
	void clickTimerStop();
	void clickTimerTimeout();
	// Marak end
	/**
	 * @brief start funkcionality Kinect thread
	 * @param set true if start, false for stop
	 */
	void setCancel( bool set );

	/**
	 * @brief function for start,pause calculate HandTracking
	 * @param set true for calculate and sending picture, false for pause
	 */
	void setImageSend( bool set );

	/**
	 * @brief function for enable sending picture to Aruco thread for Marker Detection
	 * @param set true for sending picture, false for pause
	 */
	void setImageSendToMarkerDetection( bool set );

	void pause();

	/**
	 * @brief control for mouse movement
	 * @param set true for start,false for stop
	 */
	void setCursorMovement( bool set );

	/**
	 * @brief control for graph zoom
	 * @param set true for start,false for stop
	 */
	void setZoomUpdate( bool set );

	/**
	 * @brief speed of reaction from Kinect gesture
	 * @param set base 1.0 for normal speed, <1 slower, >1 faster
	 */
	void setSpeedKinect( double set );

	/**
	 * @brief Perform markerless tracking on image (true/false)
	 * @param set
	 */
	void setMarkerlessTracking( bool set );

	/**
	 * @brief inicialize openni and nite for handtracking and add device
	 */
	void inicializeKinect();

	/**
	 * @brief pause,stop calculate
	 */
	void closeActionOpenni();

	void setCaptureImage( bool set );

private:

	bool captureImage;

	// Marak start
	QTimer* clickTimer;
	bool clickTimerFirstRun;
	Vwr::GraphNavigation* nav;
	Vwr::MouseControl* mouse;
	OpenCV::MarkerlessTracker* kTracker;
	// Marak end
	/**
	 * @brief information about status thread
	 */
	bool mCancel;

	/**
	 * @brief status for sending image and processing
	 */
	bool mSetImageEnable;

	/**
	 * @brief flag for enabling markerless tracking
	 */
	bool mMarkerlessTrackingEnabled;

	/**
	 * @brief status of OpenNI device
	 */
	bool isOpen;

	/**
	 * @brief status of cursor
	 */
	bool isCursorEnable;

	/**
	 * @brief isZoomEnable status of zoom
	 */
	bool isZoomEnable;

	/**
	 * @brief isMarkerDetectEnable status of marker detection
	 */
	bool isMarkerDetectEnable;

	/**
	 * @brief speed for reaction
	 */
	double mSpeed;

#ifdef OPENNI2_FOUND
	/**
	 * @brief base colorframe enity for save data
	 */
	openni::VideoFrameRef colorFrame;

	/**
	 * @brief depthFrame depthframe entity for save data
	 */
	openni::VideoFrameRef depthFrame;
#endif

	/**
	 * @brief base class for Hand Recognition
	 */

#ifdef NITE2_FOUND
	KinectHandTracker* kht;
#endif


	/**
	 * @brief base class for open Kinect a converted
	 */
	KinectRecognition* mKinect;
#ifdef OPENNI2_FOUND
	/**
	 * @brief video stream data for save
	 */
	openni::VideoStream  color;

	/**
	 * @brief video stream data for save
	 */
	openni::VideoStream  m_depth;
#endif
};
}

#endif // KINECTTHREAD_H
