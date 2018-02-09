#include "OpenCV/MarkerlessTracker.h"
#include <stdlib.h>
#include <aruco.h>
#include <opencv2/imgproc/imgproc.hpp>

OpenCV::MarkerlessTracker::MarkerlessTracker( ArucoModul::ArControlClass* arControlClass )
{
	mArControlClass = new ArucoModul::ArControlClass();
	framenum=0;
	QString filename = "../share/3dsoftviz/config/camera.yml";
	mCamParam.readFromXMLFile( filename.toStdString() );
}

OpenCV::MarkerlessTracker::~MarkerlessTracker()
{
}

cv::RNG rng( 12345 );
float OpenCV::MarkerlessTracker::verifyCircle( cv::Mat dt, cv::Point2f center, float radius, std::vector<cv::Point2f>& inlierSet ) //prevzate z http://stackoverflow.com/questions/26222525/opencv-detect-partial-circle-with-noise
{
	unsigned int counter = 0;
	unsigned int inlier = 0;
	float minInlierDist = 2.0f;
	float maxInlierDistMax = 100.0f;
	float maxInlierDist = radius/25.0f;
	if ( maxInlierDist<minInlierDist ) {
		maxInlierDist = minInlierDist;
	}
	if ( maxInlierDist>maxInlierDistMax ) {
		maxInlierDist = maxInlierDistMax;
	}

	// choose samples along the circle and count inlier percentage
	for ( float t =0; t<2*3.14159265359f; t+= 0.05f ) {
		counter++;
		float cX = radius*cos( t ) + center.x;
		float cY = radius*sin( t ) + center.y;

		if ( cX < dt.cols )
			if ( cX >= 0 )
				if ( cY < dt.rows )
					if ( cY >= 0 )
						if ( dt.at<float>( static_cast<int>( cY ),static_cast<int>( cX ) ) < maxInlierDist ) {
							inlier++;
							inlierSet.push_back( cv::Point2f( cX,cY ) );
						}
	}

	return static_cast<float>( inlier )/ static_cast<float>( counter );
}



inline void OpenCV::MarkerlessTracker::getCircle( cv::Point2f& p1,cv::Point2f& p2,cv::Point2f& p3, cv::Point2f& center, float& radius ) //prevzate z http://stackoverflow.com/questions/26222525/opencv-detect-partial-circle-with-noise
{
	float x1 = p1.x;
	float x2 = p2.x;
	float x3 = p3.x;

	float y1 = p1.y;
	float y2 = p2.y;
	float y3 = p3.y;




	// Vypocet stredu a polomeru z troch bodo kruhu
	center.x = ( x1*x1+y1*y1 )*( y2-y3 ) + ( x2*x2+y2*y2 )*( y3-y1 ) + ( x3*x3+y3*y3 )*( y1-y2 );
	center.x /= ( 2*( x1*( y2-y3 ) - y1*( x2-x3 ) + x2*y3 - x3*y2 ) );


	center.y = ( x1*x1 + y1*y1 )*( x3-x2 ) + ( x2*x2+y2*y2 )*( x1-x3 ) + ( x3*x3 + y3*y3 )*( x2-x1 );
	center.y /= ( 2*( x1*( y2-y3 ) - y1*( x2-x3 ) + x2*y3 - x3*y2 ) );

	radius = sqrt( ( center.x-x1 )*( center.x-x1 ) + ( center.y-y1 )*( center.y-y1 ) );
}
std::vector<cv::Point2f> OpenCV::MarkerlessTracker::getPointPositions( cv::Mat binaryImage )
{
	std::vector<cv::Point2f> pointPositions;
	//unsigned char* rowPtr = binaryImage.ptr<unsigned char>(y);
	for ( int y=0; y<binaryImage.rows; ++y ) {
		for ( int x = 0; x < binaryImage.cols; ++x ) {
			//if(rowPtr[x] > 0) pointPositions.push_back(cv::Point2i(x,y));
			if ( binaryImage.at < unsigned char > ( y, x ) > 0 ) {
				pointPositions.push_back( cv::Point2f( static_cast<float>( x ),static_cast<float>( y ) ) );
			}
		}                   //
	}

	return pointPositions;
}


