#include "OpenCV/CameraStream.h"
#include <QDebug>
#include <opencv2/highgui/highgui.hpp>
#include "Util/ApplicationConfig.h"


namespace OpenCV {

CameraStream::CameraStream( osg::Geometry* geom ) :
	QObject(),
	osg::Image(),

	mWidth( 0 ),
	mHeight( 0 ),
	mGeom( geom ),
	iplImg( nullptr )
{
#ifdef WIN32
	cv::Mat cvImg( 480,640, CV_8UC3, CV_RGB( 0,0,0 ) ); // Black on Win
#else
	cv::Mat cvImg;			// gray on Linux
#endif

	updateBackgroundImage( cvImg );

}

CameraStream::~CameraStream() {}

void CameraStream::updateBackgroundImage( cv::Mat cvImg )
{
	if ( cvImg.empty() ) {
		qDebug() << "CameraStream::updateBackgroundImage(): warning, cvImg is empty!";
		return;
	}

	if ( cvImg.cols != mWidth || mHeight != cvImg.rows ) {
		mWidth	= cvImg.cols;
		mHeight = cvImg.rows;
#ifdef WIN32		
		iplImg=cvCloneImage( &( IplImage )cvImg );
#endif
		// update geometry coordinates if thare are different dimensions of image,
		// becasuse probebly changed it ratio of sides
		if ( mGeom != NULL ) {
			updateGeometryCoords( mWidth, mHeight );
		}
	}


	// There will be probably needed refactoring on MAC OS
#ifdef WIN32
	cvCopy( &( IplImage )cvImg, iplImg, NULL );

	setImage( iplImg->width, iplImg->height,
			  3, GL_RGB, GL_RGB,
			  GL_UNSIGNED_BYTE,
			  ( unsigned char* ) iplImg->imageData,
			  osg::Image::NO_DELETE, 1 );

	cvImg.~Mat();

#else
	cv::Mat img = cvImg.clone();

	setImage( img.cols, img.rows,
			  8, img.channels(), GL_RGB,
			  GL_UNSIGNED_BYTE,
			  img.data,
			  NO_DELETE, 1 );

	//cvImg.~Mat();  // must be tested on linux for memory leaks
#endif

	dirty();

}

void CameraStream::updateGeometryCoords( int width, int height )
{
	float x;
	x = static_cast<float>( width ) / static_cast<float>( height );

	osg::Vec3Array* coords = static_cast<osg::Vec3Array*>( mGeom->getVertexArray() );
	( *coords )[0].set( -x, 1.5f, -1.0f );
	( *coords )[1].set( x, 1.5f, -1.0f );
	( *coords )[2].set( x, 1.5f,  1.0f );
	( *coords )[3].set( -x, 1.5f,  1.0f );

	mGeom->dirtyDisplayList();  // update changes

}

} // namespace OpenCV
