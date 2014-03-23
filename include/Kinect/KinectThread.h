#ifndef KINECTTHREAD_H
#define KINECTTHREAD_H

#include <QThread>
#include <QImage>


#include <opencv2/core/core.hpp>

namespace Kinect{

class KinectThread : public QThread
{
Q_OBJECT

public:

	KinectThread(QObject *parent=0);

	~KinectThread(void);

	void run();


signals:

	void pushImage(cv::Mat image);

	void pushImageQ(QImage qimage);

public slots:

	void setCancel(bool set);

	void setImageSend(bool set);

	void pause();

private:

	bool mCancel;
	bool mSetImageEnable;



};





}

#endif // KINECTTHREAD_H
