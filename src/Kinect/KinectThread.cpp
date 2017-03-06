
#include "Kinect/KinectThread.h"
#include "Kinect/KinectCore.h"
#include "Kinect/KinectRecognition.h"
#include "Kinect/KinectHandTracker.h"
#include "Kinect/KinectZoom.h"

#include "QDebug"
#include <string>
#include <time.h>

Kinect::KinectThread::KinectThread( QObject* parent ) : QThread( parent )
{
	//initialize based atributes
	mCancel=false;
	mSpeed=1.0;
	isCursorEnable=true;
	isOpen=false;
	mSetImageEnable=true;
	isZoomEnable=true;
	isMarkerDetectEnable=false;

	// timer setting
	clickTimer = new QTimer();
	connect( clickTimer ,SIGNAL( timeout() ), this, SLOT( clickTimerTimeout() ) );
	connect( this ,SIGNAL( signalClickTimerStop() ), this, SLOT( clickTimerStop() ) );
	connect( this ,SIGNAL( signalClickTimerStart() ), this, SLOT( clickTimerStart() ) );
	clickTimerFirstRun = true;

	nav = new Vwr::GraphNavigation();
	mouse = new Vwr::MouseControl();
}

Kinect::KinectThread::~KinectThread( void )
{
}

void Kinect::KinectThread::inicializeKinect()
{

	// create Openni connection
	mKinect = new Kinect::KinectRecognition();
	isOpen=mKinect->isOpenOpenni(); // checl if open

	qDebug() << "Kinect Thread inicialize. Kinect isOpen=" << isOpen ;
	if ( isOpen ) {
		// color data for Kinect windows
		color.create( mKinect->device, openni::SENSOR_COLOR );
		color.start(); // start
		// depth data for Hand Tracking
		m_depth.create( mKinect->device, openni::SENSOR_DEPTH );
		m_depth.start();

		//create hand tracker, TODO 2. parameter remove - unused
#ifdef NITE2_FOUND
		kht = new KinectHandTracker( &mKinect->device,&m_depth );
#endif
	}
}

void Kinect::KinectThread::closeActionOpenni()
{
	qDebug() << "Close Openni";
	mKinect->closeOpenni();
}

void Kinect::KinectThread::setCancel( bool set )
{
	mCancel=set;
}

void Kinect::KinectThread::setImageSend( bool set )
{
	mSetImageEnable=set;
}

void Kinect::KinectThread::setImageSendToMarkerDetection( bool set )
{
	isMarkerDetectEnable = set;
}

void Kinect::KinectThread::pause()
{
	mCancel=true;
}

void Kinect::KinectThread::setCursorMovement( bool set )
{
	isCursorEnable=set;
}
void Kinect::KinectThread::setZoomUpdate( bool set )
{
	isZoomEnable=set;
}
void Kinect::KinectThread::setSpeedKinect( double set )
{
	mSpeed=set;
}

void Kinect::KinectThread::setCaptureImage( bool set )
{
	qDebug() << "captureImage set to " << set;
	captureImage = set;
}

void Kinect::KinectThread::clickTimerTimeout()
{
	if ( clickTimerFirstRun ) {
		clickTimerFirstRun = false;
		clickTimer->start( 1500 );
	}
	else {
		clickTimer->stop();
	}
	//printf( "Timer timedout, timer stop\n" );
}
void Kinect::KinectThread::clickTimerStart()
{
	clickTimerFirstRun = true;
	clickTimer->start( 500 );

	//printf( "Hand closed, timer start\n" );
}
void Kinect::KinectThread::clickTimerStop()
{
	clickTimer->stop();

	// if gesture is click ( less than 500ms )
	if ( clickTimerFirstRun ) {
		// if navigation is enabled, do navigation click
		if ( nav->isNavEnabled ) {
			nav->selectNearestNode();
		}
		// else do basic click
		else {
			mouse->clickMouseLeftButton();
		}
	}
	// else gesture is remove ( less than 1000ms of second run )
	else {
		nav->removeLastSelectedNode();
	}

	//printf( "Hand opened, timer stop\n" );
}

