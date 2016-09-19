#ifndef KINECTWINDOW_H
#define KINECTWINDOW_H

#include <QCheckBox>
#include <QCloseEvent>
#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QStackedLayout>
#include <QVBoxLayout>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class QLabel;
class QPushButton;
class QRadioButton;
class QStackedLayout;
class QCheckBox;
class QSlider;

namespace Kinect {

class KinectThread;
/**
 * @author Matej Marconak
 *@brief The KinectWindow class
 */
class KinectWindow : public QDialog
{
	Q_OBJECT

public:

	/**
	 *	@author Matej Marconak
	 *	@brief KinectWindow
	 *	@param mApp
	 */
	KinectWindow( QWidget* parent, QApplication* app,Kinect::KinectThread* thr );

	/**
	 * @brief getLabel
	 * @return actual QImage in frame
	 */
	QLabel* getLabel() const;

signals:

	/**
	 * @brief Signal for enable Normal Video
	 * @param true for color, false for depth
	 */
	void sendNormalVideo( bool send );

	/**
	 * @brief startKinect
	 */
	void startKinect();

	/**
	 * @brief stopKinect
	 * @param true for stop, false to continue
	 */
	void stopKinect( bool set );

	/**
	 * @brief enable to view kinect video
	 * @param send true for enable, false for disable
	 */
	void sendImageKinect( bool send );

	/**
	 * @brief enable/disable for cursor move
	 * @param send true for stop, false for start
	 */
	void setMovementCursor( bool send );

	/**
	 * @brief enable/disable graph zoom
	 * @param send true for stop, false for start
	 */
	void setZoom( bool send );

	/**
	 * @brief change of speed movement for hand
	 * @param send speed for movement
	 */
	void sendSpeedKinect( double send );

public slots:

	/**
	 * @brief SLOT for setting picture in Frame
	 * @param image to set fo frame
	 */
	void setLabel( cv::Mat image );

	/**
	 * @brief SLOT for setting picture in Frame
	 * @param qimage to set frame
	 */
	void setLabelQ( QImage qimage );

	/**
	 * @brief SLOT for pause video
	 */
	void pausewindows();

	/**
	 * @brief SLOT for stop Window
	 */
	void quitWindows();


private slots:

	/**
	 * @brief private SLOT for turn OFF cursor
	 */
	void stopMovingCursor();
	void stopZoom();

	/**
	 * @brief private SLOT set speed movements
	 * @param speed of movement
	 */
	void setSpeedKinect( int speed );

private:

	/**
	 * @brief closeEvent
	 * @param event
	 */
	void closeEvent( QCloseEvent* event );

	/**
	 * @brief create Kinect Window
	 */
	void configureWindow( void );

	QApplication*	mApp;
	QLabel*			mWindowLabel;
	QRadioButton*	mKinectColor;
	QRadioButton*	mKinectDepthMap;
	QStackedLayout*	mModulesStackL;
	QCheckBox*		mDisableCursor;
	QCheckBox*		mDisableZoomCursor;
	QPushButton*		mKinectPause;
	QSlider*			mSpeed;

	/**
	 * @brief Kinect thread
	 */
	Kinect::KinectThread* thr;

};
}
#endif // KINECTWINDOW_H
