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
float verifyCircle(cv::Mat dt, cv::Point2f center, float radius, std::vector<cv::Point2f> & inlierSet)
{
	unsigned int counter = 0;
	unsigned int inlier = 0;
	float minInlierDist = 2.0f;
	float maxInlierDistMax = 100.0f;
	float maxInlierDist = radius/25.0f;
	if(maxInlierDist<minInlierDist) maxInlierDist = minInlierDist;
	if(maxInlierDist>maxInlierDistMax) maxInlierDist = maxInlierDistMax;

	// choose samples along the circle and count inlier percentage
	for(float t =0; t<2*3.14159265359f; t+= 0.05f)
	{
		counter++;
		float cX = radius*cos(t) + center.x;
		float cY = radius*sin(t) + center.y;

		if(cX < dt.cols)
			if(cX >= 0)
				if(cY < dt.rows)
					if(cY >= 0)
						if(dt.at<float>(cY,cX) < maxInlierDist)
						{
							inlier++;
							inlierSet.push_back(cv::Point2f(cX,cY));
						}
	}

	return (float)inlier/float(counter);
}


inline void getCircle(cv::Point2f& p1,cv::Point2f& p2,cv::Point2f& p3, cv::Point2f& center, float& radius)
{
	float x1 = p1.x;
	float x2 = p2.x;
	float x3 = p3.x;

	float y1 = p1.y;
	float y2 = p2.y;
	float y3 = p3.y;




		// PLEASE CHECK FOR TYPOS IN THE FORMULA :)
		center.x = (x1*x1+y1*y1)*(y2-y3) + (x2*x2+y2*y2)*(y3-y1) + (x3*x3+y3*y3)*(y1-y2);
		center.x /= ( 2*(x1*(y2-y3) - y1*(x2-x3) + x2*y3 - x3*y2) );

		center.y = (x1*x1 + y1*y1)*(x3-x2) + (x2*x2+y2*y2)*(x1-x3) + (x3*x3 + y3*y3)*(x2-x1);
		center.y /= ( 2*(x1*(y2-y3) - y1*(x2-x3) + x2*y3 - x3*y2) );

		radius = sqrt((center.x-x1)*(center.x-x1) + (center.y-y1)*(center.y-y1));
}
std::vector<cv::Point2f> getPointPositions(cv::Mat binaryImage)
{
	std::vector<cv::Point2f> pointPositions;
	//unsigned char* rowPtr = binaryImage.ptr<unsigned char>(y);
	for(unsigned int y=0; y<binaryImage.rows; ++y) {
		for (unsigned int x = 0; x < binaryImage.cols; ++x) {
			//if(rowPtr[x] > 0) pointPositions.push_back(cv::Point2i(x,y));
			if (binaryImage.at < unsigned char > (y, x) > 0){ pointPositions.push_back(cv::Point2f(x, y)); }
		}                   //
	}

return pointPositions;
}


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