void Kinect::KinectThread::run()
{
	struct tm timeinfo;
	char currtime[80];
	// flag for timer
	bool wasTimerReset = true;
	mCancel=false;

	//real word convector
	/*	openni::CoordinateConverter coordinateConverter;
		// convert milimeters to pixels
		float pDepth_x;
		float pDepth_y;
		float pDepth_z;
		float pDepth_x2;
		float pDepth_y2;
		float pDepth_z2;
	    /////////end////////////*/
	Kinect::KinectZoom* zoom = new Kinect::KinectZoom();
	cv::Mat frame;
	cv::Mat depth;

	//if set true, it will capture first frame of kinect stream and save color frame, depth frame and depth matrix in to specific location
	bool test = false;
	// check if is close
	while ( !mCancel ) {
		//check if is sending image enabling
		if ( mSetImageEnable ) {
			// read frame data
			color.readFrame( &colorFrame );
			//convert for sending
			frame=mKinect->colorImageCvMat( colorFrame );
			cv::cvtColor( frame, frame, CV_BGR2RGB );
			m_depth.readFrame( &depthFrame );
			depth = mKinect->depthImageCvMat( depthFrame );
			//if set true, it will capture the first frame of kinect stream and save color frame, depth frame and depth matrix in to specific location
			if ( captureImage ) {

				time_t now = std::time( 0 );
				timeinfo = *localtime( &now );
				strftime( currtime,80,"%Y-%m-%d %I:%M:%S",&timeinfo );

				std::string strTime( currtime );
				std::replace( strTime.begin(), strTime.end(), ':', '_' );

				depth = mKinect->depthImageCvMat( depthFrame );

				std::string file = Util::ApplicationConfig::get()->getValue( "Kinect.OutputFolder" ).toStdString();

				//save color frame
				cv::imwrite( file + "\\" +Util::ApplicationConfig::get()->getValue( "Kinect.ColourImageName" ).toStdString() + strTime + ".jpeg" , frame );

				//save depth matrix
				std::ofstream fout( file + "\\" +Util::ApplicationConfig::get()->getValue( "Kinect.DepthInfoName" ).toStdString() + strTime + ".txt" );
				if ( !fout ) {
					qDebug() <<"File Not Opened";
				}

				for ( int i=0; i<depth.rows; i++ ) {
					for ( int j=0; j < depth.cols; j++ ) {
						fout << depth.at<uint16_t>( i,j )<<"\t";
					}
					fout << "\n";
				}

				cv::normalize( depth, depth, 0,255, CV_MINMAX, CV_8UC1 );
				//save depth frame
				cv::imwrite( file + "\\" + Util::ApplicationConfig::get()->getValue( "Kinect.DepthImageName" ).toStdString() + strTime + ".jpg", depth );

				fout.close();
				captureImage =  false;
			}


#ifdef NITE2_FOUND
			//set parameters for changes movement and cursor
			kht->setCursorMovement( isCursorEnable );
			kht->setSpeedMovement( mSpeed );
			// cita handframe, najde gesto na snimke a vytvori mu "profil"
			kht->getAllGestures();
			kht->getAllHands();
#endif
			//////////////End/////////////

			//	cap >> frame; // get a new frame from camera
			cv::cvtColor( frame, frame, CV_BGR2RGB );

#ifdef NITE2_FOUND
			/*			if ( kht->isTwoHands == true ) { //TODO must be two hands for green square mark hand in frame
							// convert hand coordinate
							coordinateConverter.convertWorldToDepth( m_depth, kht->getArrayHands[0][0], kht->getArrayHands[0][1], kht->handZ[0], &pDepth_x, &pDepth_y, &pDepth_z );
							coordinateConverter.convertWorldToDepth( m_depth, kht->getArrayHands[1][0], kht->getArrayHands[1][1], kht->handZ[1], &pDepth_x2, &pDepth_y2, &pDepth_z2 );

							pDepth_y = kht->handTrackerFrame.getDepthFrame().getHeight() - pDepth_y;
							pDepth_y2 = kht->handTrackerFrame.getDepthFrame().getHeight() - pDepth_y2;

							printf( "depth X, Y, Z: %f %f %f\n",pDepth_x,pDepth_y,pDepth_z );

							// green square for hand
							cv::Rect hand_rect;

							if ( pDepth_x < pDepth_x2 ) {
								hand_rect.x = pDepth_x;
							}
							else {
								hand_rect.x = pDepth_x2;
							}
							if ( pDepth_y < pDepth_y2 ) {
								hand_rect.y = pDepth_y;
							}
							else {
								hand_rect.y = pDepth_y2;
							}

							hand_rect.width = abs( pDepth_x - pDepth_x2 );
							hand_rect.height = abs( pDepth_y - pDepth_y2 ); //kht->handY[1] - kht->handY[0];

							rectangle( frame, hand_rect, CV_RGB( 0, 255,0 ), 3 );
						}
			            else {*/
			// hand found
			if ( ( kht->numHandsTracking != 0 )&&( kht->numHandsTracking < 3 ) ) {
				int numFingers[2];

				// calculate num fingers for every tracked hand
				for ( int i = 0; i < kht->numHandsTracking; i++ ) {
					bool mainHand = false;
					if ( i==0 ) {
						mainHand = true;
					}
					// calculate depth frame
					zoom->calcHandDepthFrame( frame,&m_depth,kht->getArrayHands[i][0], kht->getArrayHands[i][1], kht->handZ[i], mainHand );
					// calculate num of fingers
					numFingers[i] = zoom->DetectContour();
				}

				// cursor disabled => move graph
				if ( !isCursorEnable ) {
					//sliding - calculate main hand movement
					kht->getRotatingMove();

					// two hand gestures
					if ( kht->numHandsTracking == 2 ) {
						// if off hand is open -> move graph by main hand
						if ( numFingers[1] != 0 ) {
							// open main hand -> move by X/Y axis
							if ( numFingers[0] != 0 ) {
								kht->moveGraphByHand( );
							}
							// closed main hand -> move by Z axis
							else {
								kht->moveGraphByHandToDepth( zoom->previousZ - zoom->currentZ );
							}
						}
					}
					// one hand gestures
					else {
						// if hand not closed - rotate
						if ( numFingers[0] != 0 ) {
							line( frame, cv::Point2i( 30, 30 ), cv::Point2i( 30, 30 ), cv::Scalar( 0, 0, 0 ), 5 ,8 );
							if ( static_cast<int>( kht->slidingHand_x ) != 0 ) {
								putText( frame, kht->slidingHand_type, cvPoint( static_cast<int>( kht->slidingHand_x ),static_cast<int>( kht->slidingHand_y ) ), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar( 0,0,250 ), 1, CV_AA );
								emit sendSliderCoords( ( kht->slidingHand_x / static_cast<float>( kht->handTrackerFrame.getDepthFrame().getWidth() ) - 0.5f ) * ( -200.0f ),
													   ( kht->slidingHand_y / static_cast<float>( kht->handTrackerFrame.getDepthFrame().getHeight() ) - 0.5f ) * ( 200.0f ),
													   ( kht->slidingHand_z / static_cast<float>( kht->handTrackerFrame.getDepthFrame().getHeight() ) - 0.5f ) * ( 200.0f ) );
							}
						}
						// if hand is closed and zomm enabled - compute zoom
						else if ( isZoomEnable ) {
							zoom->zoom();
						}
					}
				}
				// cursor enabled => move cursor
				else {
					// start highlighting neighbour nodes
					//nav->setSelectionMode( 2 );
					nav->navigate();

					// detect click gesture
					if ( wasTimerReset ) {
						// if main hand closed and timer inactive, start timer
						if ( numFingers[0] == 0 ) {
							emit signalClickTimerStart();
							// to prevent restart in cycle
							wasTimerReset = false;
						}
					}
					// if timer is not ready
					else if ( !wasTimerReset ) {
						// if main hand open
						if ( numFingers[0] > 3 ) {
							// if timer is active, stop timer and do gesture
							if ( clickTimer->isActive() ) {
								wasTimerReset = true;
								emit signalClickTimerStop();
							}
							// gesture has ended, restart timer to enable next gesture
							else {
								wasTimerReset = true;
							}
						}
					}

					// else do basic mouse gestures
					else {

					}
				}
			}
			//}
#endif
			// resize, send a msleep for next frame
			cv::resize( frame, frame,cv::Size( 320,240 ),0,0,cv::INTER_LINEAR );
			emit pushImage( frame );
			msleep( 20 );
		}
	}
}
