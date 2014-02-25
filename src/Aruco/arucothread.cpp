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
	cv::VideoCapture capture;

	QMatrix4x4 mat;
	QString filename = "../share/3dsoftviz/config/camera.yml";

	// this must do camera singleton

	if( ! capture.isOpened()){
		capture.open(1);
	}
	capture.set(CV_CAP_PROP_FRAME_WIDTH, 600);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, 500);


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
		if(mat.data()[ 0] > -100.0 && mat.data()[ 0] < 100.0  ){
			//printMat(mat, "ArucoMatrix");
			emit pushArucoMVMat( mat );			// emit matrix to CameraManipulation
		}

		emit pushImage( aCore.getDetImage());	// emit image with marked marker for debuging

		msleep(50);

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


void ArucoThread::computeCorMat( QMatrix4x4 origM ){

	QMatrix4x4 T1, T2;

	// get translation's vectors
	QVector3D vTran2(  origM(0,3),  origM(1,3),	 origM(2,3) );
	QVector3D vTran1( -origM(0,3), -origM(1,3), -origM(2,3) );
	// create translation matrixes
	T1.translate( vTran1 );					//printMat(T1, "T1");
	T2.translate( vTran2 );					//printMat(T2, "T2");

	// delete translation info -> make rotation matrix
	origM(0,3) = 0.0;
	origM(1,3) = 0.0;
	origM(2,3) = 0.0;

	// compute corection matrix
	corM = T2 * origM * T1;

	// !!! corection matrix must be on left in multiplication
	// corM * corrected matrix


	//printMat( T2,	 "Translation matrix" );
	//printMat( T1,	 "Inverse translation matrix" );
	//printMat( origM, "Rotation matrix" );
	//printMat( corM,	 "Corection Matrix" );

}


void ArucoThread::printVec( const QVector3D v, const QString name){
	qDebug() << name << " " << v.x() << " " << v.y() << " " << v.z();
}


void ArucoThread::printMat( const QMatrix4x4 mat, const QString name){
	qDebug() << name;
	QString str;
	str  = " " + QString::number(mat.data()[ 0], 'f', 2);
	str += " " + QString::number(mat.data()[ 1], 'f', 2);
	str += " " + QString::number(mat.data()[2], 'f', 2);
	str += " " + QString::number(mat.data()[3], 'f', 2);
	qDebug() << ": " << str;
	str  = " " + QString::number(mat.data()[ 4], 'f', 2);
	str += " " + QString::number(mat.data()[ 5], 'f', 2);
	str += " " + QString::number(mat.data()[6], 'f', 2);
	str += " " + QString::number(mat.data()[7], 'f', 2);
	qDebug() << ": " << str;
	str  = " " + QString::number(mat.data()[ 8], 'f', 2);
	str += " " + QString::number(mat.data()[ 9], 'f', 2);
	str += " " + QString::number(mat.data()[10], 'f', 2);
	str += " " + QString::number(mat.data()[11], 'f', 2);
	qDebug() << ": " << str;
	str  = " " + QString::number(mat.data()[ 12], 'f', 2);
	str += " " + QString::number(mat.data()[ 13], 'f', 2);
	str += " " + QString::number(mat.data()[14], 'f', 2);
	str += " " + QString::number(mat.data()[15], 'f', 2);
	qDebug() << ": " << str;
}



/*
test(){
	QMatrix4x4 origM( 1.00,  0.00,  0.00, -0.04,
					  0.00,	-0.94,  0.33,  0.22,
					  0.00,	 0.33,	0.94, -0.55,
					  0.00,	 0.00,  0.00,  1.00 );

	QMatrix4x4 mat( 1.00,  0.00,  0.00, -0.54,
					0.00, -0.94,  0.33,  0.00,
					0.00,  0.33,  0.94, -1.10,
					0.00,  0.00,  0.00,  1.00 );

	QMatrix4x4 res1( 1.00, 0.00, 0.00, -0.54,
					 0.00, 0.99, 0.00,  0.25,
					 0.00, 0.00, 0.99, -1.14,
					 0.00, 0.00, 0.00,  1.00 );

	computeCorMat(origM);
	QMatrix4x4 res2 = corM * mat;

	qDebug( res1 == res2 );
}
*/
