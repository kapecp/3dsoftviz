#include "Aruco/arucocore.h"
#include "Util/ApplicationConfig.h"
#include <QDebug>
#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"

namespace ArucoModul {

ArucoCore::ArucoCore()
{

	mMarkerSize = Util::ApplicationConfig::get()->getValue( "Aruco.MarkerSize" ).toFloat();
	mBaseMarkerIndex = 0;

}



bool ArucoCore::setCameraParameters( const QString markerDesFile )
{
	try {
		if ( markerDesFile.isEmpty() ) {
			qDebug() << "ArucoCore::cameraParameters(): markerDesFile name is empty";
		}
		//read camera paramters if passed
		mCamParam.readFromXMLFile( markerDesFile.toStdString() );
		return true;
	}
	catch ( std::exception& ex ) {
		qDebug() << "Exception: " << QString::fromUtf8( ex.what() );
		return false;
	}
}

const QMatrix4x4 ArucoCore::getDetectedMatrix( int markerId, cv::Mat inputImage )
{
	qreal modelViewMatrix[16];

	mCamParam.resize( inputImage.size() );
	mCamImage = inputImage;		//updateImage( inputImage );

	// get result model view matrix from imput image
	//this->detectMarkers();
	this->getMatrix( markerId, modelViewMatrix );

	QMatrix4x4 matrix( modelViewMatrix[ 0], modelViewMatrix[ 1], modelViewMatrix[ 2], modelViewMatrix[ 3],
					   modelViewMatrix[ 4], modelViewMatrix[ 5], modelViewMatrix[ 6], modelViewMatrix[ 7],
					   modelViewMatrix[ 8], modelViewMatrix[ 9], modelViewMatrix[10], modelViewMatrix[11],
					   modelViewMatrix[12], modelViewMatrix[13], modelViewMatrix[14], modelViewMatrix[15] );

	return matrix;
}

//jurik
const QMatrix4x4 ArucoCore::getProjectionMatrix( cv::Mat inputImage )
{
	double projectionMatrix[16];

	mCamParam.resize( inputImage.size() );
	//get projection matrix via ArUco
	mCamParam.glGetProjectionMatrix( inputImage.size(),inputImage.size(),projectionMatrix,0.01,10000.0 );

	QMatrix4x4 matrix( projectionMatrix[ 0], projectionMatrix[ 1], projectionMatrix[ 2], projectionMatrix[ 3],
					   projectionMatrix[ 4], projectionMatrix[ 5], projectionMatrix[ 6], projectionMatrix[ 7],
					   projectionMatrix[ 8], projectionMatrix[ 9], projectionMatrix[10], projectionMatrix[11],
					   projectionMatrix[12], projectionMatrix[13], projectionMatrix[14], projectionMatrix[15] );
	return matrix;
}

float ArucoCore::getMarkerSize()
{
	return this->mMarkerSize;
}

//*****

bool ArucoCore::getDetectedPosAndQuat( cv::Mat inputImage, double position[3], double quaternion[4] )
{
	mCamParam.resize( inputImage.size() );

	mCamImage = inputImage;

	detectMarkers();

	if ( mMarkers.size() > 0 ) {
		mMarkers[0].OgreGetPoseParameters( position, quaternion );
		return true;

	}
	else {
		return false;
	}

}

std::size_t ArucoCore::detect( cv::Mat inputImage )
{
	//mCamParam.resize( inputImage.size() );

	mCamImage = inputImage;

	detectMarkers();

	return mMarkers.size();
}

int ArucoCore::getPosAndQuat( unsigned int markerNum, double position[3], double quaternion[4] )
{
	// markerNum is counted from 0
	if ( mMarkers.size() > markerNum ) {
		mMarkers[markerNum].OgreGetPoseParameters( position, quaternion );
		return mMarkers[markerNum].id;

	}
	else {
		return 0;
	}

}


void ArucoCore::updateImage( cv::Mat inputImage ) // now we dont use this method
{

	mCamImage.create( inputImage.size(),CV_8UC3 );

	//transform color that by default is BGR to RGB because windows systems do not allow reading BGR images with opengl properly
	cv::cvtColor( inputImage,inputImage,CV_BGR2RGB );

	//remove distorion in image
	// nemoze byt ak je camera nastavena v konstruktore,
	// ma za nasledok neustalu postupnu zmenu parametrov kamary,
	// az sa stanu nepouzitelne a tym deformovanie obrazka do jeho rohu
	//cv::undistort(inputImage, mCamImage, mCamParam.CameraMatrix, mCamParam.Distorsion);
	mCamImage = inputImage;

}

void ArucoCore::detectMarkers()
{
	//detect markers
	mMDetector.detect( mCamImage,
					   mMarkers,
					   mCamParam, //mCamParam.CameraMatrix, cv::Mat(),   // optionaly, dont needed now
					   mMarkerSize );
}

int ArucoCore::getMatrix( int markerId, double* modelviewmatrix )
{
	if ( mMarkers.size() > 0 && markerId != -1 ) {
		mMarkers[markerId].glGetModelViewMatrix( modelviewmatrix );
		return 0;
	}
	return -1;
}



cv::Mat ArucoCore::getDetImage()
{
	//for each marker, draw info and its boundaries in the image
	for ( unsigned int i = 0; i < mMarkers.size(); i++ ) {
		mMarkers[i].draw( mCamImage, cv::Scalar( 0,0,255 ), 2 );

		//draw a 3d cube in each marker if there is 3d info
		if ( mCamParam.isValid() &&  !qFuzzyCompare( mMarkerSize,-1.0f ) ) {
			aruco::CvDrawingUtils::draw3dCube( mCamImage, mMarkers[i], mCamParam );
		}
	}

	return mCamImage;	// return image with augmented information
}

cv::Mat ArucoCore::getDetectedRectangleImage()
{
	//for each marker, draw info and cube from markers
	//qDebug() << "Pocet markerov je = " << mMarkers.size();
	for ( unsigned int i = 0; i < mMarkers.size(); i++ ) {
		mMarkers[i].draw( mCamImage, cv::Scalar( 0,0,255 ), 2 );

		//draw a 3d rectangle from markers if there is 3d info
		if ( mCamParam.isValid() &&  !qFuzzyCompare( mMarkerSize,-1.0f ) ) {
			drawCube( mCamImage, mMarkers, mCamParam );
		}
	}

	return mCamImage; //return image with augmented information
}

void ArucoCore::drawCube( cv::Mat& Image, const vector<aruco::Marker>& m,const aruco::CameraParameters& CP )
{

	//qDebug() << "Velkost vektora markerov " << m.size();

	cv::Point2f* pointArray = static_cast<cv::Point2f*>( malloc( ( m.size()+1 )*sizeof( cv::Point2f ) ) );
	cv::Point2f* pointArray2 = static_cast<cv::Point2f*>( malloc( ( m.size()+1 )*sizeof( cv::Point2f ) ) );

	//for each marker  compute his 2d representation on frame
	for ( unsigned int i = 0; i < m.size(); i++ ) {
		cv::Mat objectPoints( 2, 3, CV_32FC1 );
		objectPoints.at<float>( 0,0 )=0;
		objectPoints.at<float>( 0,1 )=0;
		objectPoints.at<float>( 0,2 )=0;

		objectPoints.at<float>( 1,0 )=0;
		objectPoints.at<float>( 1,1 )=0;
		objectPoints.at<float>( 1,2 )=0.05f;

		vector<cv::Point2f> imagePoint;
		cv::projectPoints( objectPoints, m[i].Rvec, m[i].Tvec, CP.CameraMatrix, CP.Distorsion, imagePoint );
		pointArray[i] = imagePoint[0];
		pointArray2[i] = imagePoint[1];
	}

	//if we detect 2 markers, draw a line
	if ( m.size() == 2 ) {
		//qDebug() << "Vykreslujem ciaru";
		cv::line( Image, pointArray[0], pointArray[1], cv::Scalar( 0,255,255 ), 2, CV_AA );
	}

	//if we detect 3 markers, compute the 4th point and draw a rectangle
	if ( m.size() == 3 ) {
		//qDebug() << "Vykreslujem stvorec alebo obdlznik";
		//compute the 4th point
		pointArray[3].x = pointArray[2].x + ( pointArray[0].x - pointArray[1].x );
		pointArray[3].y = pointArray[0].y + ( pointArray[2].y - pointArray[1].y );

		pointArray2[3].x = pointArray2[2].x + ( pointArray2[0].x - pointArray2[1].x );
		pointArray2[3].y = pointArray2[0].y + ( pointArray2[2].y - pointArray2[1].y );


		//cv::rectangle(Image, pointArray[2], pointArray[0], cv::Scalar(0,255,255), CV_FILLED,8,0);

		cv::line( Image, pointArray[0], pointArray[1], cv::Scalar( 0,0,255 ), 1, CV_AA );
		cv::line( Image, pointArray[1], pointArray[2], cv::Scalar( 0,0,255 ), 1, CV_AA );
		cv::line( Image, pointArray[3], pointArray[0], cv::Scalar( 0,0,255 ), 1, CV_AA );
		cv::line( Image, pointArray[3], pointArray[2], cv::Scalar( 0,0,255 ), 1, CV_AA );

		cv::line( Image, pointArray2[0], pointArray2[1], cv::Scalar( 0,255,255 ), 1, CV_AA );
		cv::line( Image, pointArray2[1], pointArray2[2], cv::Scalar( 0,255,255 ), 1, CV_AA );
		cv::line( Image, pointArray2[3], pointArray2[0], cv::Scalar( 0,255,255 ), 1, CV_AA );
		cv::line( Image, pointArray2[3], pointArray2[2], cv::Scalar( 0,255,255 ), 1, CV_AA );

		cv::line( Image, pointArray[0], pointArray2[0], cv::Scalar( 255,0,255 ), 1, CV_AA );
		cv::line( Image, pointArray[1], pointArray2[1], cv::Scalar( 255,0,255 ), 1, CV_AA );
		cv::line( Image, pointArray[2], pointArray2[2], cv::Scalar( 255,0,255 ), 1, CV_AA );
		cv::line( Image, pointArray[3], pointArray2[3], cv::Scalar( 255,0,255 ), 1, CV_AA );
	}
}

int ArucoCore::getBaseMarkerIndex()
{
	return mBaseMarkerIndex;
}

void ArucoCore::setBaseMarkerIndex( int baseMarkerIndex )
{
	mBaseMarkerIndex = baseMarkerIndex;
}

} // namespace ArucoModul
