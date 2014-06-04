#include "Core/Core.h"
#include "Kinect/KinectZoom.h"

Kinect::KinectZoom::KinectZoom()
{
	previousZ = 0.0f;
	currentZ = 0.0f;
	delta = 0.0f;
	zoomThreshold=0.0f;
}

Kinect::KinectZoom::~KinectZoom()
{

}

int Kinect::KinectZoom::DetectContour(Mat img){
	Mat drawing = Mat::zeros( img.size(), CV_8UC3 );
	vector<vector<Point> > contours;
	int numFingers=0;
	vector<Vec4i> hierarchy;

	findContours(img,contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE, Point());

	if(contours.size()>0)
	{
		vector<std::vector<int> >hull( contours.size() );
		vector<vector<Vec4i>> convDef(contours.size() );
		vector<vector<Point>> hull_points(contours.size());
		vector<vector<Point>> defect_points(contours.size());

		for( int i = 0; i < contours.size(); i++ )
		{
			if(contourArea(contours[i])>500)
			{
				convexHull( contours[i], hull[i], false );
				convexityDefects( contours[i],hull[i], convDef[i]);

				for(int k=0;k<hull[i].size();k++)
				{
					int ind=hull[i][k];
					hull_points[i].push_back(contours[i][ind]);
				}

				for(int k=0;k<convDef[i].size();k++)
				{
					if(convDef[i][k][3]>20*256) // filter defects by depth
					{
						numFingers++;
						int ind_0=convDef[i][k][0];
						int ind_1=convDef[i][k][1];
						int ind_2=convDef[i][k][2];
						defect_points[i].push_back(contours[i][ind_2]);
						cv::circle(drawing,contours[i][ind_0],5,Scalar(0,255,0),-1);
						cv::circle(drawing,contours[i][ind_1],5,Scalar(0,255,0),-1);
						cv::circle(drawing,contours[i][ind_2],5,Scalar(0,0,255),-1);
						cv::line(drawing,contours[i][ind_2],contours[i][ind_0],Scalar(0,0,255),1);
						cv::line(drawing,contours[i][ind_2],contours[i][ind_1],Scalar(0,0,255),1);
					}
				}

				drawContours( drawing, contours, i, Scalar(0,255,0), 1, 8, vector<Vec4i>(), 0, Point() );
				drawContours( drawing, hull_points, i, Scalar(255,0,0), 1, 8, vector<Vec4i>(), 0, Point() );
			}
		}
	}
	imshow( "Hull", drawing );
	return numFingers;
}


void Kinect::KinectZoom::zoom(cv::Mat frame,openni::VideoStream *m_depth, float x, float y, float z)
{
	openni::CoordinateConverter coordinateConverter;
	float x1;
	float y1;
	float x2;
	float y2;
	float z1;
	coordinateConverter.convertWorldToDepth(*m_depth, x-150.0,y-150.0,z, &x1, &y1, &z1);
	coordinateConverter.convertWorldToDepth(*m_depth, x+200.0,y+200.0,z, &x2, &y2, &z1);

	currentZ=z1;

	openni::VideoFrameRef depthFrame;
	m_depth->readFrame(&depthFrame);
	openni::DepthPixel* depthPixels = (openni::DepthPixel*)depthFrame.getData();
	cv::Mat depthImage(depthFrame.getHeight(), depthFrame.getWidth(), CV_16UC1, depthPixels);

	cv::Rect rect;
	rect.x = (int) x1;
	rect.y = frame.rows - (int) y1;
	rect.width = abs((int)(x1-x2));
	rect.height = abs((int)(y1-y2));

	if (rect.x<0) rect.x=0;
	if (rect.y<0) rect.y=0;
	if (rect.width<0) rect.width=0;
	if (rect.height<0) rect.height=0;
	if ((rect.x+rect.width)>frame.cols-1) rect.width =frame.cols-1-rect.x;
	if ((rect.y+rect.height)>frame.rows-1) rect.height = frame.rows-1-rect.y;

	depthImage = depthImage(rect);
	double minVal;
	double maxVal;
	cv::Point minLoc;
	cv::Point maxLoc;

	minMaxLoc( depthImage, &minVal, &maxVal, &minLoc, &maxLoc );

	cv::Mat depthImage2;
	depthImage.convertTo(depthImage2,CV_8UC1,255/maxVal);

	cv::Mat mask = cv::Mat::zeros(depthImage2.rows + 2, depthImage2.cols + 2, CV_8U);
	cv::floodFill(depthImage2, mask, cv::Point(depthImage2.cols/2,depthImage2.rows/2),
				  255, 0, cv::Scalar(4),
				  cv::Scalar(4),  4 + (255 << 8) + cv::FLOODFILL_MASK_ONLY + cv::FLOODFILL_FIXED_RANGE);

	cv::namedWindow( "floodfill", CV_WINDOW_AUTOSIZE );

	cv::imshow("floodfill", mask);
	cv::waitKey(33);

	int numFingers = DetectContour(mask);
	std::cout << "fingers " << numFingers << std::endl;
	if (numFingers > 2)
	{
		QOSG::ViewerQT *viewer = AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
		delta = (previousZ-currentZ);

		if( delta > 0 && delta > zoomThreshold)
		{
			std::cout << "delta " << delta << std::endl;
			viewer->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_UP,100);
		}
		else if (delta < 0 && delta < -1.0*zoomThreshold)
		{
			std::cout << "delta " << delta << std::endl;
			viewer->getEventQueue()->mouseScroll(osgGA::GUIEventAdapter::SCROLL_DOWN,100);
		}
	}
	previousZ=z1;
}
