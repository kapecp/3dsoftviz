#ifndef ARUCOTHREAD_H
#define ARUCOTHREAD_H

#include <QThread>

#include <osg/Quat>
#include <osg/Vec3d>
#include <osg/Matrixd>
#include <QMatrix4x4>
#include <QImage>

#include <opencv2/core/core.hpp>

#include "Util/SizedQueue.h"

namespace ArucoModul {
class ArucoCore;
class ArControlClass;
}

namespace OpenCV {
class CapVideo;
}

namespace ArucoModul {

/**
	*@brief Class ArucoThread
	*@author Autor: David durcak
	*@date 18.02.2014
	*@note This class provides a thread for tracking Aruco.
	*/
class ArucoThread : public QThread
{
	Q_OBJECT

public:
	ArucoThread( QObject* parent = 0 );
	~ArucoThread( void );

	/**
	*@author Dávid Durčák
	* @brief run It is a thread method.  In cyclus are performed: get frame from camera, add frame to aruco for detecting,
	* get result position and rotation from Aruco, correct it, if correction is enabled, send it to Camera manipulator ang CoreGraph
	* for rotating and translating graph, send it for mouse controll, and send frame to FaceDetectionWindow to user can see it
	*/
	void run();

signals:
	/**
	* @author Dávid Durčák
	* @brief pushArucoMVMat signal that emit ModelViewMatrix that represent position and rotation of detected marker
	* @param mat ModelView matrix from aruco
	*/
	void pushArucoMVMat( QMatrix4x4 mat );

	/**
	* @author Dávid Durčák
	* @brief sendArucoPosVec signal that emit vector that represent position of detected marker
	* @param pos Position vector
	*/
	void sendArucoPosVec( osg::Vec3d pos );

	/**
	* @author Dávid Durčák
	* @brief sendArucoRorQuat signal that emit Quaternion that represent rotation of detected marker
	* @param quat Quaternion
	*/
	void sendArucoRorQuat( const osg::Quat quat );

	/**
	* @author Michael Garaj
	* @brief sendArucoPosAndQuat signal that emit Quaternion that represent rotation of detected marker and position Vector
	* @param quat Quaternion
	* @param pos Vec3d
	*/
	void sendArucoPosAndQuat( const osg::Quat quat, osg::Vec3d pos );

	/**
	* @author Dávid Durčák
	* @brief pushImage signal that emit image with marked marker for debuging
	* @param image emited image matrix
	*/
	void pushImagemMat( cv::Mat image ) const;

	/**
	* @author Michael Garaj
	* @brief pushImageFromKinect signal that emit image with marked marker for debuging from Kinect
	* @param image emited image matrix
	*/
	void pushImageFromKinect( cv::Mat image ) const;

	/**
	* @author Dávid Durčák
	* @brief pushBackgrImage signal that emit image for background
	* @param image emited image matrix
	*/
	void pushBackgrImage( cv::Mat image ) const;

	/**
	* @author Dávid Durčák
	* @brief corParUpdated Signal when correction parameters were update succefull
	*/
	void corParUpdated();

	/**
	* @author Dávid Durčák
	* @brief moveMouseArucoSignal Signal for sending parameters to control mouse
	*/
	void moveMouseArucoSignal( double positionX,double positionY,bool isClick, Qt::MouseButton button );

	//jurik
	/**
	* @author Igor Jurík
	* @brief sendModelViewMatrix sending modelview matrix of detected marker
	*/
	void sendModelViewMatrix( QMatrix4x4 modelviewmatrix );

	/**
	* @author Igor Jurík
	* @brief sendProjectionMatrix sending projection matrix of detected marker
	*/
	void sendProjectionMatrix( QMatrix4x4 projectionmatrix );

	void sendMarkerSize( float markerSize );
	//*****

public slots:
	/**
	* @author Dávid Durčák
	* @brief setCancel sets cancel=true
	* @param set Cancel thread if set == true
	*/
	void setCancel( bool set );

	/**
	* @author Dávid Durčák
	* @brief pauses recognition window
	*/
	void pause();

	/**
	* @author Dávid Durčák
	* @brief setPositionOfMarker Set mMarkerIsBehind variable. If true, marker is behing projection/monitor, else marker is in front of in our side
	* @param behind
	*/
	void setPositionOfMarker( bool behind );

	/**
	* @author Dávid Durčák
	* @brief setCorEnabling Set enabling of correction.
	* @param corEnabled
	*/
	void setCorEnabling( bool corEnabled );

	/**
	* @author Dávid Durčák
	* @brief setSendImgEnabling Set emiting of actual frame.
	* @param sendImgEnabled
	*/
	void setSendImgEnabling( bool sendImgEnabled );

	/**
	* @author Dávid Durčák
	* @brief setSendBackgrImgEnabled Set emiting of actual frame to backgroung.
	* @param sendImgEnabled
	*/
	void setSendBackgrImgEnabled( bool sendBackgrImgEnabled );

	/**
	* @author Dávid Durčák
	* @brief updateCorectionPar Set mUpdCorPar tu true what will cause calling computeCorQuatAndPos for updating correction parameters, and emit corParUpdated signal if succesed
	*/
	void updateCorectionPar();

	/**
	* @author Dávid Durčák
	* @brief setCapVideo Set member mCapVideo.
	* @param capVideo
	*/
	void setCapVideo( OpenCV::CapVideo* capVideo );

