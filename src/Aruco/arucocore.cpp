#include "Aruco/arucocore.h"
#include "QDebug"


using namespace ArucoModul;

ArucoCore::ArucoCore()
{

	mMarkerSize = 0.06f;

}



bool ArucoCore::setCameraParameters(const QString markerDesFile)
{
	try {
		if(markerDesFile.isEmpty()){
			qDebug() << "ArucoCore::cameraParameters(): markerDesFile name is empty";
		}
		//read camera paramters if passed
		mCamParam.readFromXMLFile( markerDesFile.toStdString() );
		return true;
	}
	catch(std::exception &ex) {
		qDebug() << "Exception: " << QString::fromUtf8( ex.what());
		return false;
	}
}

const QMatrix4x4 ArucoCore::getDetectedMatrix(cv::Mat inputImage)
{
	double modelViewMatrix[16];

#ifdef WIN32
	mCamParam.resize(inputImage.size());
	mCamImage = inputImage;
#else
	updateImage( inputImage );
#endif

	// get result model view matrix from imput image
	this->detectMarkers();
	this->getMatrix( modelViewMatrix );

	QMatrix4x4 matrix(modelViewMatrix);
	return matrix;
}

bool ArucoCore::getDetectedPosAndQuat(cv::Mat inputImage, double position[3], double quaternion[4])
{
#ifdef WIN32
	mCamParam.resize(inputImage.size());
	mCamImage = inputImage;
#else
	updateImage( inputImage );
#endif

	detectMarkers();

	if( mMarkers.size() > 0){
		mMarkers[0].OgreGetPoseParameters( position, quaternion);
		return true;

	} else {
		return false;
	}

}

// tato funkcia je nepouzitelna na Windows
void ArucoCore::updateImage(cv::Mat inputImage)
{

	mCamParam.resize(inputImage.size());


	mCamImage.create(inputImage.size(),CV_8UC3);

	//transform color that by default is BGR to RGB because windows systems do not allow reading BGR images with opengl properly
	cv::cvtColor(inputImage,inputImage,CV_BGR2RGB);   // pri testovani praveze opacny efekt

	//remove distorion in image
	// nemoze byt ak je camera nastavena v konstruktore,
	// ma za nasledok neustalu postupnu zmenu parametrov kamary,
	// az sa stanu nepouzitelne a tym deformovanie obrazka do jeho rohu
	cv::undistort(inputImage, mCamImage, mCamParam.CameraMatrix, mCamParam.Distorsion);


}

void ArucoCore::detectMarkers()
{
	//detect markers
	mMDetector.detect( mCamImage,
					   mMarkers,
					   mCamParam, //mCamParam.CameraMatrix,
					   //cv::Mat(),
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

cv::Mat ArucoCore::getDetImage()
{
	//for each marker, draw info and its boundaries in the image
	for (unsigned int i = 0; i < mMarkers.size(); i++) {
		mMarkers[i].draw( mCamImage, cv::Scalar(0,0,255), 2);

		//draw a 3d cube in each marker if there is 3d info
		//if (  mCamParam.isValid() && mMarkerSize != -1 ){
		if (  mCamParam.isValid() &&  !qFuzzyCompare(mMarkerSize,-1.0f)){
			aruco::CvDrawingUtils::draw3dCube( mCamImage, mMarkers[i], mCamParam);
		}
	}

	return mCamImage;	// return image with augmented information
}