void OpenCV::MarkerlessTracker::track( cv::Mat& frame )
{
	//(x,y,radius)
	cv::vector<cv::Vec3f> circles;
	findCirclesInFrame( frame );


	//TODO read from config file
	float circle_width = 120.0; //mm

	// Handle found circles
	// TODO assign ids to circles, backtrack them
	for ( size_t i = 0; i < tBalls.size(); i++ ) {
		//(x,y,radius)
		cv::Point3f circle( cvRound( static_cast<double>( tBalls.at( i ).cent.x ) ), cvRound( static_cast<double>( tBalls.at( i ).cent.y ) ), cvRound( static_cast<double>( tBalls.at( i ).radius ) ) );

		cv::Mat rvec( 3, 1, CV_32FC1 );
		cv::Mat tvec( 3, 1, CV_32FC1 );
		estimatePoseCircle( osg::Vec2f( circle.x, circle.y ), circle_width/2, circle.z, mCamParam, rvec, tvec );

		QMatrix4x4 mVMatrix;
		calculateMVMatrixFromPose( rvec, tvec, mVMatrix );

		if ( mArControlClass != NULL ) {
			mArControlClass->updateObjectPositionAruco( 1, mVMatrix, true );
		}
	}

}


void OpenCV::MarkerlessTracker::findCirclesInFrame( cv::Mat& frame )  //trackuje lopty pomocou ransacu a historeie
{
	//	cv::resize(frame,frame,cv::Size(frame.rows/4,frame.cols/4));
	cv::flip( frame, frame, 1 );
	cv::Mat frame_gray;
	cv::cvtColor( frame, frame_gray, CV_BGR2GRAY );
	cv::Mat frame_bilateral;
	cv::bilateralFilter( frame_gray, frame_bilateral, 15, 30, 7.5 );
	cv::Mat mask;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::Canny( frame_gray, mask, 100, 200, 3 ); //ziskaie hran
	cv::findContours( mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point( 0, 0 ) ); //transfrmacia hran na kontury
	cv::Mat drawing;
	drawing =cv::Mat::zeros( mask.size(),CV_8UC1 );
	for ( unsigned int i = 0; i< contours.size(); i++ ) { //vytvoreni hranoveho obrazka
		cv::Scalar color = cv::Scalar( 255, 255, 255 );
		cv::drawContours( drawing, contours,static_cast<int>( i ), color, 1, 8, hierarchy, 0, cv::Point() );
	}
	mask=drawing;
	std::vector<cv::Point2f> edgePositions;
	edgePositions = getPointPositions( mask );
	cv::Mat dt;
	cv::distanceTransform( 255-mask, dt, CV_DIST_L1, 3 );
	cv::Point3f aktual;
//	int numOfBalls= tBalls.size();
	/*int numOfBalls= 0;
	for(int i=0;i<tBalls.size();i++){
		if(tBalls.at(i).found){
			numOfBalls+=1;
		}
	}*/
//	std::cout << "number of balls " << numOfBalls << std::endl;
	if ( tBalls.size()!=0 ) { //prechod histriou
		for ( unsigned int i=0; i<tBalls.size(); i++ ) {
			if ( tBalls.at( i ).found ) { //hladaj glicku k bola predtym najdena
				tBalls.at( i ).found=false;

				bool found=false;
				if ( tBalls.at( i ).history.size()>=5 ) {
					tBalls.at( i ).history.erase( tBalls.at( i ).history.begin() ); //odstran nadbytcne zaznami v historii pre guliku
				}
				float minCirclePercentage = 0.5f;
				for ( unsigned int g=0; g<tBalls.at( i ).history.size(); g++ ) {
					if ( found ) { //hladaj len pokial gulicka nebola nadena
						break;
					}
					aktual= tBalls.at( i ).history.at( g );

					cv::Point2f center;
					float radius;
					for ( int l = 0; l <10 ; ++l ) { //upravuj hodnoty polomeru
						for ( int j = 0; j < 10 ; j++ ) { //hodnou x
							center.x=aktual.x+static_cast<float>( j );
							for ( int k = 0; k < 10 ; k++ ) { //hodnotu y

								center.y=aktual.y+static_cast<float>( k );

								std::vector<cv::Point2f> inlierSet;
								radius=aktual.z+static_cast<float>( l );

								float cPerc = verifyCircle( dt, center, radius, inlierSet ); //ziskajfunkciou pecento bodov z fitovaneho kruhu

								if ( cPerc >= minCirclePercentage && radius>50 ) { //ak je perceto boov vysie ako hranica tak gulicka bola najdena

									//                            std::cout << "history circle with " << cPerc * 100.0f << " % inlier" << std::endl;
									//                            std::cout << "circle: " << "center: " << center << " radius: " << radius << std::endl;
									// first step would be to approximate the circle iteratively from ALL INLIER to obtain a better circle center
									// but that's a TODO


									cv::circle( frame, center, static_cast<int>( radius ), cv::Scalar( 255, 255, 0 ), 1 ); //vykresli kruh pre gulicku
									cv::circle( frame, center, 5, cv::Scalar( 255, 0, 0 ), 3 ); //vykresli kruh pre stred

									cv::circle( frame, center, static_cast<int>( radius ), cv::Scalar( 255, 255, 0 ), 1 );
									cv::circle( frame, center, 5, cv::Scalar( 255, 0, 0 ), 3 );

									// accept circle => remove it from the edge list
									cv::circle( mask, center, static_cast<int>( radius ), cv::Scalar( 0 ), 10 );

									//update edge positions and distance transform
									edgePositions = getPointPositions( mask );
									cv::distanceTransform( 255 - mask, dt, CV_DIST_L1, 3 );
									tBalls.at( i ).history.push_back( cv::Point3f( center.x,center.y,radius ) ); //zapamtaj si hodnty
									tBalls.at( i ).cent=center;
									tBalls.at( i ).radius=radius;
									tBalls.at( i ).lastInlierPerc=cPerc;
									tBalls.at( i ).found =true;
									tBalls.at( i ).numOfFramesnfound =0;
									found =true;

								}
								radius=aktual.z-static_cast<float>( l ); //zmena adius zaporna inac rovnaky proces ako napisan vyssie
								cPerc = verifyCircle( dt, center, radius, inlierSet );

								if ( cPerc >= minCirclePercentage && radius>50 && !found ) {
									//                            std::cout << "history circle with " << cPerc * 100.0f << " % inlier" << std::endl;
									//                            std::cout << "circle: " << "center: " << center << " radius: " << radius << std::endl;
									// first step would be to approximate the circle iteratively from ALL INLIER to obtain a better circle center
									// but that's a TODO


									cv::circle( frame, center,static_cast<int>( radius ), cv::Scalar( 255, 255, 0 ), 1 );
									cv::circle( frame, center, 5, cv::Scalar( 255, 0, 0 ), 3 );
									// accept circle => remove it from the edge list
									cv::circle( mask, center,static_cast<int>( radius ), cv::Scalar( 0 ), 10 );

									//update edge positions and distance transform
									edgePositions = getPointPositions( mask );
									cv::distanceTransform( 255 - mask, dt, CV_DIST_L1, 3 );
									tBalls.at( i ).history.push_back( cv::Point3f( center.x,center.y,radius ) );
									tBalls.at( i ).cent=center;
									tBalls.at( i ).radius=radius;
									tBalls.at( i ).lastInlierPerc=cPerc;
									tBalls.at( i ).found =true;
									tBalls.at( i ).numOfFramesnfound =0;
									found =true;

								}
								if ( found ) {
									break;
								}
							}
							if ( found ) {
								break;
							}
						}
						if ( found ) {
							break;
						}
					}
					if ( !found ) { //ta ista kontrola historie len so zapornou zmenou x a y posunov
						for ( int l = 0; l < 10; ++l ) {
							for ( int j = 1; j < 10; j++ ) {

								center.x = aktual.x -static_cast<float>( j );
								for ( int k = 1; k < 10; k++ ) {
									center.y = aktual.y - static_cast<float>( k );

									std::vector<cv::Point2f> inlierSet;
									radius = aktual.z + static_cast<float>( l );
									float cPerc = verifyCircle( dt, center, radius, inlierSet );
									//                        std::cout << "history circle with " << cPerc * 100.0f << " % inlier" << std::endl;
									//                        std::cout << "circle: " << "center: " << center << " radius: " << radius << std::endl;
									if ( cPerc >= minCirclePercentage && radius > 50 ) {

										// first step would be to approximate the circle iteratively from ALL INLIER to obtain a better circle center
										// but that's a TODO


										cv::circle( frame, center,static_cast<int>( radius ), cv::Scalar( 255, 255, 0 ), 1 );
										cv::circle( frame, center, 5, cv::Scalar( 255, 0, 0 ), 3 );
										// accept circle => remove it from the edge list
										cv::circle( mask, center,static_cast<int>( radius ), cv::Scalar( 0 ), 10 );

										//update edge positions and distance transform
										edgePositions = getPointPositions( mask );
										cv::distanceTransform( 255 - mask, dt, CV_DIST_L1, 3 );
										tBalls.at( i ).history.push_back( cv::Point3f( center.x, center.y, radius ) );
										tBalls.at( i ).cent=center;
										tBalls.at( i ).radius=radius;
										tBalls.at( i ).lastInlierPerc=cPerc;
										tBalls.at( i ).found =true;
										tBalls.at( i ).numOfFramesnfound =0;
										found = true;
									}
									radius = aktual.z - static_cast<float>( l );
									cPerc = verifyCircle( dt, center, radius, inlierSet );
									//                                                    std::cout << "history circle with " << cPerc * 100.0f << " % inlier" << std::endl;
									//                                                    std::cout << "circle: " << "center: " << center << " radius: " << radius << std::endl;
									if ( cPerc >= minCirclePercentage && radius > 50 && !found ) {

										// first step would be to approximate the circle iteratively from ALL INLIER to obtain a better circle center
										// but that's a TODO


										cv::circle( frame, center, static_cast<int>( radius ), cv::Scalar( 255, 255, 0 ), 1 );
										cv::circle( frame, center, 5, cv::Scalar( 255, 0, 0 ), 3 );
										// accept circle => remove it from the edge list
										cv::circle( mask, center,static_cast<int>( radius ), cv::Scalar( 0 ), 10 );

										//update edge positions and distance transform
										edgePositions = getPointPositions( mask );
										cv::distanceTransform( 255 - mask, dt, CV_DIST_L1, 3 );
										tBalls.at( i ).history.push_back( cv::Point3f( center.x, center.y, radius ) );
										tBalls.at( i ).cent=center;
										tBalls.at( i ).radius=radius;
										tBalls.at( i ).found =true;
										tBalls.at( i ).lastInlierPerc=cPerc;
										tBalls.at( i ).numOfFramesnfound =0;

										found = true;

									}
									if ( found ) {
										break;
									}
								}
								if ( found ) {
									break;
								}

							}
							if ( found ) {
								break;
							}
						}
					}

				}
				if ( found ) { //ak bola gulica najdena tak sa vykresl historia
					for ( unsigned int k=1; k<tBalls.at( i ).history.size(); k++ ) {
						cv::line( frame,cv::Point( static_cast<int>( tBalls.at( i ).history.at( k ).x ),static_cast<int>( tBalls.at( i ).history.at( k ).y ) ),cv::Point( static_cast<int>( tBalls.at( i ).history.at( k-1 ).x ),static_cast<int>( tBalls.at( i ).history.at( k-1 ).y ) ),cv::Scalar( 0,0,255 ),static_cast<int>( k ) );
					}
				}
			}
			else {
				tBalls.at( i ).numOfFramesnfound +=1;
				if ( tBalls.at( i ).numOfFramesnfound>=20 ) {
					tBalls.erase( tBalls.begin()+i );
				}
			}
		}
	}
	unsigned int nIterations = 0;
	unsigned int maxIterations =2000;
	/*if (tBalls.size()>0){
		maxIterations=1000;
	}*/
	framenum+=1;//pocitaj o ktory frame sa jedna
	if ( framenum>=2 ) { //a je piaty abo vyssi fame ak spra ransacna cely obraz
		framenum=0;
		while ( nIterations<=maxIterations ) { //vyber nahodne body z hraoveho obrazu
			unsigned int idx1 = static_cast<unsigned int>( rand() ) % static_cast<unsigned int>( edgePositions.size() );
			unsigned int idx2 = static_cast<unsigned int>( rand() ) % static_cast<unsigned int>( edgePositions.size() );
			unsigned int idx3 = static_cast<unsigned int>( rand() ) % static_cast<unsigned int>( edgePositions.size() );
			if ( idx1 == idx2 ) {
				continue;
			}
			if ( idx1 == idx3 ) {
				continue;
			}
			if ( idx3 == idx2 ) {
				continue;
			}
			cv::Point2f center;
			float radius;
			getCircle( edgePositions[idx1], edgePositions[idx2], edgePositions[idx3], center, radius ); //vypocitaj stred a polomer gulicky
			if ( isinf( radius ) ) {
				radius =1;
			}
			float minCirclePercentage = 0.7f;
			// inlier set unused at the moment but could be used to approximate a (more robust) circle from alle inlier
			std::vector<cv::Point2f> inlierSet;

			//verify or falsify the circle by inlier counting:
			float cPerc = verifyCircle( dt, center, radius, inlierSet );

			if ( cPerc >= minCirclePercentage && radius>60 ) {
				//                std::cout << "accepted circle with " << cPerc * 100.0f << " % inlier" << std::endl;
				// first step would be to approximate the circle iteratively from ALL INLIER to obtain a better circle center
				// but that's a TODO

				//                std::cout << "circle: " << "center: " << center << " radius: " << radius << std::endl;
				cv::circle( frame, center,static_cast<int>( radius ), cv::Scalar( 255, 255, 0 ), 1 );
				cv::circle( frame, center, 5, cv::Scalar( 255, 0, 0 ), 3 );

				// accept circle => remove it from the edge list
				cv::circle( mask, center, static_cast<int>( radius ), cv::Scalar( 0 ), 10 );

				//update edge positions and distance transform
				edgePositions = getPointPositions( mask );
				cv::distanceTransform( 255 - mask, dt, CV_DIST_L1, 3 );
				bool check= false;
				for ( unsigned int i=0; i<tBalls.size()&&!check; i++ ) {	//skontroluj ci sa gulicka nachadza tam kde sa nachadzala nejaka predchadzajuca gulicka
					if ( ( abs( tBalls.at( i ).cent.x-center.x )<50 ) &&( abs( tBalls.at( i ).cent.y-center.y )<50 ) && ( abs( tBalls.at( i ).radius-radius )<50 ) && ( !tBalls.at( i ).found ) ) {
						cv::Point3f location;
						location.x=tBalls.at( i ).cent.x;
						location.y=tBalls.at( i ).cent.y;
						location.z=tBalls.at( i ).radius;
						tBalls.at( i ).history.push_back( location );
						tBalls.at( i ).cent=center;
						tBalls.at( i ).radius=radius;
						tBalls.at( i ).lastInlierPerc=cPerc;
						tBalls.at( i ).found =true;
						tBalls.at( i ).numOfFramesnfound =0;
						for ( unsigned int k=1; k<tBalls.at( i ).history.size(); k++ ) {
							cv::line( frame,cv::Point( static_cast<int>( tBalls.at( i ).history.at( k ).x ),static_cast<int>( tBalls.at( i ).history.at( k ).y ) ),cv::Point( static_cast<int>( tBalls.at( i ).history.at( k-1 ).x ),static_cast<int>( tBalls.at( i ).history.at( k-1 ).y ) ),cv::Scalar( 0,0,255 ),static_cast<int>( k ) );
						}
						check=true;
					}
				}
				if ( !check ) {
					TrackerBall help;
					help.cent=center;
					help.radius=radius;
					help.lastInlierPerc=cPerc;
					help.id=static_cast<int>( tBalls.size()+1 );
					cv::Point3f location;
					location.x=center.x;
					location.y=center.y;
					location.z=radius;
					help.found =true;
					help.numOfFramesnfound =0;
					help.history.push_back( location );

					tBalls.push_back( help );
				}
			}


			cv::Mat tmp;
			mask.copyTo( tmp );

			// prevent cases where no fircle could be extracted (because three points collinear or sth.)
			// filter NaN values
			if ( qFuzzyCompare( center.x, center.x ) && qFuzzyCompare( center.y, center.y ) && qFuzzyCompare( radius, radius ) ) {
				cv::circle( tmp, center,static_cast<int>( radius ), cv::Scalar( 255 ) );
				//            cv::namedWindow("RANSAC"); cv::imshow("RANSAC", tmp);
			}
			else {
				//                std::cout << "circle illegal" << std::endl;
			}
			++nIterations;

		}
	}


}

