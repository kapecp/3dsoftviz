#include "Viewer/CameraStream.h"
#include <QDebug>
#include <opencv2/highgui/highgui.hpp>
#include "Util/ApplicationConfig.h"


using namespace Vwr;

CameraStream::CameraStream(osg::Geometry *geom) : QObject(), osg::ImageStream()
{
	mGeom	= geom;
	mWidth	= 0;
	mHeight = 0;

	cv::Mat cvImg = cv::Mat::zeros(480,640, CV_8UC3);

	updateBackgroundImage( cvImg);
}

CameraStream::~CameraStream(){}

void CameraStream::updateBackgroundImage( cv::Mat cvImg)
{
	// update geometry coordinates if thare are different dimensions of image,
	// becasuse probebly changed it ratio of sides
	if(mGeom != NULL ){
		if( cvImg.cols != mWidth || mHeight != cvImg.rows){
			mWidth	= cvImg.cols;
			mHeight = cvImg.rows;
			updateGeometryCoords( mWidth, mHeight);
		}
	}


	IplImage* iplImg = cvCloneImage( &(IplImage)cvImg);

	setImage( iplImg->width, iplImg->height, 3,
			  GL_RGB, GL_BGR, GL_UNSIGNED_BYTE,
			  (BYTE*)( iplImg->imageData),
			  osg::Image::AllocationMode::NO_DELETE, 1);

	dirty();
}

void CameraStream::updateGeometryCoords(int width, int height)
{
	//qDebug() << width << " x " << height;
	float x;
	x = ((float)width) / ((float)height);

	//qDebug() << "x " << x;

	osg::Vec3Array* coords = (osg::Vec3Array*) mGeom->getVertexArray();
	(*coords)[0].set( -x, 1.5f, -1.0f );
	(*coords)[1].set(  x, 1.5f, -1.0f );
	(*coords)[2].set(  x, 1.5f,  1.0f );
	(*coords)[3].set( -x, 1.5f,  1.0f );

	mGeom->dirtyDisplayList();  // update changes

}
