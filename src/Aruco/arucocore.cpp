#include "arucocore.h"

ArucoCore::ArucoCore()
{

    ArucoCore::MarkerSize = 0.05;
    try
    {
        cameraParameters();
    }
    catch(std::exception &ex)
    {
        cout<<"Exception: "<<ex.what()<<endl;
    }
}
void ArucoCore::cameraParameters()
{
    //read camera paramters if passed
    ArucoCore::Camera.readFromXMLFile("intrinsics.yml");
}

void ArucoCore::updateImage(cv::Mat InputImage)
{

    ArucoCore::Camera.resize(InputImage.size());
    ArucoCore::CameraImage.create(InputImage.size(),CV_8UC3);
    //transform color that by default is BGR to RGB because windows systems do not allow reading BGR images with opengl properly
   cv::cvtColor(InputImage,InputImage,CV_BGR2RGB);
    //remove distorion in image
    cv::undistort(InputImage,ArucoCore::CameraImage, ArucoCore::Camera.CameraMatrix, ArucoCore::Camera.Distorsion);
}
void ArucoCore::detectMarkers()
{
    //detect markers
    ArucoCore::MDetector.detect(ArucoCore::CameraImage,ArucoCore::Markers, ArucoCore::Camera.CameraMatrix,cv::Mat(),ArucoCore::MarkerSize);
}

int ArucoCore::getMatrix(double *modelviewmatrix)
{
    if(ArucoCore::Markers.size() > 0)
    {
        ArucoCore::Markers[0].glGetModelViewMatrix(modelviewmatrix);
        return 0;
    }
    return -1;
}
