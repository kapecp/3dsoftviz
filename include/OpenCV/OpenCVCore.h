#ifndef OPENCVCORE_H
#define OPENCVCORE_H

#include <QPointer>

class QApplication;

namespace ArucoModul {
class ArucoThread;
}

namespace QOpenCV {
class FaceRecognitionThread;
class FaceRecognitionWindow;
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
		 * @author Autor: David Durcak
		 * @brief faceRecognition Initialize Face Detection and Aruco threads and Face Detection Window
		 */
	void faceRecognition();


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


	static OpenCVCore*	mOpenCVCore;	// static singleton instance
	bool				mThrsCreated;	// if false, threads were not initialized yet

	QWidget*							mParent;
	QApplication*					mApp;
	ArucoModul::ArucoThread*			mThrAruco;
	QOpenCV::FaceRecognitionThread*	mThrFaceRec;
	QPointer<QOpenCV::FaceRecognitionWindow> mOpencvDialog;

};
}

#endif // OPENCVCORE_H
