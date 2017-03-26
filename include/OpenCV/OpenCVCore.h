#ifndef OPENCVCORE_H
#define OPENCVCORE_H

#include <QPointer>

class QApplication;

namespace ArucoModul {
class ArucoThread;
}

namespace Kinect {
class KinectThread;
}

namespace QOpenCV {
class FaceRecognitionThread;
//class FaceRecognitionWindow;
class OpenCVWindow;
}
namespace OpenCV {
class FaceRecognizer;
}

namespace OpenCV {
/**
	*@brief Class OpenCVCore Controller for Face detection and Aruco functionality.
	*@author Autor: Marek Jakab, David Durcak
	*@date 18.11.2013
	*/
class OpenCVCore
{

public:

	/**
		 * @author Autor: Marek Jakab
		 * @brief getInstance Return instance of OpenCVCore class
		 * @param app QApplication
		 * @return OpenCVCore instance
		 */
	static OpenCVCore*	getInstance( QApplication* app, QWidget* parent );

	/**
		 * @author Autor: Marek Jakab
		 * @brief ~OpenCVCore Destructor, that Stops thread, close Window, free memory
		 */
	~OpenCVCore( void );

	/**
		 * @author Autor: Michael Garaj
		 * @brief faceRecognition Initialize OpenCV Window for Aruco and Kinect
		 */
	void opencvInit();


private:
	/**
		 * @author Autor: David Durcak
		 * @brief OpenCVCore Costructor
		 * @param app QApplication
		 */
	OpenCVCore( QApplication* app, QWidget* parent );

	/**
		 * @author Autor: David Durcak
		 * @brief createPermanentConnection Create conections for sending results from threads to Camera Manipulator and CoreGraph
		 */
	void createPermanentConnection();

	/**
		 * @author Autor: David Durcak
		 * @brief createConnectionFaceRec Create conections for cotrollig Face Detection thread from Face Detection window
		 */
	void createConnectionFaceRec();

	/**
		 * @author Autor: David Durcak
		 * @brief createConnectionAruco Create conections for cotrollig Aruco thread from Face Detection window
		 */
	void createConnectionAruco();

	/**
		 * @author Autor: Michael Garaj
		 * @brief createConnectionAruco Create conections for cotrollig Aruco thread from OpenCVWindow
	     */
	void createConnectionMultiAruco();

	/**
		 * @author Matej Marconak
		 * @brief create Connection Kinect
		 * @brief create SLOT and SIGNAL for comunication between THREAD and Window,CameraManipulator
		 */
	void createConnectionKinect();


	static OpenCVCore*	mOpenCVCore;	// static singleton instance
	bool				mArucoThrsCreated;	// if false, threads were not initialized yet
	bool				mKinectThrsCreated; // if false, threads were not initialized yet

	QWidget*							mParent;
	QApplication*					mApp;
	ArucoModul::ArucoThread*			mThrAruco;
	QOpenCV::FaceRecognitionThread*	mThrFaceRec;
	Kinect::KinectThread* mThrKinect;
	//QPointer<QOpenCV::FaceRecognitionWindow> mOpencvDialog;
	QPointer<QOpenCV::OpenCVWindow> mOpencvWindow;

};
}

#endif // OPENCVCORE_H
