#include "Kinect/KinectZoom.h"

#include <vector>

Kinect::KinectZoom::KinectZoom()
{
	previousZ = 0.0f;
	currentZ = 0.0f;
	delta = 0.0f;
	zoomThreshold=7.0f;
	viewer = AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
}

Kinect::KinectZoom::~KinectZoom()
{

}

// zoom
void Kinect::KinectZoom::zoom()
{
	delta = ( previousZ-currentZ );
	if ( abs( delta ) > zoomThreshold ) {
		viewer->getEventQueue()->mouseScroll2D( 0, delta*5.0f, 0 );
	}
}

// get depth image of hand and depth value
void Kinect::KinectZoom::calcHandDepthFrame( cv::Mat frame,openni::VideoStream* m_depth, float x, float y, float z, bool mainHand )
{
	// convert coordinates of hand
	openni::CoordinateConverter coordinateConverter;
	float x1;
	float y1;
	float x2;
	float y2;
	float z1;
	coordinateConverter.convertWorldToDepth( *m_depth, x-150.0f, y-150.0f, z, &x1, &y1, &z1 );
	coordinateConverter.convertWorldToDepth( *m_depth, x+200.0f, y+200.0f, z, &x2, &y2, &z1 );

	// store current and previous depth for main hand only
	if ( mainHand ) {
		previousZ = currentZ;
		currentZ = z1;
	}

	openni::VideoFrameRef depthFrame;
	m_depth->readFrame( &depthFrame );
	//PK mod:
	// original:
	//openni::DepthPixel* depthPixels = ( openni::DepthPixel* )depthFrame.getData();
	// new:
	openni::DepthPixel* depthPixels = const_cast<openni::DepthPixel*>( reinterpret_cast<const openni::DepthPixel*>( depthFrame.getData() ) );
	//PK end
	cv::Mat depthImage( depthFrame.getHeight(), depthFrame.getWidth(), CV_16UC1, depthPixels );

	cv::Rect rect;
	rect.x = static_cast<int>( x1 );
	//rect.y = frame.rows - static_cast<int>( y1 );
	rect.y = depthImage.rows - static_cast<int>( y1 );
	rect.width = abs( static_cast<int>( x1-x2 ) );
	rect.height = abs( static_cast<int>( y1-y2 ) );

	if ( rect.x<0 ) {
		rect.x=0;
	}
	if ( rect.y<0 ) {
		rect.y=0;
	}
	if ( rect.width<0 ) {
		rect.width=0;
	}
	if ( rect.height<0 ) {
		rect.height=0;
	}
	if ( ( rect.x+rect.width )>frame.cols-1 ) {
		rect.width =frame.cols-1-rect.x;
	}
	if ( ( rect.y+rect.height )>frame.rows-1 ) {
		rect.height = frame.rows-1-rect.y;
	}

	depthImage = depthImage( rect );
	double minVal;
	double maxVal;
	cv::Point minLoc;
	cv::Point maxLoc;

	minMaxLoc( depthImage, &minVal, &maxVal, &minLoc, &maxLoc );

	cv::Mat depthImage2;
	depthImage.convertTo( depthImage2,CV_8UC1,255/maxVal );

	// floodfill segmentation of hand from depth map
	mask = cv::Mat::zeros( depthImage2.rows + 2, depthImage2.cols + 2, CV_8U );
	cv::floodFill( depthImage2, mask, cv::Point( depthImage2.cols/2,depthImage2.rows/2 ),
				   255, 0, cv::Scalar( 4 ),
				   cv::Scalar( 4 ),  4 + ( 255 << 8 ) + cv::FLOODFILL_MASK_ONLY + cv::FLOODFILL_FIXED_RANGE );
#ifdef QT_DEBUG
	// show images of segmented hand in debug mode only
	//cv::namedWindow( "floodfill", CV_WINDOW_AUTOSIZE );
	//cv::imshow( "floodfill", mask );
	//cv::waitKey( 33 );
#endif
}

// find contours of segmented hand and count fingers
int Kinect::KinectZoom::DetectContour( )
{
	int numFingers = 0;
	cv::Mat drawing = cv::Mat::zeros( mask.size(), CV_8UC3 );
	cv::vector<cv::vector<cv::Point> > contours;
	cv::vector<cv::Vec4i> hierarchy;

	findContours( mask,contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE, cv::Point() );

	if ( contours.size()>0 ) {

		//hull MUSI byt typu int !!!
		cv::vector<std::vector<int> >hull( contours.size() );
		cv::vector<cv::vector<cv::Vec4i> > convDef( contours.size() );
		cv::vector<cv::vector<cv::Point> > hull_points( contours.size() );
		cv::vector<cv::vector<cv::Point> > defect_points( contours.size() );

		for ( size_t i = 0; i < contours.size(); i++ ) {
			if ( cv::contourArea( contours[i] )>500 ) {
				cv::convexHull( contours[i], hull[i], false );
				cv::convexityDefects( contours[i],hull[i], convDef[i] );

				for ( size_t k=0; k<hull[i].size(); k++ ) {
					auto ind=hull[i][k];
					hull_points[i].push_back( contours[i][static_cast<unsigned long>( ind )] );
				}

				for ( size_t k=0; k<convDef[i].size(); k++ ) {
					if ( convDef[i][k][3]>20*256 ) { // filter defects by depth
						numFingers++;
						size_t ind_0=static_cast<size_t>( convDef[i][k][0] );
						size_t ind_1=static_cast<size_t>( convDef[i][k][1] );
						size_t ind_2=static_cast<size_t>( convDef[i][k][2] );
						defect_points[i].push_back( contours[i][ind_2] );
						cv::circle( drawing,contours[i][ind_0],5,cv::Scalar( 0,255,0 ),-1 );
						cv::circle( drawing,contours[i][ind_1],5,cv::Scalar( 0,255,0 ),-1 );
						cv::circle( drawing,contours[i][ind_2],5,cv::Scalar( 0,0,255 ),-1 );
						cv::line( drawing,contours[i][ind_2],contours[i][ind_0],cv::Scalar( 0,0,255 ),1 );// filter defects by depth
						cv::line( drawing,contours[i][ind_2],contours[i][ind_1],cv::Scalar( 0,0,255 ),1 );
					}
				}
				// draw results
				cv::drawContours( drawing, contours, static_cast<int>( i ), cv::Scalar( 0,255,0 ), 1, 8, cv::vector<cv::Vec4i>(), 0, cv::Point() );
				drawContours( drawing, hull_points, static_cast<int>( i ), cv::Scalar( 255,0,0 ), 1, 8, cv::vector<cv::Vec4i>(), 0, cv::Point() );
			}
		}
	}
#ifdef QT_DEBUG
	// imshow( "Hull", drawing );
#endif

	return numFingers;
}
