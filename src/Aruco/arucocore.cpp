#include "Aruco/arucocore.h"
#include "Util/ApplicationConfig.h"
#include "QDebug"
#include "opencv2/imgproc/imgproc.hpp"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"



namespace ArucoModul {

ArucoCore::ArucoCore()
{

    mMarkerSize = Util::ApplicationConfig::get()->getValue( "Aruco.MarkerSize" ).toFloat();

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

const QMatrix4x4 ArucoCore::getDetectedMatrix( cv::Mat inputImage )
{
    double modelViewMatrix[16];

    mCamParam.resize( inputImage.size() );
    mCamImage = inputImage;		//updateImage( inputImage );

    // get result model view matrix from imput image
    this->detectMarkers();
    this->getMatrix( modelViewMatrix );

    QMatrix4x4 matrix( modelViewMatrix );
    return matrix;
}

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

long ArucoCore::detect( cv::Mat inputImage )
{
    mCamParam.resize( inputImage.size() );

    mCamImage = inputImage;

    detectMarkers();

    return mMarkers.size();
}

bool ArucoCore::getPosAndQuat( unsigned int markerNum, double position[3], double quaternion[4] )
{
    // markerNum is counted from 0
    if ( mMarkers.size() > markerNum ) {
        mMarkers[markerNum].OgreGetPoseParameters( position, quaternion );
        return true;

    }
    else {
        return false;
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

int ArucoCore::getMatrix( double* modelviewmatrix )
{
    if ( mMarkers.size() > 0 ) {
        mMarkers[0].glGetModelViewMatrix( modelviewmatrix );
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

cv::Mat ArucoCore::getDetectedTriangleImage() {
    for( unsigned int i = 0; i < mMarkers.size(); i++ ) {
        mMarkers[i].draw( mCamImage, cv::Scalar( 0,0,255 ), 2 );

        if ( mCamParam.isValid() &&  !qFuzzyCompare( mMarkerSize,-1.0f ) ) {
            drawTriangle( mCamImage, mMarkers, mCamParam );
        }
    }

    return mCamImage;
}

void ArucoCore::drawTriangle(cv::Mat &Image, vector<aruco::Marker> &m,const aruco::CameraParameters &CP ) {

    cv::Point2f *pointArray = (cv::Point2f *) malloc(m.size()*sizeof(cv::Point2f));

    for( unsigned int i = 0; i < m.size(); i++ ) {
        cv::Mat objectPoints(1, 3, CV_32FC1);
        objectPoints.at<float>(0,0)=0;
        objectPoints.at<float>(0,1)=0;
        objectPoints.at<float>(0,2)=0;

        vector<cv::Point2f> imagePoint;
        cv::projectPoints(objectPoints, m[i].Rvec, m[i].Tvec, CP.CameraMatrix, CP.Distorsion, imagePoint);
        pointArray[i] = imagePoint[0];
    }
/*
    for(unsigned int j = 0; j < m.size(); j++ ) {
        cv::circle(Image, pointArray[j], 50, cv::Scalar(255,0,0), 3);
    }
*/
    if( m.size() == 2 ) {
        cv::line(Image, pointArray[0], pointArray[1], cv::Scalar(0,255,255), 2, CV_AA);
    }

    if( m.size() == 3 ) {
        cv::rectangle(Image, pointArray[2], pointArray[0], cv::Scalar(0,255,255), CV_FILLED,8,0);
        cv::line(Image, pointArray[0], pointArray[1], cv::Scalar(0,0,255), 2, CV_AA);
        cv::line(Image, pointArray[1], pointArray[2], cv::Scalar(0,0,255), 2, CV_AA);
        cv::line(Image, pointArray[2], pointArray[0], cv::Scalar(0,0,255), 2, CV_AA);
    }
}

} // namespace ArucoModul
