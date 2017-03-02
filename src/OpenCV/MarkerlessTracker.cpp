#include "OpenCV/MarkerlessTracker.h"


OpenCV::MarkerlessTracker::MarkerlessTracker()
{
}

OpenCV::MarkerlessTracker::~MarkerlessTracker()
{
}

cv::RNG rng( 12345 );

void OpenCV::MarkerlessTracker::track( cv::Mat& img )
{

//	Mat img = imread("img_test.jpg");
	cv::resize( img, img, cv::Size( img.cols / 4, img.rows / 4 ) );
	cv::Mat img1;
	cv::cvtColor( img, img1, CV_BGR2GRAY );
	cv::Mat dst_gaus;
	cv::GaussianBlur( img1, dst_gaus, cv::Size( 7, 7 ), 1.5, 0.0, 4 );
	cv::Mat dst_bil;
	int kernel_size = 15;
	cv::bilateralFilter( img1, dst_bil, kernel_size, kernel_size * 2, kernel_size / 2 );

	cv::vector<cv::Vec3f> circles;
	cv::vector<cv::Vec3f> circlesReal;

	/// Hough Circles
	cv::HoughCircles( dst_bil, circles, CV_HOUGH_GRADIENT, 1, dst_bil.rows / 8, 25, 35, 0, 0 );

	cv::vector<cv::Mat> mask_vector;

	cv::Mat img_circles;
	img.copyTo( img_circles );
	bool do_continue = true;

	/// Draw the circles
	for ( size_t i = 0; i < circles.size(); i++ ) {
		do_continue = true;
		cv::Mat mask( dst_bil.size(), dst_bil.type() );
		mask.setTo( cv::Scalar( 0, 0, 0 ) );

		cv::Point center( cvRound( circles[i][0] ), cvRound( circles[i][1] ) );

		//Test stredu kruhu pod dalsim kruhom - odstranenie z vectora
		for ( int j = 0; j < mask_vector.size(); j++ ) {
			uchar color = mask_vector[j].at<uchar>( center.y, center.x );
			if ( color > 0 ) {
				do_continue = false;
			}
		}
		if ( !do_continue ) {
			continue;
		}

		int radius = cvRound( circles[i][2] );
		// circle center
		circle( img_circles, center, 3, cv::Scalar( 0, 255, 0 ), -1, 8, 0 );
		// circle outline
		circle( img_circles, center, radius, cv::Scalar( 0, 0, 255 ), 3, 8, 0 );

		circle( mask, center, radius, cv::Scalar( 255, 255, 255 ), -1, 8, 0 );
		mask_vector.push_back( mask );
		circlesReal.push_back( circles[i] );
	}

	/// Show your results
	img = img_circles;

}
