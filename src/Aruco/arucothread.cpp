#include "Aruco/arucothread.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Aruco/arucocore.h"

#include <QDebug>

using namespace ArucoModul;

ArucoThread::ArucoThread(void)
{
	mCancel		= false;
	mCorSetted	= false;
	mRatioCamCoef = 0;
	qRegisterMetaType< osg::Vec3d >("osgVec3d");
	qRegisterMetaType< osg::Quat >("osgQuat");
}

ArucoThread::~ArucoThread(void)
{
}

void ArucoThread::setCancel(bool set){
	mCancel	= set;
}

void ArucoThread::pause()
{
	mCancel = true;
}

void ArucoThread::run()
{
	double	actPosArray[3];		// x, y, z
	double	actQuatArray[4];		// angle(w), x, y, z
	bool	marerDetected = false;


	// this code will be changed soon
	cv::Mat frame;
	cv::VideoCapture capture;

	QMatrix4x4 mat;
	QString filename = "../share/3dsoftviz/config/camera.yml";

	// this must do camera singleton

	if( ! capture.isOpened()){
		capture.open(1);
	}
	const double width = 600;
	const double height = 500;
	const double camDistRatio = 2.75;
	capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);


	QFileInfo file(filename);
	if( ! file.exists() ){
		qDebug() << "File " << file.absoluteFilePath() << " does Not exist!";
	}
	ArucoCore aCore(filename);


	// compute correction translation vector and rotation quaternion
	//double xpos[3]	= {  0.00706000, -0.122270,  0.32559200 };
	//double xor[4]	= {  0.98069500, -0.194950, -0.00705001, -0.0135225 };
	double xpos[3]	= { -0.00709373, -0.124475,  0.33048100 };				// with ratio
	double xor[4]	= {  0.98163100, -0.188311, -0.02564880, -0.0168029 };	// with ratio

	//qDebug() << "pos  " << pos[0] << " " << pos[1] << " " << pos[2];
	//qDebug() << "ori  " << or[0] << " " << or[1] << " " << or[2] << " " << or[3];

	//computeCorQuatAndPos( xpos, xor);

	while(! mCancel) {	// doing aruco work in loop

		capture >> frame;		// get image from camera

		// add image to aruco and get position vector and rotation quaternion
		marerDetected = aCore.getDetectedPosAndQuat( frame, actPosArray, actQuatArray );
		if( marerDetected ){
			// test if marker was detect (if not, all number in matrix are not range)
			//if(mat.data()[ 0] > -100.0 && mat.data()[ 0] < 100.0  ){  ?????????????????



			osg::Quat  actQuat( actQuatArray[1], actQuatArray[2], actQuatArray[3], actQuatArray[0] );
			osg::Vec3d actPos( -actPosArray[0], -actPosArray[1], -actPosArray[2] );

			// correct Y centering, because of camerra different ration aruco top max y value is less than bottom one
			mRatioCamCoef = ( 1 - height/width ) / camDistRatio;
			actPos.y() = ( mRatioCamCoef * actPos.z()  + actPos.y() );




			//qDebug() << "1>>>>  " << actPos.x() << " " << actPos.y() << " " << actPos.z();

			//this->correctQuatAndPos( actPos, actQuat);

			//qDebug() << "2>>>>  " << actPos.x() << " " << actPos.y() << " " << actPos.z();

			emit sendArucoPosVec( actPos );
			emit sendArucoRorQuat( actQuat );

			//}  ?????
		}
		emit pushImage( aCore.getDetImage());	// emit image with marked marker for debuging
		if(! mCancel){
			msleep(50);
		}

	}
	capture.release();
}



void ArucoThread::computeCorQuatAndPos(const double position[3], const double rotation[4] ){
	// set corection translation
	mCorP.x() = -position[0];
	mCorP.y() = -position[1];
	mCorP.z() = -position[2];
	// set corection quaternion
	mCorQ.w() =  rotation[3];
	mCorQ.x() =  rotation[0];
	mCorQ.y() =  rotation[1];
	mCorQ.z() =  rotation[2];
	mCorQ = mCorQ.conj();
}

void ArucoThread::correctQuatAndPos( osg::Vec3d &actPos, osg::Quat &actQuat ) const{
	if( mCorSetted == true ){
		// correct position
		// rotate point around correction point(corP) =  translate, rotate and translate back

		actPos = (mCorQ * (actPos - mCorP)) + mCorP;

		// correct rotation
		actQuat = mCorQ * actQuat;
		//actQuat = actQuat * mCorQ ;
	} else {
		qDebug() << "ArucoThread:computeCorQautAndPos() was called before setted correction parameters before!";
	}
}

void ArucoThread::computeCorMat( QMatrix4x4 origM )
{

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
	mCorM = T2 * origM * T1;

	// !!! corection matrix must be on left in multiplication
	// corM * corrected matrix


	//printMat( T2,	 "Translation matrix" );
	//printMat( T1,	 "Inverse translation matrix" );
	//printMat( origM, "Rotation matrix" );
	//printMat( corM,	 "Corection Matrix" );

}


