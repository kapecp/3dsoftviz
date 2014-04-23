#include "Aruco/arucothread.h"
#include "Aruco/arucocore.h"
#include "Util/ApplicationConfig.h"
#include <QDebug>

#include "OpenCV/CapVideo.h"

using namespace ArucoModul;

ArucoThread::ArucoThread(QObject *parent)
	: QThread(parent)
{
	mCapVideo		= NULL;
	mCancel			= false;
	mCorSetted		= false;
	mMarkerIsBehind = false; // marker is default in front of projection or monitor
	mCorEnabled		= false;
	mUpdCorPar		= false;
	mSendImgEnabled	= true;
	mSendBackgrImgEnabled = false;
	mRatioCamCoef	= 0;

	qRegisterMetaType< osg::Vec3d >("osgVec3d");
	qRegisterMetaType< osg::Quat >("osgQuat");
}

ArucoThread::~ArucoThread(void)
{
}

void ArucoThread::setCancel(bool set)
{
	mCancel	= set;
}

void ArucoThread::pause()
{
	mCancel = true;
}

void ArucoThread::setPositionOfMarker( bool behind )
{
	mMarkerIsBehind = behind;
}

void ArucoThread::setCorEnabling( bool corEnabled )
{
	mCorEnabled = corEnabled;
}

void ArucoThread::setSendImgEnabling( bool sendImgEnabled )
{
	mSendImgEnabled = sendImgEnabled;
}

void ArucoThread::setSendBackgrImgEnabled( bool sendBackgrImgEnabled )
{
	mSendBackgrImgEnabled = sendBackgrImgEnabled;
}

void ArucoThread::updateCorectionPar(){
	mUpdCorPar = true;
}

void ArucoThread::setCapVideo( OpenCV::CapVideo *capVideo){
	mCapVideo = capVideo;
}

void ArucoThread::run()
{
	cv::Mat frame;
	mCancel = false;

	if( mCapVideo == NULL){
		qDebug() << "[ArucoThread::run()]  Camera is not set";
		return;
	}
	const double width  = mCapVideo->getWidth();
	const double height = mCapVideo->getHeight();
	const double camDistRatio  = Util::ApplicationConfig::get()->getValue("Aruco.CamDistancRatio").toDouble();
	mRatioCamCoef = ( 1 - height/width ) / camDistRatio;


	double		 actPosArray[3];			// x, y, z
	double		 actQuatArray[4];		// angle(w), x, y, z
	bool		 markerDetected = false;


	QString filename = "../share/3dsoftviz/config/camera.yml";
	QFileInfo file(filename);
	if( ! file.exists() ){
		qDebug() << "File " << file.absoluteFilePath() << " does Not exist!";
		return;
	}

	// create Aruco, set camera pararameters and check it
	ArucoCore aCore;
	bool camParametersOk = aCore.setCameraParameters( filename );

	if( camParametersOk ){
		while(! mCancel) {	// doing aruco work in loop

			frame = mCapVideo->queryFrame();		// get image from camera

			// add image to aruco and get position vector and rotation quaternion
			markerDetected = aCore.getDetectedPosAndQuat( frame, actPosArray, actQuatArray );
			if( markerDetected ){

				// test if marker was detect (if not, all number in matrix are not range)
				if( actPosArray[2] > 0.0  &&  actPosArray[2] < 10.0
						&&   actQuatArray[0] >= -1.0  &&  actQuatArray[0] <= 1.0 ){



					// can be corection parameters updated
					if( mUpdCorPar ){
						computeCorQuatAndPos( actPosArray, actQuatArray );
					}

					osg::Vec3d actPos( -actPosArray[0], -actPosArray[1], -actPosArray[2] );
					osg::Quat  actQuat;


					//  forward/backward,   left/right,  around,   w
					if( mMarkerIsBehind ){
						actQuat.set( -actQuatArray[1],  actQuatArray[3],  actQuatArray[2],  actQuatArray[0] );
					} else {
						actQuat.set(  actQuatArray[1], -actQuatArray[3],  actQuatArray[2],  actQuatArray[0] );
					}


					// correct Y centering, because of camerra different ration aruco top max y value is less than bottom one
					actPos.y() = ( mRatioCamCoef * actPos.z()  + actPos.y() );


					if ( mCorEnabled ) {
						correctQuatAndPos( actPos, actQuat);
					}

					emit sendArucoPosVec( actPos );
					emit sendArucoRorQuat( actQuat );

				}
			}

			cv::Mat image = aCore.getDetImage();

			cv::cvtColor( image, image, CV_BGR2RGB );
			if ( mSendImgEnabled ) {

				QImage qimage ( (uchar*) image.data, image.cols, image.rows,(int) image.step, QImage::Format_RGB888);

				emit pushImage( qimage );	// emit image with marked marker for debuging
			}

			if( mSendBackgrImgEnabled && !image.empty() ){
				emit pushBackgrImage( image.clone() );
			}

			if(! mCancel){
				msleep(50);
			}

		}
	}

	mCapVideo->release();
	mCapVideo = NULL;
}



void ArucoThread::computeCorQuatAndPos(const double position[3], const double rotation[4] ){
	qDebug() << "ARUCO: comput cor par done>";
	// set corection translation
	mCorP.x() = -position[0];
	mCorP.y() = -position[1];
	mCorP.z() = -position[2];
	// set corection quaternion
	osg::Quat tmp(rotation[1], rotation[2], rotation[3], rotation[0] );
	mCorQ = tmp.conj();

	mCorSetted = true;
	mUpdCorPar = false;
	emit corParUpdated();	// emit that corection parameters were updated
}

void ArucoThread::correctQuatAndPos( osg::Vec3d &actPos, osg::Quat &actQuat ) const{
	if( mCorSetted == true ){
		// correct position
		// rotate point around correction point(corP) =  translate, rotate and translate back
		actPos = ( mCorQ * (actPos - mCorP)) + mCorP;

		// correct rotation
		actQuat = actQuat * mCorQ;

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
