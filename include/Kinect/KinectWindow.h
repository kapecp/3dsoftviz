#ifndef KINECTWINDOW_H
#define KINECTWINDOW_H

#include <QDialog>
#include <opencv2/core/core.hpp>


class QLabel;
class QPushButton;
class QRadioButton;

namespace Kinect
{
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
	KinectWindow(QWidget *parent, QApplication *app);

	QLabel *getLabel() const;

signals:

	/**
	 * @brief Signal for enable Normal Video
	 * @param send
	 */
	void sendNormalVideo(bool send);

	/**
	 * @brief startKinect
	 */
	void startKinect();

	/**
	 * @brief stopKinect
	 */
	void stopKinect();



public slots:

	void setLabel(cv::Mat image);

	//void setLabelQ(QImage qimage );

private slots:

	void onKinectStartCancel(bool checked);

private:

	void closeEvent(QCloseEvent *event);

	void configureWindow(void);

	QApplication	*mApp;
	QLabel			*mWindowLabel;
	QRadioButton	*mKinectColor;
	QRadioButton	*mKinectDepthMap;

	QPushButton		*mKinectStartStop;



};

}
#endif // KINECTWINDOW_H
