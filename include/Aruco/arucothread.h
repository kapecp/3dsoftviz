#ifndef ARUCOTHREAD_H
#define ARUCOTHREAD_H

#include <QThread>

#include <osg/Quat>
#include <osg/Vec3d>
#include <osg/Matrixd>
#include <QMatrix4x4>
#include <QImage>

#include <opencv2/core/core.hpp>

Q_DECLARE_METATYPE( osg::Vec3d )
Q_DECLARE_METATYPE( osg::Quat )


namespace ArucoModul {
class ArucoCore;
}

namespace OpenCV{
class CapVideo;
}

namespace ArucoModul {

/**
* @brief Class ArucoThread
* @author Dávid Durčák
* @date 18.02.2014
*/
class ArucoThread : public QThread
	{
	Q_OBJECT

public:
	ArucoThread( QObject *parent = 0);
	~ArucoThread(void);

	/**
	* @brief run Starts thread
	*@author Dávid Durčák
	*/
	void run();

signals:
	/**
	* @author Dávid Durčák
	* @brief pushArucoMVMat signal that emit ModelViewMatrix that represent position and rotation of detected marker
	* @param mat ModelView matrix from aruco
	*/
	void pushArucoMVMat( QMatrix4x4 mat);

	/**
	* @author Dávid Durčák
	* @brief sendArucoPosVec signal that emit vector that represent position of detected marker
	* @param pos Position vector
	*/
	void sendArucoPosVec( osg::Vec3d pos);

	/**
	* @author Dávid Durčák
	* @brief sendArucoRorQuat signal that emit Quaternion that represent rotation of detected marker
	* @param quat Quaternion
	*/
	void sendArucoRorQuat( const osg::Quat quat);

	/**
	* @author Dávid Durčák
	* @brief pushImage signal that emit image with marked marker for debuging
	* @param image emited image matrix
	*/
	//void pushImage( cv::Mat image );
	void pushImage( QImage qimage ) const;

	void pushBackgrImage( cv::Mat image ) const;

	/**
	* @author Dávid Durčák
	* @brief corParUpdated Signal when correction parameters were update succefull
	*/
	void corParUpdated();

	void moveMouseArucoSignal(double positionX,double positionY,bool isClick, Qt::MouseButton button );


public slots:
	/**
	* @author Dávid Durčák
	* @brief setCancel sets cancel=true
	* @param set Cancel thread if set == true
	*/
	void setCancel(bool set);

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
	void setCapVideo( OpenCV::CapVideo *capVideo);

	/**
	* @author Dávid Durčák
	* @brief interchangeMarkers Interchange markers for Graph and Mouse controll
	*/
	void interchangeMarkers();

private:

	/**
	* @author Dávid Durčák
	* @brief graphControlling method that process info about marker to controlling graph
	* @param actPosArray[3] const array that describe position of marker by vector (x,y,z)
	* @param actQuatArray[4] const array that describe orientation of marker by quaternion (w,x,y,z)
	*/
	void graphControlling(const double actPosArray[3], const double actQuatArray[4]);

	/**
	* @author Dávid Durčák
	* @brief mouseControlling method that process info about marker to controlling mouse
	* @param actPosArray[3] const array that describe position of marker by vector (x,y,z)
	* @param actQuatArray[4] const array that describe orientation of marker by quaternion (w,x,y,z)
	*/
	void mouseControlling(const double actPosArray[3], const double actQuatArray[4]);

	/**
	* @author Dávid Durčák
	* @brief imagesSending Pripare images for sending and emiting them
	*/
	void imagesSending(ArucoCore &aCore) const;

	/**
	* @author Dávid Durčák
	* @brief computeCorQuatAndPos Compute correction corP translation vector and corQ rotation quaternion, that are used later in correctQuatAndPost method.
	* @param position[3] array that describe position of marker by vector (x,y,z)
	* @param quaternion[4] array that describe orientation of marker by quaternion (w,x,y,z)
	*/
	void computeCorQuatAndPos( const double position[3], const double rotation[4] );

	/**
	* @author Dávid Durčák
	* @brief correctQuatAndPos Compute corrected actual position vector and rotation quaternion from correction mCorP vector and mCorQ quaternion.
	* It is need to be done if cammera is not in horizontal direction. Result corrected parameters describe marker like camera was in horizontal direction.
	* @param actPos input vector that  array that describe actual position of marker by vector (x,y,z)
	* @param actQuat array that describe actual orientation of marker by quaternion (w,x,y,z)
	*/
	void correctQuatAndPos( osg::Vec3d &actPos, osg::Quat &actQuat ) const;

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
	void printMat( const osg::Matrixd mat, const QString name = "Matrix: ") const;

	/**
	* @author Dávid Durčák
	* @brief printVec Debug method for printing vector
	* @param v Printed vector
	* @param name Optional name of printed vector
	*/
	void printVec( const osg::Vec3d v, const QString name = "Vector:  ") const;
	void printVec( const osg::Vec4d v, const QString name = "Vector:  ") const;


	bool		mCancel;			// thread is running if false
	bool		mCorSetted;			// true if mCorQ and mCorP was setted by computeCorQuatAndPos()
	bool		mMarkerIsBehind;	// if true, marker is behing projection, else marker is in front of in our side
	bool		mCorEnabled;		// if true, correction is enabled
	bool		mUpdCorPar;			// if true, correction can be setted
	bool		mSendImgEnabled;	// if true, of emiting actual frame is enabled
	bool		mSendBackgrImgEnabled;
	unsigned int	mGrM;				// number of marker for graph controll
	unsigned int	mMoM;				// number of marker for mouse controll


	QMatrix4x4	mCorM;				// correction matrix
	osg::Quat	mCorQ;				// correction quaternion
	osg::Vec3d	mCorP;				// correction vector
	double		mRatioCamCoef;		// correct aruco ration not centering y-axis
	double		mCamDistRatio;
	double		mHalfRatioCoef;

	OpenCV::CapVideo	*mCapVideo; // Cap video instanc, that managed camera and provides frames



	};
} // ArucoModul namespace
#endif //ARUCOTHREAD_H
