#include "Kinect/KinectRecognition.h"
#include <QDebug>

Kinect::KinectRecognition::KinectRecognition()
{
	this->inicialize();
}

void Kinect::KinectRecognition::inicialize()
{
	isOpen=true;

	//initialize OpenNI
	mOpenniStatus=openni::OpenNI::initialize();
	if ( mOpenniStatus != openni::STATUS_OK ) { // check
		qDebug() << "Openni initialization failed";
		isOpen=false;
	}

	// open device - here we can select specific device - in Microsoft Kinect 0,1,...
	mOpenniStatus=device.open( openni::ANY_DEVICE );
	//check
	if ( mOpenniStatus != openni::STATUS_OK ) {
		qDebug() << "Openni Device not Open";
		isOpen=false;
	}
}

void Kinect::KinectRecognition::closeOpenni()
{
	// close device and shutdown Openni
	device.close();
	openni::OpenNI::shutdown();
}

Kinect::KinectRecognition::~KinectRecognition()
{
	device.close();
	openni::OpenNI::shutdown();
}

cv::Mat Kinect::KinectRecognition::colorImageCvMat( const openni::VideoFrameRef& colorFrame )
{
	cv::Mat frame;
	// read specific data from color frame
	const openni::RGB888Pixel* imageBuffer = static_cast<const openni::RGB888Pixel*>( colorFrame.getData() );
	// create cv mat frame
	frame.create( colorFrame.getHeight(), colorFrame.getWidth(), CV_8UC3 );
	// copy data
	memcpy( frame.data, imageBuffer, static_cast<size_t>( 3*colorFrame.getHeight()*colorFrame.getWidth() )*sizeof( uint8_t ) );

	return frame;
}

cv::Mat Kinect::KinectRecognition::depthImageCvMat( const openni::VideoFrameRef& depthFrame )
{
	cv::Mat frame;
	//read specific data from depth frame
	const openni::DepthPixel* imageBuffer = static_cast<const openni::DepthPixel*>( depthFrame.getData() );
	//create cv mat frame
	frame.create( depthFrame.getHeight(), depthFrame.getWidth(), CV_16UC1 );
	//copy data
	memcpy( frame.data, imageBuffer, static_cast<size_t>( depthFrame.getHeight()*depthFrame.getWidth() )*sizeof( uint16_t ) );

	return frame;
}

QImage Kinect::KinectRecognition::colorImageQImage( const openni::VideoFrameRef& colorFrame )
{
	// create QImage from videframeref
	QImage image = QImage( static_cast<const uchar*>( colorFrame.getData() ),
						   colorFrame.getWidth(), colorFrame.getHeight(),
						   QImage::Format_RGB888 ); // format color
	// rgb swapped
	image = image.rgbSwapped();
	return image;

}

QImage Kinect::KinectRecognition::deptImageQImage( const openni::VideoFrameRef& colorFrame )
{
	cv::Mat depthMat = depthImageCvMat( colorFrame );
	QImage image = QImage( depthMat.data,
						   colorFrame.getWidth(), colorFrame.getHeight(),
						   QImage::Format_Indexed8 ); // color white black
	// convert to format
	return image.convertToFormat( QImage::Format_RGB32 );
}

bool Kinect::KinectRecognition::isOpenOpenni()
{
	return isOpen;
}
