#include "Aruco/arucothread.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Aruco/arucocore.h"

#include <QDebug>

using namespace ArucoModul;

ArucoThread::ArucoThread(void)
{
	cancel=false;
}

ArucoThread::~ArucoThread(void)
{
}



void ArucoThread::run()
{
	// this code will be changed soon
	cv::Mat frame;
	//CvCapture *capture;
	cv::VideoCapture capture;

	QMatrix4x4 mat;
	QString filename = "../share/3dsoftviz/config/camera.yml";

	// this must do camera singleton

	if( ! capture.isOpened()){
		capture.open(1);
	}
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 400);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 300);


	QFileInfo file(filename);
	if( ! file.exists() ){
		qDebug() << "File " << file.absoluteFilePath() << " does Not exist!";
	}
	ArucoCore aCore(filename);


	while(!cancel) {	// doing aruco work in loop

		capture >> frame;		// get image from camera

		// add image to aruco and get matrix
		mat = aCore.getDetectedMatrix(frame);

		// test if marker was detect (if not, all number in matrix are not range)
		if(mat.data()[ 0] > -1.0 && mat.data()[ 0] < 1.0  ){
			emit pushArucoMVMat( mat );			// emit matrix to CameraManipulation
		}

		emit pushImage( aCore.getDetImage());	// emit image with marked marker for debuging

		msleep(200);

	}
	capture.release();
}


void ArucoThread::setCancel(bool set){
	this->cancel=set;
}

void ArucoThread::pause()
{
	this->cancel=true;
}