	/**
	* @author Dávid Durčák
	* @brief interchangeMarkers Interchange markers for Graph and Mouse controll
	*/
	void interchangeMarkers();

	/**
	* @author Michael Garaj
	* @brief setMultiMarker Set mMultiMarkerEnabled true to enable multi marker detection, false to disable
	* @param set
	*/
	void setMultiMarker( bool set );

	/**
	* @author Michael Garaj
	* @brief detectMarkerFromImage Detect markers from kinect image
	* @param set
	*/
	void detectMarkerFromImage( cv::Mat image );

private:

	/**
	* @author Dávid Durčák
	* @brief graphControlling method that process info about marker to controlling graph
	* @param actPosArray[3] const array that describe position of marker by vector (x,y,z)
	* @param actQuatArray[4] const array that describe orientation of marker by quaternion (w,x,y,z)
	*/
	//void graphControlling( const double actPosArray[3], const double actQuatArray[4] );
	void graphControlling( const osg::Vec3f actPosArray, const osg::Quat actQuatArray );

	/**
	* @author Dávid Durčák
	* @brief mouseControlling method that process info about marker to controlling mouse
	* @param actPosArray[3] const array that describe position of marker by vector (x,y,z)
	* @param actQuatArray[4] const array that describe orientation of marker by quaternion (w,x,y,z)
	*/
	//void mouseControlling( const double actPosArray[3], const double actQuatArray[4] );
	void mouseControlling( const osg::Vec3f actPosArray, const osg::Quat actQuatArray );

	/**
	* @author Dávid Durčák
	* @brief imagesSending Pripare images for sending and emiting them
	*/
	void imagesSending( ArucoCore& aCore, const cv::Mat frame ) const;

	/**
	* @author Dávid Durčák
	* @brief computeCorQuatAndPos Compute correction corP translation vector and corQ rotation quaternion, that are used later in correctQuatAndPost method.
	* @param position[3] array that describe position of marker by vector (x,y,z)
	* @param quaternion[4] array that describe orientation of marker by quaternion (w,x,y,z)
	*/
	//void computeCorQuatAndPos( const double position[3], const double rotation[4] );
	void computeCorQuatAndPos( const osg::Vec3f position, const osg::Quat rotation );


	/**
	* @author Dávid Durčák
	* @brief correctQuatAndPos Compute corrected actual position vector and rotation quaternion from correction mCorP vector and mCorQ quaternion.
	* It is need to be done if cammera is not in horizontal direction. Result corrected parameters describe marker like camera was in horizontal direction.
	* @param actPos input vector that  array that describe actual position of marker by vector (x,y,z)
	* @param actQuat array that describe actual orientation of marker by quaternion (w,x,y,z)
	*/
	void correctQuatAndPos( osg::Vec3d& actPos, osg::Quat& actQuat ) const;

	/**
	* @author Dávid Durčák
	* @brief computeCorMat Compute correction corM matrix.
	* Correction matrix == rotation around point described by origM.
	* Applycation: corected matrix = corM * mat.   Correction matrix must be on left in multiplication.
	* result corrected matrix describe marker like camera was in horizontal direction.
	* @param origM Matrix that describe point with translation and rotation
	*/
	void computeCorMat( QMatrix4x4 origM );

	/**
	* @author Dávid Durčák
	* @brief printMat Debug method for printing matrix
	* @param mat Printed matrix
	* @param name Optional name of printed matrix
	*/
	void printMat( const osg::Matrixd mat, const QString name = "Matrix: " ) const;

	/**
	* @author Dávid Durčák
	* @brief printVec Debug method for printing vector
	* @param v Printed vector
	* @param name Optional name of printed vector
	*/
	void printVec( const osg::Vec3d v, const QString name = "Vector:  " ) const;
	void printVec( const osg::Vec4d v, const QString name = "Vector:  " ) const;


	bool		mCancel;			// thread is running if false
	bool		mCorSetted;			// true if mCorQ and mCorP was setted by computeCorQuatAndPos()
	bool		mMarkerIsBehind;	// if true, marker is behing projection, else marker is in front of in our side
	bool		mCorEnabled;		// if true, correction is enabled
	bool		mUpdCorPar;			// if true, correction can be setted
	bool		mSendImgEnabled;	// if true, of emiting actual frame is enabled
	bool		mSendBackgrImgEnabled;
	bool		mMultiMarkerEnabled;

	unsigned int	mGrM;				// number of marker for graph controll
	unsigned int	mMoM;				// number of marker for mouse controll


	QMatrix4x4	mCorM;				// correction matrix
	osg::Quat	mCorQ;				// correction quaternion
	osg::Vec3d	mCorP;				// correction vector
	double		mRatioCamCoef;		// correct aruco ration not centering y-axis
	double		mCamDistRatio;
	double		mHalfRatioCoef;

	OpenCV::CapVideo*	mCapVideo; // Cap video instanc, that managed camera and provides frames
	Util::SizedQueue* boolQueue;

	//JMA
	ArControlClass* mArControlClass;
	osg::Vec3d normalizePos( const osg::Vec3f actPosArray, const osg::Quat actQuatArray );//ArucoThread::



};
} // ArucoModul namespace
#endif //ARUCOTHREAD_H
