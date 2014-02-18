#include "Aruco/arucocore.h"
#include "QDebug"

using namespace ArucoModul;

ArucoCore::ArucoCore()
{

	mMarkerSize = 0.05;
	try {
		cameraParameters();
	}
	catch(std::exception &ex) {
		qDebug() << "Exception: " << QString::fromUtf8( ex.what());
	}
}
void ArucoCore::cameraParameters()
{
	//read camera paramters if passed
	mCamParam.readFromXMLFile("intrinsics.yml");
}

void ArucoCore::updateImage(cv::Mat InputImage)
{

	mCamParam.resize(InputImage.size());
	mCamImage.create(InputImage.size(),CV_8UC3);
	//transform color that by default is BGR to RGB because windows systems do not allow reading BGR images with opengl properly
	cv::cvtColor(InputImage,InputImage,CV_BGR2RGB);
	//remove distorion in image
	cv::undistort(InputImage, mCamImage, mCamParam.CameraMatrix, mCamParam.Distorsion);
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
	if( mMarkers.size() > 0)
	{
		mMarkers[0].glGetModelViewMatrix(modelviewmatrix);
		return 0;
	}
	return -1;
}
