
#include "Kinect/KinectThread.h"
#include "Kinect/KinectCore.h"
#include "Kinect/KinectRecognition.h"
#include "Kinect/KinectHandTracker.h"
#include "Kinect/KinectZoom.h"

#include "QDebug"

Kinect::KinectThread::KinectThread( QObject* parent ) : QThread( parent )
{
    //initialize based atributes
    mCancel=false;
    mSpeed=1.0;
    isCursorEnable=true;
    isOpen=false;
    mSetImageEnable=true;
    isZoomEnable=true;
}

Kinect::KinectThread::~KinectThread( void )
{
}

void Kinect::KinectThread::inicializeKinect()
{
    // create Openni connection
    mKinect = new Kinect::KinectRecognition();
    isOpen=mKinect->isOpenOpenni(); // checl if open

    qDebug() << "Kinect Thread inicializeKinect isOpen=" << isOpen ;
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
    //return isOpen;
}

void Kinect::KinectThread::closeActionOpenni()
{
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

void Kinect::KinectThread::run()
{

    mCancel=false;

    //real word convector
    openni::CoordinateConverter coordinateConverter;
    // convert milimeters to pixels
    float pDepth_x;
    float pDepth_y;
    float pDepth_z;
    float pDepth_x2;
    float pDepth_y2;
    float pDepth_z2;
    /////////end////////////
    Kinect::KinectZoom* zoom = new Kinect::KinectZoom();
    cv::Mat frame, depth;
    bool test = true;
    // check if is close
    while ( !mCancel ) {
        //check if is sending image enabling
        if ( mSetImageEnable ) {
            // read frame data
            color.readFrame( &colorFrame );
            //convert for sending
            frame=mKinect->colorImageCvMat( colorFrame );
            cv::cvtColor( frame, frame, CV_BGR2RGB );
            m_depth.readFrame( &depthFrame);
            if(test) {
                depth = mKinect->depthImageCvMat(depthFrame);

                cv::imwrite("C:\\Users\\Leachim\\Pictures\\frame1.jpg", frame);
                //cv::FileStorage file("C:\\Users\\Leachim\\Pictures\\depth.xml", cv::FileStorage::WRITE);
                //file << "matName" << depth;
                qDebug() << "Matrix row " << depth.rows << " and cols " << depth.cols;

                std::ofstream fout("C:\\Users\\Leachim\\Pictures\\depth1.txt");
                if(!fout)
                {
                       qDebug() <<"File Not Opened";
                }

                for(int i=0; i<depth.rows; i++)
                {
                    for(int j=0; j < depth.cols; j++)
                    {
                        fout << depth.at<uint16_t>(i,j)<<"\t";
                    }
                        fout << "\n";
                }

                cv::normalize(depth, depth, 0,255, CV_MINMAX, CV_8UC1);
                cv::imwrite("C:\\Users\\Leachim\\Pictures\\depth1.jpg", depth);

                fout.close();
                test =  false;
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


#ifdef NITE2_FOUND
            if ( kht->isTwoHands == true ) { //TODO must be two hands for green square mark hand in frame
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
            else {

                //sliding - calculate gesture
                kht->getRotatingMove();
                line( frame, cv::Point2i( 30, 30 ), cv::Point2i( 30, 30 ), cv::Scalar( 0, 0, 0 ), 5 ,8 );

                char* text;
                text = kht->slidingHand_type;
                if ( ( int )kht->slidingHand_x != 0 ) {
                    putText( frame, text, cvPoint( ( int )kht->slidingHand_x,( int )kht->slidingHand_y ), cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar( 0,0,250 ), 1, CV_AA );
                    //signal pre
                    emit sendSliderCoords( ( kht->slidingHand_x/kht->handTrackerFrame.getDepthFrame().getWidth()-0.5 )*( -200 ),
                                           ( kht->slidingHand_y/kht->handTrackerFrame.getDepthFrame().getHeight()-0.5 )*( 200 ),
                                           ( kht->slidingHand_z/kht->handTrackerFrame.getDepthFrame().getHeight()-0.5 )*200 );
                    // compute zoom if enabled
                    if ( isZoomEnable ) {
                        zoom->zoom( frame,&m_depth,kht->getArrayHands[0][0], kht->getArrayHands[0][1], kht->handZ[0] );
                    }
                    printf( "%.2lf %.2lf z %.2lf -  %.2lf slider \n", ( kht->slidingHand_x/kht->handTrackerFrame.getDepthFrame().getWidth()-0.5 )*200,
                            ( kht->slidingHand_y/kht->handTrackerFrame.getDepthFrame().getHeight()-0.5 )*200, ( kht->slidingHand_z/kht->handTrackerFrame.getDepthFrame().getHeight()-0.5 )*200, kht->slidingHand_z );
                }
            }
#endif
            // resize, send a msleep for next frame
            cv::resize( frame, frame,cv::Size( 320,240 ),0,0,cv::INTER_LINEAR );
            emit pushImage( frame );
            msleep( 20 );
        }

    }
}
