#ifndef FACERECOGNITIONWINDOW_H
#define FACERECOGNITIONWINDOW_H

#include <QDialog>
#include <opencv2/core/core.hpp>

class QLabel;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QStackedLayout;

namespace ArucoModul{
	class ArucoThread;
}
namespace OpenCV {
class FaceRecognizer;
}

namespace QOpenCV
{

class FaceRecognitionThread;

/**
	 * @author Autor: Marek Jakab
	 * @brief Class FaceRecognitionWindow
	 */
class FaceRecognitionWindow : public QDialog
	{
	Q_OBJECT

public:
	/**
		 * @author Autor: Marek Jakab
		 * @brief FaceRecognitionWindow Constructor
		 * @param parent Parent Widget
		 * @param app QApplication
		 * @param thr Instance of thread
		 */
	FaceRecognitionWindow(QWidget *parent, QApplication *mApp, QOpenCV::FaceRecognitionThread *thrFaceRec, ArucoModul::ArucoThread *thrAruco);

	QLabel		*getLabel() const;
	QCheckBox	*getMarkerBehindCB() const;
	QCheckBox	*getCorEnabledCB() const;
	QPushButton	*getUpdateCorParPB() const;

signals:
	void sendImgFaceRec( bool send );
	void sendImgMarker( bool send );
	void stopMarker( bool set );
	void stopFaceRec( bool set );
	void startMarker();
	void startFaceRec();

public slots:
	void onCorParUpdated();
	void onFaceRecThrFinished();
	void onMarkerThrFinished();

private slots:
	/**
			 * @author Autor: Marek Jakab
			 * @brief quitWindow Cancel thread and call destructor
			 */
	void quitWindow();

	/**
			 * @author Autor: Marek Jakab
			 * @brief setLabel Draw image on label
			 */
	void setLabel(cv::Mat);


	void onSelModulChange();
	void onUpdateCorPar();
	void onFaceRecStartCancel(bool checked);
	void onMarkerStartCancel(bool checked);




private:
	void closeEvent(QCloseEvent *event);

	/**
		 * @author Autor: Marek Jakab
		 * @brief configureWindow Sets window properties
		 */
	void configureWindow();



	QApplication	*mApp;
	QLabel			*mWindowLabel;
	QImage			mqimage;

	// switching the camera,
	QRadioButton	*mFaceRecRB;
	QRadioButton	*mMarkerRB;
	QCheckBox		*mNoVideo;
	QStackedLayout	*mModulesStackL;
	QPushButton		*mFaceRecStartCancelPB;
	QPushButton		*mMarkerStartCancelPB;

	// aruco controll
	QCheckBox		*mMarkerBehindCB;
	QCheckBox		*mCorEnabledCB;
	QPushButton		*mUpdateCorParPB;


	FaceRecognitionThread *mThrFaceRec;
	ArucoModul::ArucoThread *mThrAruco;

	};
}

#endif //FACERECOGNITIONWINDOW_H
