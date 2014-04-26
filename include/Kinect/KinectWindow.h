#ifndef KINECTWINDOW_H
#define KINECTWINDOW_H

#include <QDialog>
#include <opencv2/core/core.hpp>

class QLabel;
class QPushButton;
class QRadioButton;
class QStackedLayout;
class QCheckBox;
class QSlider;

namespace Kinect
{

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
	 * @author Matej Marconak
	 *@brief KinectWindow
	 * @param mApp
	 */
	KinectWindow(QWidget *parent, QApplication *app,Kinect::KinectThread *thr);

	QLabel *getLabel() const;

signals:

	/**
	 * @brief Signal for enable Normal Video
	 * @param true for color, false for depth
	 */
	void sendNormalVideo(bool send);

	/**
	 * @brief startKinect
	 */
	void startKinect();

	/**
	 * @brief stopKinect
	 * @param true for stop, false to continue
	 */
	void stopKinect(bool set);

	/**
	 * @brief enable to view kinect video
	 * @param send true for enable, false for disable
	 */
	void sendImageKinect(bool send);

	/**
	 * @brief enable/disable for cursor move
	 * @param send true for stop, false for start
	 */
	void setMovementCursor(bool send);

	/**
	 * @brief change of speed movement for hand
	 * @param send speed for movement
	 */
	void sendSpeedKinect(double send);

public slots:

	void setLabel(cv::Mat image);
	void setLabelQ(QImage qimage );

	void pausewindows();

	void quitWindows();


private slots:

	void stopMovingCursor();

	void setSpeedKinect(int speed);

private:

	void closeEvent(QCloseEvent *event);

	/**
	 * @brief create Kinect Window
	 */
	void configureWindow(void);

	QApplication	*mApp;
	QLabel			*mWindowLabel;
	QRadioButton	*mKinectColor;
	QRadioButton	*mKinectDepthMap;
	QStackedLayout	*mModulesStackL;
	QCheckBox		*mDisableCursor;
	QPushButton		*mKinectPause;
	QSlider			*mSpeed;

	Kinect::KinectThread *thr;

};
}
#endif // KINECTWINDOW_H
