#include "Aruco/ArucoThread.h"
#include "Aruco/ArucoCore.h"
#include "Aruco/arControlObject.h"
#include "Util/ApplicationConfig.h"

#include <QDebug>
#include <QFileInfo>

#include <opencv2/imgproc/imgproc.hpp>

#include "OpenCV/MarkerlessTracker.h"
#include "OpenCV/CapVideo.h"
#include <cv.h>

namespace ArucoModul {

ArucoThread::ArucoThread( QObject* parent )
	: QThread( parent )
{
	mCapVideo		= NULL;
	mCancel			= false;
	mCorSetted		= false;
	mMarkerIsBehind = false; // marker is default in front of projection or monitor
	mCorEnabled		= false;
	mUpdCorPar		= false;
	mSendImgEnabled	= true;
	mSendBackgrImgEnabled = false;
	mMarkerlessTrackingEnabled = false;
	//JMA
	mMultiMarkerEnabled = true;
	mRatioCamCoef	= 0;
	mCamDistRatio = 0;
	mHalfRatioCoef = 0;
	mGrM			= 0;
	mMoM			= 1;
	boolQueue = new Util::SizedQueue( 5, 0.0 );

	//JMA
	mArControlClass = new ArControlClass();
	mMarkerlessTracker = new OpenCV::MarkerlessTracker();
}

ArucoThread::~ArucoThread( void )
{

}

void ArucoThread::setCancel( bool set )
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

void ArucoThread::setMultiMarker( bool set )
{
	mMultiMarkerEnabled = set;
	qDebug() << mMultiMarkerEnabled;
}

void ArucoThread::setMarkerlessTracking( bool set )
{
	mMarkerlessTrackingEnabled = set;
}

void ArucoThread::setSendImgEnabling( bool sendImgEnabled )
{
	mSendImgEnabled = sendImgEnabled;
}

void ArucoThread::setSendBackgrImgEnabled( bool sendBackgrImgEnabled )
{
	mSendBackgrImgEnabled = sendBackgrImgEnabled;
}

void ArucoThread::updateCorectionPar()
{
	mUpdCorPar = true;
}

void ArucoThread::setCapVideo( OpenCV::CapVideo* capVideo )
{
	mCapVideo = capVideo;
}

void ArucoThread::interchangeMarkers()
{
	unsigned int aux = mGrM;
	mGrM	= mMoM;
	mMoM	= aux;
}

void ArucoThread::run()
{
	cv::Mat frame;
	mCancel = false;

	// test if camera is set
	if ( mCapVideo == NULL ) {
		qDebug() << "[ArucoThread::run()]  Camera is not set";
		return;
	}

	// prepare parameters for correction
	const double width  = mCapVideo->getWidth();
	const double height = mCapVideo->getHeight();

	mCamDistRatio  = Util::ApplicationConfig::get()->getValue( "Aruco.CamDistancRatio" ).toDouble();
	mRatioCamCoef  = ( 1 - height/width ) / mCamDistRatio;
	mHalfRatioCoef = 0.5 + width / ( 2*height );

	// file with parameters for camera, Aruco need it
	QString filename = "../share/3dsoftviz/config/camera.yml";
	QFileInfo file( filename );
	if ( ! file.exists() ) {
		qDebug() << "[ArucoThread::run()]: File " << file.absoluteFilePath() << " does Not exist!";
		return;
	}

	// create Aruco, set camera pararameters and check it
	ArucoCore aCore;
	bool camParametersOk = aCore.setCameraParameters( filename );

	if ( camParametersOk ) {
		while ( ! mCancel ) {	// doing aruco work in loop

			// variables for result from aruco
			bool		 markerDetected;

			frame = mCapVideo->queryFrame();		// get image from camera

			int         markerArraySize = 0;
			markerArraySize = aCore.detect( frame.clone() );

			//JMA
			if ( mMultiMarkerEnabled ) {
				//reset base marker index for this run
				aCore.setBaseMarkerIndex( -1 );

				for ( int i = 0; i< markerArraySize; i++ ) {
					double		 actPosArray[3];			// x, y, z
					double		 actQuatArray[4];		// angle(w), x, y, z
					int curMarkerId = aCore.getPosAndQuat( i, actPosArray, actQuatArray );

					// qDebug() << i << " : ID [" << curMarkerId << "] " << actPosArr[i].x() << " / " << actQuatArr[i].x();

					//it this is base marker
					if ( curMarkerId == 789 ) {
						// set this marker as Base marker
						aCore.setBaseMarkerIndex( i );

						//jurik
						//set and send modelview matrix of detected marker
						QMatrix4x4 modelviewmatrix = aCore.getDetectedMatrix( i, frame.clone() );
						emit sendModelViewMatrix( modelviewmatrix );

						//set and send projection matrix of detected image
						QMatrix4x4 projectionmatrix = aCore.getProjectionMatrix( frame.clone() );
						emit sendProjectionMatrix( projectionmatrix );

						//send marker size
						emit sendMarkerSize( aCore.getMarkerSize() );
						//*****
					}
					else {
						mArControlClass->updateObjectPositionAruco(
							curMarkerId,
							aCore.getDetectedMatrix( i, frame.clone() ),
							mMarkerIsBehind
						);
					}
				}
			}
			else {
				/*
				// single marker -> set control marker index to 0
				aCore.setBaseMarkerIndex(0);
				// graph controll
				markerDetected = aCore.getPosAndQuat( mGrM, actPosArray, actQuatArray );

				if ( markerDetected ) {

					// test if marker was detect (if not, all number in matrix are not range)
					if ( actPosArray[2] > 0.0  &&  actPosArray[2] < 10.0
							&&   actQuatArray[0] >= -1.0  &&  actQuatArray[0] <= 1.0 ) {

				        graphControlling(
				            osg::Vec3f( actPosArray[0], actPosArray[1], actPosArray[2] ),
				            osg::Quat( actQuatArray[0],  actQuatArray[1],  actQuatArray[2], actQuatArray[3] )
				        );
					}
				}

				// mouse controll
				markerDetected = aCore.getPosAndQuat( mMoM, actPosArray, actQuatArray );

				if ( markerDetected ) {

					// test if marker was detect (if not, all number in matrix are not range)
					if ( actPosArray[2] > 0.0  &&  actPosArray[2] < 10.0
							&&   actQuatArray[0] >= -1.0  &&  actQuatArray[0] <= 1.0 ) {

				        mouseControlling(
				            osg::Vec3f( actPosArray[0], actPosArray[1], actPosArray[2] ),
				            osg::Quat( actQuatArray[0],  actQuatArray[1],  actQuatArray[2], actQuatArray[3] )
				        );
					}
				}
				*/
			}
			imagesSending( aCore, frame );

			if ( ! mCancel ) {
				msleep( 50 );
			}
		}
	}

	mCapVideo->release();
	mCapVideo = NULL;
}

//JMA - rewrite to use vector as input
osg::Vec3d ArucoThread::normalizePos( const osg::Vec3f actPosArray, const osg::Quat actQuatArray )
{

	// can be corection parameters updated
	if ( mUpdCorPar ) {
		computeCorQuatAndPos( actPosArray, actQuatArray );
	}

	osg::Vec3d actPos( -actPosArray[0], -actPosArray[1], -actPosArray[2] );

	osg::Quat  actQuat;

	//  forward/backward,   left/right,  around,   w
	if ( mMarkerIsBehind ) {
		actQuat.set( actQuatArray[1], -actQuatArray[3],  actQuatArray[2],  actQuatArray[0] );
	}
	else {
		actQuat.set( actQuatArray[1],  actQuatArray[3],  actQuatArray[2],  -actQuatArray[0] );
	}


	if ( mCorEnabled ) {
		correctQuatAndPos( actPos, actQuat );
	}



	// normalizin from [0,0] in top left corner to [1,1] in roght bottom corner
	double absZ		= actPosArray[2]  < 0.0 ? - actPosArray[2]	:  actPosArray[2];		// distance of marker
	double halfSize = absZ / mCamDistRatio;

	double normX = actPos.x() / halfSize;							// horizontal
	double normY = actPos.y() / halfSize;		// vertical

	// correct Y centering, because of camerra different ration aruco top max y value is less than bottom one
	normX = normX - 0.1 - 0.01/absZ;

	actPos.x() = normX;
	actPos.y() = normY;

	return actPos;
}


//void ArucoThread::graphControlling( const double actPosArray[3], const double actQuatArray[4] )
void ArucoThread::graphControlling( const osg::Vec3f actPosArray, const osg::Quat actQuatArray )
{

	// can be corection parameters updated
	if ( mUpdCorPar ) {
		computeCorQuatAndPos( actPosArray, actQuatArray );
	}

	osg::Vec3d actPos( -actPosArray[0], -actPosArray[1], -actPosArray[2] );
	//osg::Vec3d actPos( -actPosArray[0], -actPosArray[1] * mHalfRatioCoef, -actPosArray[2] );

	osg::Quat  actQuat;


	//  forward/backward,   left/right,  around,   w
	if ( mMarkerIsBehind ) {
		actQuat.set( actQuatArray[1], -actQuatArray[3],  actQuatArray[2],  actQuatArray[0] );
	}
	else {
		actQuat.set( actQuatArray[1],  actQuatArray[3],  actQuatArray[2],  -actQuatArray[0] );
	}


	if ( mCorEnabled ) {
		correctQuatAndPos( actPos, actQuat );
	}



	// normalizin from [0,0] in top left corner to [1,1] in roght bottom corner
	double absZ		= actPosArray[2]  < 0.0 ? - actPosArray[2]	:  actPosArray[2];		// distance of marker
	double halfSize = absZ / mCamDistRatio;

	double normX = actPos.x() / halfSize;							// horizontal
	double normY = actPos.y() / halfSize;		// vertical

	// correct Y centering, because of camerra different ration aruco top max y value is less than bottom one
	normX = normX - 0.1 - 0.01/absZ;

	actPos.x() = normX;
	actPos.y() = normY;



	//emit sendArucoPosVec( actPos );
	//emit sendArucoRorQuat( actQuat );
	emit sendArucoPosAndQuat( actQuat, actPos );
}

//void ArucoThread::mouseControlling( const double actPosArray[3], const double actQuatArray[4] )
void ArucoThread::mouseControlling( const osg::Vec3f actPosArray, const osg::Quat actQuatArray )
{
	osg::Vec3d actPos( actPosArray[0], -actPosArray[1] * mHalfRatioCoef, -actPosArray[2] );

	// normalizin from [0,0] in top left corner to [1,1] in roght bottom corner
	double absZ		= actPosArray[2]  < 0.0 ? - actPosArray[2]	:  actPosArray[2];		// distance of marker
	double halfSize = absZ / mCamDistRatio;

	double normX = ( halfSize + actPos.x() ) / ( halfSize*2 );							// horizontal
	double normY = ( halfSize + actPos.y() ) / ( halfSize*2 );		// vertical

	if ( normX < 0.0 ) {
		normX = 0.0;
	}
	if ( normX > 1.0 ) {
		normX = 1.0;
	}
	if ( normY < 0.0 ) {
		normY = 0.0;
	}
	if ( normY > 1.0 ) {
		normY = 1.0;
	}

	bool click = boolQueue->getAvgBool( actQuatArray[3] <= 0.0 );

	emit moveMouseArucoSignal( normX, normY, click, Qt::LeftButton ); // now we dont recognize buttons, but in future it could be implemented

}

void ArucoThread::imagesSending( ArucoCore& aCore, cv::Mat frame ) const
{
	if ( mSendBackgrImgEnabled && !frame.empty() ) {
		if ( ! mMarkerIsBehind ) {
			cv::flip( frame, frame, 1 );
		}
		cv::cvtColor( frame, frame,CV_BGR2RGB );

		emit pushBackgrImage( frame.clone() );
	}

	cv::Mat image;
	if ( mMultiMarkerEnabled ) {
		image = aCore.getDetectedRectangleImage();
	}
	else {
		image = aCore.getDetImage();
	}

	if ( mMarkerlessTrackingEnabled ){
		mMarkerlessTracker->track(image);
	}

	if ( mSendImgEnabled ) {
		if ( ! mMarkerIsBehind ) {
			cv::flip( image, image, 1 );
		}
		cv::cvtColor( image, image, CV_BGR2RGB );


		if ( mSendBackgrImgEnabled ) {
			//if you comment this, background image will be without the augmented reality
			emit pushBackgrImage( image.clone() );
		}

		emit pushImagemMat( image.clone() );

	}
}

void ArucoThread::detectMarkerFromImage( cv::Mat image )
{
	ArucoCore aCore;
	aCore.detect( image );
	cv::Mat frame;
	frame = aCore.getDetectedRectangleImage();
	emit pushImageFromKinect( frame.clone() );
}

//void ArucoThread::computeCorQuatAndPos( const double position[3], const double rotation[4] )
void ArucoThread::computeCorQuatAndPos( const osg::Vec3f position, const osg::Quat rotation )

{
	qDebug() << "ARUCO: comput cor par done>";
	// set corection translation
	mCorP.x() = -position[0];
	mCorP.y() = -position[1];
	mCorP.z() = -position[2];
	// set corection quaternion
	osg::Quat tmp( rotation[1], rotation[2], rotation[3], rotation[0] );
	mCorQ = tmp.conj();

	mCorSetted = true;
	mUpdCorPar = false;
	emit corParUpdated();	// emit that corection parameters were updated
}

void ArucoThread::correctQuatAndPos( osg::Vec3d& actPos, osg::Quat& actQuat ) const
{
	if ( mCorSetted == true ) {
		// correct position
		// rotate point around correction point(corP) =  translate, rotate and translate back
		actPos = ( mCorQ * ( actPos - mCorP ) ) + mCorP;

		// correct rotation
		actQuat = actQuat * mCorQ;

	}
	else {
		qDebug() << "ArucoThread:computeCorQautAndPos() was called before setted correction parameters before!";
	}
}

void ArucoThread::computeCorMat( QMatrix4x4 origM )
{

	QMatrix4x4 T1, T2;

	// get translation's vectors
	QVector3D vTran2( origM( 0,3 ),  origM( 1,3 ),	 origM( 2,3 ) );
	QVector3D vTran1( -origM( 0,3 ), -origM( 1,3 ), -origM( 2,3 ) );
	// create translation matrixes
	T1.translate( vTran1 );					//printMat(T1, "T1");
	T2.translate( vTran2 );					//printMat(T2, "T2");

	// delete translation info -> make rotation matrix
	origM( 0,3 ) = 0.0;
	origM( 1,3 ) = 0.0;
	origM( 2,3 ) = 0.0;

	// compute corection matrix
	mCorM = T2 * origM * T1;

	// !!! corection matrix must be on left in multiplication
	// corM * corrected matrix
}


void ArucoThread::printVec( const osg::Vec3d v, const QString name )const
{
	qDebug() << name << " " << v.x() << " " << v.y() << " " << v.z();
}

void ArucoThread::printVec( const osg::Vec4d v, const QString name ) const
{
	qDebug() << name << " " << v.x() << " " << v.y() << " " << v.z() << " " << v.w();
}

void ArucoThread::printMat( const osg::Matrixd mat, const QString name ) const
{
	qDebug() << name;
	QString str;
	str  = " " + QString::number( mat( 0,0 ), 'f', 2 );
	str += " " + QString::number( mat( 0,1 ), 'f', 2 );
	str += " " + QString::number( mat( 0,2 ), 'f', 2 );
	str += " " + QString::number( mat( 0,3 ), 'f', 2 );
	qDebug() << ": " << str;
	str  = " " + QString::number( mat( 1,0 ), 'f', 2 );
	str += " " + QString::number( mat( 1,1 ), 'f', 2 );
	str += " " + QString::number( mat( 1,2 ), 'f', 2 );
	str += " " + QString::number( mat( 1,3 ), 'f', 2 );
	qDebug() << ": " << str;
	str  = " " + QString::number( mat( 2,0 ), 'f', 2 );
	str += " " + QString::number( mat( 2,1 ), 'f', 2 );
	str += " " + QString::number( mat( 2,2 ), 'f', 2 );
	str += " " + QString::number( mat( 2,3 ), 'f', 2 );
	qDebug() << ": " << str;
	str  = " " + QString::number( mat( 3,0 ), 'f', 2 );
	str += " " + QString::number( mat( 3,1 ), 'f', 2 );
	str += " " + QString::number( mat( 3,2 ), 'f', 2 );
	str += " " + QString::number( mat( 3,3 ), 'f', 2 );
	qDebug() << ": " << str;
}

} // namespace ArucoModul

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