void OpenCV::MarkerlessTracker::findCirclesInFrame( cv::Mat& frame ){
//	cv::resize(frame,frame,cv::Size(frame.rows/4,frame.cols/4));
	cv::flip( frame, frame, 1 );
	cv::Mat frame_gray;
    cv::cvtColor( frame, frame_gray, CV_BGR2GRAY );
    cv::Mat frame_bilateral;
    cv::bilateralFilter( frame_gray, frame_bilateral, 15, 30, 7.5);
	cv::Mat mask;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	cv::Canny(gray, mask, 100, 200, 3);
	cv::findContours( mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
	cv::Mat drawing;
	cv::drawing =cv::Mat::zeros(mask.size(),CV_8UC1);
	for( int i = 0; i< contours.size(); i++ )
	{
	cv::Scalar color = cv::Scalar( 255, 255, 255 );
	cv::drawContours( drawing, contours, i, color, 1, 8, hierarchy, 0, Point() );
	}
	mask=drawing;
	std::vector<cv::Point2f> edgePositions;
	edgePositions = getPointPositions(mask);
	cv::Mat dt;
	cv::distanceTransform(255-mask, dt, CV_DIST_L1, 3);
	unsigned int nIterations = 0;
	cv::Point3f aktual;
	int numOfBalls= tBalls.size();
	if (numOfBalls!=0){
		for(int i=0;i<numOfBalls;i++){
			bool found=false;
			if(tBalls.at(i).history.size()>=5){
					tBalls.at(i).history.erase(tBalls.at(i).history.begin());
					}
			float minCirclePercentage = 0.5f;
			for(int g=0;g<tBalls.at(i).history.size();g++){
				if(found){
					break;
				}
				aktual= tBalls.at(i).history.at(g);

				cv::Point2f center;
				float radius;
				for (int l = 0; l <3 ; ++l) {
					for (int j = 0; j < 5 ; j++) {
						center.x=aktual.x+j;
						for (int k = 0; k < 5 ; k++) {
							center.y=aktual.y+k;

							std::vector<cv::Point2f> inlierSet;
							radius=aktual.z+l;
							float cPerc = verifyCircle(dt, center, radius, inlierSet);

							if (cPerc >= minCirclePercentage && radius>50) {
								//                            std::cout << "history circle with " << cPerc * 100.0f << " % inlier" << std::endl;
								//                            std::cout << "circle: " << "center: " << center << " radius: " << radius << std::endl;
								// first step would be to approximate the circle iteratively from ALL INLIER to obtain a better circle center
								// but that's a TODO


								cv::circle(frame, center, radius, cv::Scalar(255, 255, 0), 1);

								// accept circle => remove it from the edge list
								cv::circle(mask, center, radius, cv::Scalar(0), 10);

								//update edge positions and distance transform
								edgePositions = getPointPositions(mask);
								cv::distanceTransform(255 - mask, dt, CV_DIST_L1, 3);
								tBalls.at(i).history.push_back(cv::Point3f(center.x,center.y,radius));
								found =true;

							}
							radius=aktual.z-l;
							cPerc = verifyCircle(dt, center, radius, inlierSet);

							if (cPerc >= minCirclePercentage && radius>50 && !found) {
								//                            std::cout << "history circle with " << cPerc * 100.0f << " % inlier" << std::endl;
								//                            std::cout << "circle: " << "center: " << center << " radius: " << radius << std::endl;
								// first step would be to approximate the circle iteratively from ALL INLIER to obtain a better circle center
								// but that's a TODO


								cv::circle(frame, center, radius, cv::Scalar(255, 255, 0), 1);

								// accept circle => remove it from the edge list
								cv::circle(mask, center, radius, cv::Scalar(0), 10);

								//update edge positions and distance transform
								edgePositions = getPointPositions(mask);
								cv::distanceTransform(255 - mask, dt, CV_DIST_L1, 3);
								tBalls.at(i).history.push_back(cv::Point3f(center.x,center.y,radius));
								found =true;

							}
							if(found){
								break;
							}
						}
						if(found){
							break;
						}
					}
					if(found){
						break;
					}
				}
				if(!found) {
					for (int l = 0; l < 3; ++l) {
						for (int j = 1; j < 5; j++) {
							center.x = aktual.x - j;
							for (int k = 1; k < 5; k++) {
								center.y = aktual.y - k;

								std::vector<cv::Point2f> inlierSet;
								radius = aktual.z + l;
								float cPerc = verifyCircle(dt, center, radius, inlierSet);
								//                        std::cout << "history circle with " << cPerc * 100.0f << " % inlier" << std::endl;
								//                        std::cout << "circle: " << "center: " << center << " radius: " << radius << std::endl;
								if (cPerc >= minCirclePercentage && radius > 50) {

									// first step would be to approximate the circle iteratively from ALL INLIER to obtain a better circle center
									// but that's a TODO


									cv::circle(frame, center, radius, cv::Scalar(255, 255, 0), 1);

									// accept circle => remove it from the edge list
									cv::circle(mask, center, radius, cv::Scalar(0), 10);

									//update edge positions and distance transform
									edgePositions = getPointPositions(mask);
									cv::distanceTransform(255 - mask, dt, CV_DIST_L1, 3);
									tBalls.at(i).history.push_back(cv::Point3f(center.x, center.y, radius));
									found = true;
								}
								radius = aktual.z - l;
								cPerc = verifyCircle(dt, center, radius, inlierSet);
								//                                                    std::cout << "history circle with " << cPerc * 100.0f << " % inlier" << std::endl;
								//                                                    std::cout << "circle: " << "center: " << center << " radius: " << radius << std::endl;
								if (cPerc >= minCirclePercentage && radius > 50 && !found) {

									// first step would be to approximate the circle iteratively from ALL INLIER to obtain a better circle center
									// but that's a TODO


									cv::circle(frame, center, radius, cv::Scalar(255, 255, 0), 1);

									// accept circle => remove it from the edge list
									cv::circle(mask, center, radius, cv::Scalar(0), 10);

									//update edge positions and distance transform
									edgePositions = getPointPositions(mask);
									cv::distanceTransform(255 - mask, dt, CV_DIST_L1, 3);
									tBalls.at(i).history.push_back(cv::Point3f(center.x, center.y, radius));
									found = true;

								}
								if(found){
									break;
								}
							}
							if(found){
								break;
							}

						}
						if(found){
							break;
						}
					}
				}

			}
		}

	}
	unsigned int nIterations = 0;
	 while (nIterations<=2000 and !found) {

	 }


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
