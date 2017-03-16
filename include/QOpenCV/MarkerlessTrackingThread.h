#ifndef MARKERLESSTRACKINGTHREAD_H
#define MARKERLESSTRACKINGTHREAD_H

#include <QThread>
#include "opencv2/core/core.hpp"
#include <opencv2/core/mat.hpp>

namespace OpenCV {
class CapVideo;
}

namespace QOpenCV {

/**
 * @brief The MarkerlessTrackingThread class
 * @author Autor: Lukas Hagara
 * @date 24.2.2017
 */
class MarkerlessTrackingThread : public QThread
{
	Q_OBJECT

public:

	MarkerlessTrackingThread();
	~MarkerlessTrackingThread( void );

	/**
	 * @author Autor: Lukas Hagara
	 * @brief run Starts thread
	 */
	void run();

signals:

	/**
	 * @author Autor: Lukas Hagara
	 * @brief pushImage Used for sending image to a window
	 * @param Image cv::Mat image to push
	 */
	void pushImage( cv::Mat Image );

public slots:

	/**
	 * @author Autor: Lukas Hagara
	 * @brief setCancel Set member mCancel
	 */
	void setCancel( bool );

	/**
	 * @author Autor: Lukas Hagara
	 * @brief setCapVideo Set member mCapVideo
	 * @param capVideo
	 */
	void setCapVideo( OpenCV::CapVideo* capVideo );

private:

	/**
	 * @brief mCapVideo represents the video source
	 */
	OpenCV::CapVideo*	mCapVideo;
	/**
	 * @brief mCancel the thread loop variable
	 */
	bool				mCancel;


};
}

#endif //MARKERLESSTRACKINGTHREAD_H
