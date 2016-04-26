#ifndef PROJECTIVE_AR_CORE_H
#define PROJECTIVE_AR_CORE_H

#include <QPointer>

class QApplication;

namespace QOSG {
//class FaceRecognitionThread;
class ProjectiveARWindow;
}

namespace QOSG {
/**
	*@brief Class OpenCVCore Controller for Face detection and Aruco functionality.
	*@author Autor: Marek Jakab, David Durcak
	*@date 18.11.2013
	*/
class ProjectiveARCore
{

public:

	/**
		 * @author Autor: Marek Jakab
		 * @brief getInstance Return instance of OpenCVCore class
		 * @param app QApplication
		 * @return OpenCVCore instance
		 */
    static ProjectiveARCore*	getInstance( QApplication* app, QWidget* parent );

	/**
		 * @author Autor: Marek Jakab
		 * @brief ~OpenCVCore Destructor, that Stops thread, close Window, free memory
		 */
    ~ProjectiveARCore( void );

	/**
		 * @author Autor: Michael Garaj
		 * @brief faceRecognition Initialize OpenCV Window for Aruco and Kinect
		 */
    void init(ViewerQT* sourceViewer);


private:
	/**
		 * @author Autor: David Durcak
		 * @brief OpenCVCore Costructor
		 * @param app QApplication
		 */
    ProjectiveARCore( QApplication* app, QWidget* parent );

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


    static ProjectiveARCore*	mProjectiveARCore;	// static singleton instance

    QWidget*                                mParent;
    QApplication*                           mApp;
    //QOpenCV::FaceRecognitionThread*         mThrFaceRec;
    QPointer<QOSG::ProjectiveARWindow>   mProjectiveARWindow;

};
}

#endif // PROJECTIVE_AR_CORE_H
