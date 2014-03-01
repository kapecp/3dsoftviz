#ifndef ARUCOTHREAD_H
#define ARUCOTHREAD_H

#include <QThread>
#include <opencv2/core/core.hpp>


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
	ArucoThread(void);
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
	void sendArucoPosVec( QVector3D pos);

	/**
	* @author Dávid Durčák
	* @brief sendArucoRorQuat signal that emit Quaternion that represent rotation of detected marker
	* @param quat Quaternion
	*/
	void sendArucoRorQuat( QQuaternion quat);

	/**
	* @author Dávid Durčák
	* @brief pushImage signal that emit image with marked marker for debuging
	* @param image emited image matrix
	*/
	void pushImage(cv::Mat image);
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

private:

	/**
	* @author Dávid Durčák
	* @brief computeCorQuatAndPos Compute correction corP translation vector and corQ rotation quaternion, that are used later in correctQuatAndPost method.
	* @param position[3] array that describe position of marker by vector (x,y,z)
	* @param quaternion[4] array that describe orientation of marker by quaternion (w,x,y,z)
	*/
	void computeCorQuatAndPos( const double position[3], const double rotation[] );

	/**
	* @author Dávid Durčák
	* @brief correctQuatAndPos Compute corrected actual position vector and rotation quaternion from correction mCorP vector and mCorQ quaternion.
	* It is need to be done if cammera is not in horizontal direction. Result corrected parameters describe marker like camera was in horizontal direction.
	* @param actPos input vector that  array that describe actual position of marker by vector (x,y,z)
	* @param actQuat array that describe actual orientation of marker by quaternion (w,x,y,z)
	*/
	void correctQuatAndPos( QVector3D &actPos, QQuaternion &actQuat ) const;

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
	void printMat( const QMatrix4x4 mat, const QString name = "Matrix: ") const;

	/**
	* @author Dávid Durčák
	* @brief printVec Debug method for printing vector
	* @param v Printed vector
	* @param name Optional name of printed vector
	*/
	void printVec( const QVector3D v, const QString name = "Vector:  ") const;
	void printVec( const QVector4D v, const QString name = "Vector:  ") const;


	bool		mCancel;
	bool		mCorSetted;	// true if mCorQ and mCorP was setted by computeCorQuatAndPos()
	QMatrix4x4	mCorM;		// correction matrix
	QQuaternion mCorQ;		// correction quaternion
	QVector3D   mCorP;		// correction vector


	};
} // ArucoModul namespace
#endif //ARUCOTHREAD_H
