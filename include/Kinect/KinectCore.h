#ifndef KinectCORE_H
#define KinectCORE_H

#include <QPointer>

namespace Kinect
{
class KinectThread;
class KinectWindow;
}

namespace Kinect
{

/**
	*@brief Class KinectCore
	*@author Autor: Patrik Hlavac
	*@date 4.3.2014
	*/
class KinectCore
{

public:

	~KinectCore(void);
	/**
		 * @author Autor: Patrik Hlavac
		 * @brief getInstance Return instance of KinectCore class
		 * @param app QApplication
		 * @return KinectCore instance
		 */
	static KinectCore *getInstance(QApplication* app, QWidget *parent);


	/**
	 * @author Matej Marconak
	 *@brief kinectRecognition start
	 */
	void kinectRecognition();

private:

	/**
	  *author Matej Marconak
	 * @brief create Connection Kinect
	 */
	void createConnectionKinect();

	KinectCore(QApplication* app, QWidget *parent);

	static KinectCore *mKinectCore;

	QApplication *app;
	QWidget	*mParent;

	Kinect::KinectThread *mThrKinect;
	bool	mThrsCreated;

	QPointer<Kinect::KinectWindow> mKinectDialog;





};
}

#endif // KinectCORE_H
