#ifndef KinectCORE_H
#define KinectCORE_H

#include "Core/Core.h"
#include <QPointer>
#include "Kinect/KinectThread.h"
#include "Kinect/KinectWindow.h"

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
	KinectCore(QApplication* app, QWidget *parent);
	~KinectCore(void);
	/**
		 * @author Autor: Patrik Hlavac
		 * @brief getInstance Return instance of KinectCore class
		 * @param app QApplication
		 * @return KinectCore instance
		 */
	static KinectCore *getInstance(QApplication* app, QWidget *parent);

	static KinectCore *mKinectCore;

	/**
	 * @author Matej Marconak
	 *@brief kinectRecognition start
	 */
	void kinectRecognition();

private:

	void createConnectionKinect();

	QApplication *app;
	QWidget	*mParent;
	Kinect::KinectThread *mThrKinect;
	QPointer<Kinect::KinectWindow> mKinectDialog;

	bool	mThrsCreated;



};
}

#endif // KinectCORE_H
