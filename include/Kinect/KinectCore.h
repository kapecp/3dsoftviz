#ifndef KinectCORE_H
#define KinectCORE_H

#include <QPointer>

namespace Kinect {
class KinectThread;
class KinectWindow;
}

namespace QOpenCV {
class OpenCVWindow;
}

namespace Kinect {

/**
	* @brief Class KinectCore
	* @author Autor: Patrik Hlavac
	* @date 4.3.2014
	* @brief Core class for create Window a Thread Kinect, here is create connection between Window and Thread based on SLOT a SIGNAL
	*/
class KinectCore
{

public:

	~KinectCore( void );
	/**
		 * @author Autor: Patrik Hlavac
		 * @brief getInstance Return instance of KinectCore class
		 * @param app QApplication
		 * @return KinectCore instance
		 */
	static KinectCore* getInstance( QApplication* app, QWidget* parent );

private:

	/**
	 * @author Matej Marconak
	 * @brief create Connection Kinect
	 * @brief create SLOT and SIGNAL for comunication between THREAD and Window,CameraManipulator
	 */
	void createConnectionKinect();

	/**
	 * @brief KinectCore
	 * @param app link to QApplication
	 * @param parent link to parent QWidget
	 * @brief Singleton
	 */
	KinectCore( QApplication* app, QWidget* parent );

	/**
	 * @brief Singleton Kinect core
	 */
	static KinectCore* mKinectCore;

	/**
	 * @brief link to Qaplication
	 */
	QApplication* app;

	/**
	 * @brief link to QWidget parent
	 */
	QWidget*	mParent;

	/**
	 * @brief Kinect thread
	 */
	Kinect::KinectThread* mThrKinect;

	/**
	 * @brief is Thread created
	 */
	bool	mThrsCreated;

	/**
	 * @brief OpenCV Window
	 */
	QPointer<QOpenCV::OpenCVWindow> mOpencvWindow;

};
}

#endif // KinectCORE_H
