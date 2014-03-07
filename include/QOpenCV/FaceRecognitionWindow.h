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
	/**
		 * @author Autor: Marek Jakab
		 * @brief cancelLoop Signal to cancel thread
		 */
	void cancelLoop( bool set);
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
			 * @brief pauseWindow Cancel run
			 */
	void pauseWindow();

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

	/**
			* @author Autor: Martina Tregerova
			* @brief switchImage Switches the image shown between marker detection and face recognition
			*/
	void switchImage();

	/**
			* @author Autor:Martina Tregerova
			* @brief startThread Starts threads based on the checked box
			*/

	void startThread();

	/**
			* @author Autor:Martina Tregerova
			* @brief startThread Stops threads based on the checked box
			*/

	void stopThread();

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


	void createGui();


	QApplication	*mApp;
	QLabel			*mWindowLabel;
	// -new
	QRadioButton	*mFaceRecRB;
	QRadioButton	*mMarkerRB;
	QCheckBox		*mNoVideo;
	QStackedLayout	*mModulesStackL;

	QPushButton		*mFaceRecStartCancelPB;
	QPushButton		*mMarkerStartCancelPB;


	QCheckBox		*mMarkerBehindCB;
	QCheckBox		*mCorEnabledCB;
	QPushButton		*mUpdateCorParPB;


	//radio buttons for switching the camera
	QRadioButton	*mFaceRadioButton;			//--
	QRadioButton	*mMarkerRadioButton;		//--

	QPushButton		*mPauseButton;				//--
	QPushButton		*mCancelButton;				//--

	//check buttons for switching the threads on and off
	QCheckBox		*mFaceCheckbox;				//--
	QCheckBox		*mMarkerCheckbox;			//--

	QImage			mqimage;
	FaceRecognitionThread *mThrFaceRec;
	ArucoModul::ArucoThread *mThrAruco;



	};
}

#endif //FACERECOGNITIONWINDOW_H
