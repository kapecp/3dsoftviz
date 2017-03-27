#include "OpenCV/MarkerlessTracker.h"
#include <aruco.h>
#include <opencv2/imgproc/imgproc.hpp>

OpenCV::MarkerlessTracker::MarkerlessTracker()
{
    mArControlClass = new ArucoModul::ArControlClass();

    QString filename = "../share/3dsoftviz/config/camera.yml";
    mCamParam.readFromXMLFile( filename.toStdString() );
}

OpenCV::MarkerlessTracker::~MarkerlessTracker()
{
}

cv::RNG rng( 12345 );

void OpenCV::MarkerlessTracker::track( cv::Mat& frame )
{
    //(x,y,radius)
    cv::vector<cv::Vec3f> circles;
    findCirclesInFrame( frame, circles);


    //TODO read from config file
    float circle_width = 120.0; //mm

    // Handle found circles
    // TODO assign ids to circles, backtrack them
    for ( size_t i = 0; i < circles.size(); i++ ) {
        //(x,y,radius)
        cv::Point3f circle( cvRound( circles[i][0] ), cvRound( circles[i][1] ), cvRound( circles[i][2] ) );

        cv::Mat rvec(3, 1, CV_32FC1);
        cv::Mat tvec(3, 1, CV_32FC1);
        estimatePoseCircle( osg::Vec2f(circle.x, circle.y), circle_width/2, circle.z, mCamParam, rvec, tvec);

        QMatrix4x4 mVMatrix;
        calculateMVMatrixFromPose( rvec, tvec, mVMatrix);

        mArControlClass->updateObjectPositionAruco( 1, mVMatrix, true);
    }

}

void OpenCV::MarkerlessTracker::findCirclesInFrame( cv::Mat& frame,
                                                    cv::vector<cv::Vec3f>& circlesReal
                                                   ){
//	cv::resize(frame,frame,cv::Size(frame.rows/4,frame.cols/4));
	cv::Mat frame_gray;
    cv::cvtColor( frame, frame_gray, CV_BGR2GRAY );
    cv::Mat frame_bilateral;
    cv::bilateralFilter( frame_gray, frame_bilateral, 15, 30, 7.5);

    cv::vector<cv::Vec3f> circles;
    cv::HoughCircles( frame_bilateral, circles, CV_HOUGH_GRADIENT, 2, frame_bilateral.rows / 8, 200, 100, 0, 0);

    cv::vector<cv::Mat> mask_vector;

    cv::Mat img_circles;
        frame.copyTo( img_circles );

    /// Draw the circles
    bool do_continue = true;
    for ( size_t i = 0; i < circles.size(); i++ ) {
        do_continue = true;
        cv::Mat mask( frame_bilateral.size(), frame_bilateral.type() );
        mask.setTo( cv::Scalar( 0, 0, 0 ) );

        cv::Point center( cvRound( circles[i][0] ), cvRound( circles[i][1] ) );

        //Test stredu kruhu pod dalsim kruhom - odstranenie z vectora
        for ( int j = 0; j < mask_vector.size(); j++ ) {
            uchar color = mask_vector[j].at<uchar>( center.y, center.x );
            if ( color > 0 ) {
                do_continue = false;
            }
        }
        if ( do_continue ) {
            int radius = cvRound( circles[i][2] );
            circle( img_circles, center, 3, cv::Scalar( 0, 255, 0 ), -1, 8, 0 );
            circle( img_circles, center, radius, cv::Scalar( 0, 0, 255 ), 3, 8, 0 );
            circle( mask, center, radius, cv::Scalar( 255, 255, 255 ), -1, 8, 0 );
            mask_vector.push_back( mask );
            circlesReal.push_back( circles[i] );
        }
    }

    // Show your results
    frame = img_circles;

	/*
    cv::imwrite( "C:/DevTools/ImageStreams/circle_masked.jpg", frame_masked );
    cv::imwrite( "C:/DevTools/ImageStreams/circle_detected.jpg", img_circles );
	*/
}

//JMA
void OpenCV::MarkerlessTracker::estimatePoseCircle( osg::Vec2f framePosition,
                                                    float circleRadiusReal,
                                                    float circleRadiusFrame,
                                                    aruco::CameraParameters mCamParam,
                                                    cv::Mat& rvec, cv::Mat& tvec
                                                   ){
    /*
     * Graph geometry - coordinates of points where object shouldbe rendered
     */
    cv::Mat objectPoints(4, 3, CV_32FC1);
        objectPoints.at< float >(0, 0) = 0;
        objectPoints.at< float >(0, 1) = circleRadiusReal;
        objectPoints.at< float >(0, 2) = 0;
        objectPoints.at< float >(1, 0) = circleRadiusReal;
        objectPoints.at< float >(1, 1) = 0;
        objectPoints.at< float >(1, 2) = 0;
        objectPoints.at< float >(2, 0) = 0;
        objectPoints.at< float >(2, 1) = -circleRadiusReal;
        objectPoints.at< float >(2, 2) = 0;
        objectPoints.at< float >(3, 0) = -circleRadiusReal;
        objectPoints.at< float >(3, 1) = 0;
        objectPoints.at< float >(3, 2) = 0;

    /*
     * Frame geometry - coordinates of points where object is found on the frame
     */
   cv::Mat imagePoints(4, 2, CV_32FC1);
        imagePoints.at< float >(0, 0) = framePosition.x();
        imagePoints.at< float >(0, 1) = framePosition.y()+circleRadiusFrame;
        imagePoints.at< float >(1, 0) = framePosition.x()+circleRadiusFrame;
        imagePoints.at< float >(1, 1) = framePosition.y();
        imagePoints.at< float >(2, 0) = framePosition.x();
        imagePoints.at< float >(2, 1) = framePosition.y()-circleRadiusFrame;
        imagePoints.at< float >(3, 0) = framePosition.x()-circleRadiusFrame;
        imagePoints.at< float >(3, 1) = framePosition.y();

    /*
     * Rotation and translation vectors(matrices)
     */
    cv::Mat raux, taux;
    cv::solvePnP(objectPoints, imagePoints, mCamParam.CameraMatrix, mCamParam.Distorsion, raux, taux);
    raux.convertTo(rvec, CV_32F);
    taux.convertTo(tvec, CV_32F);
}

//JMA
void OpenCV::MarkerlessTracker::calculateMVMatrixFromPose( cv::Mat rvec, cv::Mat tvec,
                                                           QMatrix4x4& mVMatrix
                                                          ){
    /*
     * convert rvec(rotation vector 3x1) to rotaion matrix(3x3)
     */
    cv::Mat rotationMatrix(3,3,cv::DataType<double>::type);
    cv::Rodrigues(rvec,rotationMatrix);

    QMatrix4x4 matrix(
        rotationMatrix.at< float >(0, 0),
        rotationMatrix.at< float >(1, 0),
        -rotationMatrix.at< float >(2, 0),
        0.0,
        rotationMatrix.at< float >(0, 1),
        rotationMatrix.at< float >(1, 1),
        -rotationMatrix.at< float >(2, 1),
        0.0,
        rotationMatrix.at< float >(0, 2),
        rotationMatrix.at< float >(1, 2),
        -rotationMatrix.at< float >(2, 2),
        0.0,
        tvec.at< float >(0, 0),
        tvec.at< float >(1, 0),
        -tvec.at< float >(2, 0),
        1.0
    );

    mVMatrix = matrix;
}
