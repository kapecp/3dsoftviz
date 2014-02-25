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
	void printMat( const QMatrix4x4 mat, const QString name = "Matrix: ");

	/**
	* @author Dávid Durčák
	* @brief printVec Debug method for printing vector
	* @param v Printed vector
	* @param name Optional name of printed vector
	*/
	void printVec( const QVector3D v, const QString name = "vector:  ");


	bool cancel;
	QMatrix4x4 corM;		// correction matrix

	};
} // ArucoModul namespace
#endif //ARUCOTHREAD_H