//JMA
void OpenCV::MarkerlessTracker::estimatePoseCircle( osg::Vec2f framePosition,
		float circleRadiusReal,
		float circleRadiusFrame,
		aruco::CameraParameters mCamParam,
		cv::Mat& rvec, cv::Mat& tvec
												  )
{
	/*
	 * Graph geometry - coordinates of points where object shouldbe rendered
	 */
	cv::Mat objectPoints( 4, 3, CV_32FC1 );
	objectPoints.at< float >( 0, 0 ) = 0;
	objectPoints.at< float >( 0, 1 ) = circleRadiusReal;
	objectPoints.at< float >( 0, 2 ) = 0;
	objectPoints.at< float >( 1, 0 ) = circleRadiusReal;
	objectPoints.at< float >( 1, 1 ) = 0;
	objectPoints.at< float >( 1, 2 ) = 0;
	objectPoints.at< float >( 2, 0 ) = 0;
	objectPoints.at< float >( 2, 1 ) = -circleRadiusReal;
	objectPoints.at< float >( 2, 2 ) = 0;
	objectPoints.at< float >( 3, 0 ) = -circleRadiusReal;
	objectPoints.at< float >( 3, 1 ) = 0;
	objectPoints.at< float >( 3, 2 ) = 0;

	/*
	 * Frame geometry - coordinates of points where object is found on the frame
	 */
	cv::Mat imagePoints( 4, 2, CV_32FC1 );
	imagePoints.at< float >( 0, 0 ) = framePosition.x();
	imagePoints.at< float >( 0, 1 ) = framePosition.y()+circleRadiusFrame;
	imagePoints.at< float >( 1, 0 ) = framePosition.x()+circleRadiusFrame;
	imagePoints.at< float >( 1, 1 ) = framePosition.y();
	imagePoints.at< float >( 2, 0 ) = framePosition.x();
	imagePoints.at< float >( 2, 1 ) = framePosition.y()-circleRadiusFrame;
	imagePoints.at< float >( 3, 0 ) = framePosition.x()-circleRadiusFrame;
	imagePoints.at< float >( 3, 1 ) = framePosition.y();

	/*
	 * Rotation and translation vectors(matrices)
	 */
	cv::Mat raux, taux;
	cv::solvePnP( objectPoints, imagePoints, mCamParam.CameraMatrix, mCamParam.Distorsion, raux, taux );
	raux.convertTo( rvec, CV_32F );
	taux.convertTo( tvec, CV_32F );
}

//JMA
void OpenCV::MarkerlessTracker::calculateMVMatrixFromPose( cv::Mat rvec, cv::Mat tvec,
		QMatrix4x4& mVMatrix
														 )
{
	/*
	 * convert rvec(rotation vector 3x1) to rotaion matrix(3x3)
	 */
	cv::Mat rotationMatrix( 3,3,cv::DataType<double>::type );
	cv::Rodrigues( rvec,rotationMatrix );

	QMatrix4x4 matrix(
		rotationMatrix.at< float >( 0, 0 ),
		rotationMatrix.at< float >( 1, 0 ),
		-rotationMatrix.at< float >( 2, 0 ),
		0.0,
		rotationMatrix.at< float >( 0, 1 ),
		rotationMatrix.at< float >( 1, 1 ),
		-rotationMatrix.at< float >( 2, 1 ),
		0.0,
		rotationMatrix.at< float >( 0, 2 ),
		rotationMatrix.at< float >( 1, 2 ),
		-rotationMatrix.at< float >( 2, 2 ),
		0.0,
		tvec.at< float >( 0, 0 ),
		tvec.at< float >( 1, 0 ),
		-tvec.at< float >( 2, 0 ),
		1.0
	);

	mVMatrix = matrix;
}
