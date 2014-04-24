#ifndef FACERECOGNITIONWINDOW_H
#define FACERECOGNITIONWINDOW_H

#include <QDialog>
#include <opencv2/core/core.hpp>


class QLabel;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QStackedLayout;

namespace OpenCV {
class CapVideo;
}

namespace QOpenCV
{

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
	FaceRecognitionWindow(QWidget *parent, QApplication *mApp);

	/**
		 * @author Autor: David Durcak
		 * @brief getLabel Return mWindowLabel
		 * @return QLabel*
		 */
	QLabel		*getLabel() const;

	/**
		 * @author Autor: David Durcak
		 * @brief getMarkerBehindCB Return mMarkerBehindCB
		 * @return QCheckBox*
		 */
	QCheckBox	*getMarkerBehindCB() const;

	/**
		 * @author Autor: David Durcak
		 * @brief getCorEnabledCB Return mCorEnabledCB
		 * @return QCheckBox*
		 */
	QCheckBox	*getCorEnabledCB() const;

	/**
		 * @author Autor: David Durcak
		 * @brief getUpdateCorParPB Return mUpdateCorParPB
		 * @return QPushButton*
		 */
	QPushButton	*getUpdateCorParPB() const;

	/**
		 * @author Autor: David Durcak
		 * @brief getInterchangeMarkersCB Return mInterchangeMarkersCB
		 * @return QCheckBox*
		 */
	QPushButton	*getInterchangeMarkersPB() const;


signals:
	/**
		 * @author Autor: David Durcak
		 * @brief sendImgFaceRec Signal for enagling or disabling sending actual image from FaceDetection thread
		 * @param send
		 */
	void sendImgFaceRec( bool send );

	/**
		 * @author Autor: David Durcak
		 * @brief sendImgMarker Signal for enagling or disabling sending actual image from Aruco thread
		 * @param send
		 */
	void sendImgMarker( bool send );

	void sendBackgrImgFaceRec( bool send );
	void sendBackgrImgMarker( bool send );

	/**
		 * @author Autor: David Durcak
		 * @brief stopMarker Signal for canceling Aruco thread
		 * @param set cancel if true
		 */
	void stopMarker( bool set );


	/**
		 * @author Autor: David Durcak
		 * @brief stopFaceRec Signal for canceling FaceDetection thread
		 * @param set cancel if true
		 */
	void stopFaceRec( bool set );

	/**
		 * @author Autor: David Durcak
		 * @brief startMarker Signal for starting Aruco thread
		 */
	void startMarker();

	/**
		 * @author Autor: David Durcak
		 * @brief startFaceRec Signal for starting FaceDetection thread
		 */
	void startFaceRec();

	void setCapVideoFaceRec( OpenCV::CapVideo *capVideo);

	void setCapVideoMarker( OpenCV::CapVideo *capVideo);


public slots:
	/**
		 * @author Autor: David Durcak
		 * @brief onCorParUpdated When corection parameters were updated
		 */
	void onCorParUpdated();

	/**
		 * @author Autor: David Durcak
		 * @brief onFaceRecThrFinished When Face detection thread finished, enable mFaceRecStartCancelPB button
		 */
	void onFaceRecThrFinished();

	/**
		 * @author Autor: David Durcak
		 * @brief onMarkerThrFinished When Aruco thread finished, update mMarkerStartCancelPB button
		 */
	void onMarkerThrFinished();

	/**
		* @author Autor: Marek Jakab
		* @brief setLabel Draw image on label
		*/
	void setLabel( cv::Mat image );
	void setLabelQ(QImage qimage );

private slots:

	/**
		 * @author Autor: David Durcak
		 * @brief onSelModulChange Switch between Aruco, Face Detection gui or any image sending
		 */
	void onSelModulChange();

	/**
		 * @author Autor: David Durcak
		 * @brief onUpdateCorPar  Disabling mUpdateCorParPB, until corrections parameter are not updated
		 */
	void onUpdateCorPar();

	/**
		 * @author Autor: David Durcak
		 * @brief onFaceRecStartCancel Start or stop Face Detection thread
		 * @param checked If true, thread will start
		 */
	void onFaceRecStartCancel(bool checked);

	/**
		 * @author Autor: David Durcak
		 * @brief onMarkerStartCancel Start or stop Aruco thread
		 * @param checked If true, thread will start
		 */
	void onMarkerStartCancel(bool checked);

	void onFaceDetBackgrCBClicked(bool checked);
	void onMarkerBackgrCBClicked(bool checked);


private:
	/**
		 * @author Autor: David Durcak
		 * @brief closeEvent Reimplement close event. Disconnect all conections
		 * @param event Close event
		 */
	void closeEvent(QCloseEvent *event);

	/**
		 * @author Autor: David Durcak
		 * @brief configureWindow Create dialog's gui,( all layouts, widgets and connection inside class)
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

	QCheckBox		*mMarkerBackgrCB;
	QCheckBox		*mFaceDetBackgrCB;

	// aruco controll
	QCheckBox		*mMarkerBehindCB;
	QCheckBox		*mCorEnabledCB;
	QPushButton		*mUpdateCorParPB;
	QPushButton		*mInterchangeMarkersPB;

	};
}

#endif //FACERECOGNITIONWINDOW_H