void ArucoThread::printVec( const osg::Vec3d v, const QString name)const
{
	qDebug() << name << " " << v.x() << " " << v.y() << " " << v.z();
}

void ArucoThread::printVec( const osg::Vec4d v, const QString name) const
{
	qDebug() << name << " " << v.x() << " " << v.y() << " " << v.z() << " " << v.w();
}

void ArucoThread::printMat( const osg::Matrixd mat, const QString name) const
{
	qDebug() << name;
	QString str;
	str  = " " + QString::number( mat(0,0), 'f', 2);
	str += " " + QString::number( mat(0,1), 'f', 2);
	str += " " + QString::number( mat(0,2), 'f', 2);
	str += " " + QString::number( mat(0,3), 'f', 2);
	qDebug() << ": " << str;
	str  = " " + QString::number( mat(1,0), 'f', 2);
	str += " " + QString::number( mat(1,1), 'f', 2);
	str += " " + QString::number( mat(1,2), 'f', 2);
	str += " " + QString::number( mat(1,3), 'f', 2);
	qDebug() << ": " << str;
	str  = " " + QString::number( mat(2,0), 'f', 2);
	str += " " + QString::number( mat(2,1), 'f', 2);
	str += " " + QString::number( mat(2,2), 'f', 2);
	str += " " + QString::number( mat(2,3), 'f', 2);
	qDebug() << ": " << str;
	str  = " " + QString::number( mat(3,0), 'f', 2);
	str += " " + QString::number( mat(3,1), 'f', 2);
	str += " " + QString::number( mat(3,2), 'f', 2);
	str += " " + QString::number( mat(3,3), 'f', 2);
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

/*
test(){

			// original, nemenny------------
			double xpos[3]	= {  0.0110871, -0.125618, 0.331737 };
			double xor[4]	= { -0.0172416, -0.018111, 0.9853, -0.168995 };
			QVector3D xposvk( xpos[0],  xpos[1],  xpos[2]);
			QVector3D xposvz(-xpos[0], -xpos[1], -xpos[2]);
			QVector4D xorv(   xor[0],   xor[1],   xor[2], xor[3] );
			QQuaternion xquatQ(xorv);
			QMatrix4x4 xrotM;
			xrotM.rotate(xquatQ);
			QMatrix4x4 xTk;
			xTk.translate(xposvk);
			xrotM = xTk * xrotM;
			///printMat(xrotM, "xrotM  --- original");
			// original matica podla ktrej sa bude robit

			// --------------------------
			// priprava aktualnej matice
			QVector3D posvk( pos[0],  pos[1],  pos[2]);
			QVector3D posvz(-pos[0], -pos[1], -pos[2]);
			///printVec(posvk, "aktual pos ");
			///qDebug() <<		"aktual quat " << or[0] << " " << or[1] << " " << or[2] << " " << or[3];
			QVector4D orv(   or[0],   or[1],   or[2], or[3] );

			QQuaternion quatQ(orv);
			QMatrix4x4 rotM;
			rotM.rotate(quatQ);
			QMatrix4x4 Tk;
			Tk.translate(posvk);
			rotM = Tk * rotM;
			printMat(rotM, "rotM  ---   aktual");



			// ----------------------------
			// vysledok podla matic
			QMatrix4x4 orig = xrotM;
			computeCorMat(orig);
			QMatrix4x4 resM1 = corM * rotM;
			//printMat(corM,  "corM   ---   1  medyivysledok");
			///printMat(resM1, "resM1  ---   1  vysledok, podla matic");
			// -----------------------
			//=======================================
			//=======================================

			//musim vediet vypocitat 2 veci bod posunuty
			// qatermion
			QQuaternion corQ;		// xquatQ, xposvk, xposvz

			corQ = xquatQ;
			//corQ.slerp();


			//------------------
			QMatrix4x4 corQM;
			corQM.rotate(corQ);
			//printMat(corQM, "corQM  ---  2 medyivysledok");
			//-------------------------------------


			QQuaternion resQ;		// corQ, posvk, posvz, quatQ
			QVector3D rposv;

			rposv = posvk - xposvk;
			printVec( rposv, "2 vysledok " );
			rposv = corQ.rotatedVector(rposv);
			//rposv = (corQM * QQuaternion(0, rposv) * corQM.conjugate()).vector();
			printVec( rposv, "2 vysledok " );
			rposv = rposv + xposvk;

			printVec( rposv, "2 vysledok " );

			resQ = corQ*quatQ;
			//resQ = quatQ*corQ;


			//------------------------------------

			QMatrix4x4 resQM;
			resQM.rotate(resQ);
			printMat(resM1, "resM1  ---   1  vysledok, podla matic");
			QMatrix4x4 rTk;
			rTk.translate(rposv);
			resQM = rTk * resQM;
			printMat(resQM, "resQM  ---   2 aktual pre qaternion");


}

*/
