#include "Aruco/arucocore.h"
#include "QDebug"

using namespace ArucoModul;

ArucoCore::ArucoCore(const QString markerDesFile)
{

	mMarkerSize = 0.05;
	try {
		cameraParameters(markerDesFile);
	}
	catch(std::exception &ex) {
		qDebug() << "Exception: " << QString::fromUtf8( ex.what());
	}
}

const QMatrix4x4 ArucoCore::getDetectedMatrix(cv::Mat inputImage){
	double modelViewMatrix[16];

	// get result model view matrix from imput image
	this->updateImage( inputImage );
	this->detectMarkers();
	this->getMatrix( modelViewMatrix );

	QMatrix4x4 matrix(modelViewMatrix);
	return matrix;
}

void ArucoCore::cameraParameters(const QString markerDesFile)
{
	if(markerDesFile.isEmpty()){
		qDebug() << "ArucoCore::cameraParameters(): markerDesFile name is empty";
	}
	//read camera paramters if passed

	mCamParam.readFromXMLFile( markerDesFile.toStdString() );
}

void ArucoCore::updateImage(cv::Mat inputImage)
{
	mCamParam.resize(inputImage.size());
	mCamImage.create(inputImage.size(),CV_8UC3);
	//transform color that by default is BGR to RGB because windows systems do not allow reading BGR images with opengl properly
	cv::cvtColor(inputImage,inputImage,CV_BGR2RGB);
	//remove distorion in image
	cv::undistort(inputImage, mCamImage, mCamParam.CameraMatrix, mCamParam.Distorsion);
}
void ArucoCore::detectMarkers()
{
	//detect markers
	mMDetector.detect( mCamImage,
					   mMarkers,
					   mCamParam.CameraMatrix,
					   cv::Mat(),
					   mMarkerSize);
}

int ArucoCore::getMatrix(double *modelviewmatrix)
{
	double modelview_matrix[16];
	if( mMarkers.size() > 0)
	{
		mMarkers[0].glGetModelViewMatrix(modelviewmatrix);
		return 0;
	}
	return -1;
}
