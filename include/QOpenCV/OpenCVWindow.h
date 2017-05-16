#ifndef OPENCVWINDOW_H
#define OPENCVWINDOW_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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

namespace OpenCV {
class CapVideo;
}

namespace QOpenCV {

class OpenCVWindow : public QDialog
{
	Q_OBJECT

public:
	/**
	 * @author Autor: Michael Garaj
	 * @brief OpenCVWindow Constructor
	 * @param parent Parent Widget
	 * @param mApp QApplication
	 */
	OpenCVWindow( QWidget* parent, QApplication* mApp );

	/**
	     * @author Autor: Michael Garaj
	     * @brief getLabel Return mWindowLabel
	     * @return QLabel*
	     */
	QLabel*		getLabel() const;

	/**
	     * @author Autor: David Durcak
	     * @brief getMarkerBehindCB Return mMarkerBehindCB
	     * @return QCheckBox*
	     */
	QCheckBox*	getMarkerBehindCB() const;

	/**
	     * @author Autor: David Durcak
	     * @brief getCorEnabledCB Return mCorEnabledCB
	     * @return QCheckBox*
	     */
	QCheckBox*	getCorEnabledCB() const;

	/**
	     * @author Autor: Juraj Marak
	     * @brief getMultiMarkerEnableCB Return mMultiMarkerEnableCB
	     * @return QCheckBox*
	     */
	QCheckBox*	getMultiMarkerEnableCB() const;

	/**
	     * @author Autor: David Durcak
	     * @brief getUpdateCorParPB Return mUpdateCorParPB
	     * @return QPushButton*
	     */
	QPushButton*	getUpdateCorParPB() const;

	/**
	     * @author Autor: David Durcak
	     * @brief getInterchangeMarkersCB Return mInterchangeMarkersCB
	     * @return QCheckBox*
	     */
	QPushButton*	getInterchangeMarkersPB() const;

signals:
	/**
	     * @author Autor: David Durcak
	     * @brief sendImgFaceRec Signal for enagling or disabling sending actual image from FaceDetection thread
	     * @param send
	     */
	void sendImgFaceRec( bool send );

	/**
	     * @author Autor: David Durcak
	     * @brief sendImgMarker Signal for enagling or disabling sending actual image from Aruco thread to this window
	     * @param send
	     */
	void sendImgMarker( bool send );

	/**
	     * @author Autor: David Durcak
	     * @brief sendBackgrImgFaceRec Signal for enagling or disabling sending actual image from FaceDetection thread to background
	     * @param send
	     */
	void sendBackgrImgFaceRec( bool send );

	/**
	     * @author Autor: David Durcak
	     * @brief sendBackgrImgMarker Signal for enagling or disabling sending actual image from Aruco thread thread to background
	     * @param send
	     */
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
	     * @author Autor: Michael Garaj
	     * @brief startMultiMarker Signal for starting Aruco thread
	     */
	void startMultiMarker();

	/**
	     * @author Autor: Michael Garaj
	     * @brief stopMultiMarker Signal for canceling Aruco thread
	     * @param set cancel if true
	     */
	void stopMultiMarker( bool set );

	/**
	     * @author Autor: David Durcak
	     * @brief startFaceRec Signal for starting FaceDetection thread
	     */
	void startFaceRec();

	/**
	     * @author Autor: Michael Garaj
	     * @brief setMultiMarker Signal for setting MultiMarker in Aruco thread
	     * @param set cancel if true
	     */
	void setMultiMarker( bool set );

	/**
	     * @author Autor: David Durcak
	     * @brief setCapVideoFaceRec Signal for setting camera in FaceRecognizationThread
	     * @param capVideo camera object
	     */
	void setCapVideoFaceRec( OpenCV::CapVideo* capVideo );

	/**
	     * @author Autor: Michael Garaj
	     * @brief setCapVideoFaceRec Signal for setting camera in ArucoThread
	     * @param capVideo camera object
	     */
	void setCapVideoMarker( OpenCV::CapVideo* capVideo );

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
	 * @brief enable/disable marker detection from kinect camera
	 * @param send true for start, false for stop
	 */
	void setKinectMarkerDetection( bool send );

	/**
	 * @brief switch markerless detection on kinect
	 * @param set (on/off)
	 */
	void setKinectMarkerlessDetection( bool set );

	/**
	 * @brief switch markerless detection on camera
	 * @param set (on/off)
	 */
	void setCameraMarkerlessDetection( bool set );

	/**
	 * @brief change of speed movement for hand
	 * @param send speed for movement
	 */
	void sendSpeedKinect( double send );

	/**
	 * @author Autor: Michael Garaj
	 * @brief inicializeKinect Signal to inicialize the kinect
	 */
	void inicializeKinect();

	/**
	 * @author Autor: Michael Garaj
	 * @brief closeActionOpenni Signal to close Openni stream
	 */
	void closeActionOpenni();

	void setKinectCaptureImage( bool set );

	void arucoRunning( bool isRunning );

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
	    * @author Autor: Michael Garaj
	    * @brief setLabel Draw image on label
	    */
	void setLabel( cv::Mat image );

private slots:
	/**
	     * @author Autor: Michael Garaj
	     * @brief onSelModulChange Switch between Aruco and Kinect layout
	     */
	void onSelModulChange();

	/**
	     * @author Autor: Michael Garaj
	     * @brief onSelSubModulChange Switch between Aruco Sub modules
	     */
	void onSelSubModulChange();

	/**
	     * @author Autor: Michael Garaj
	     * @brief onMultiMarkerStartCancel Start or stop Aruco thread
	     * @param checked If true, thread will start
	     */
	void onMultiMarkerStartCancel( bool checked );

	/**
	     * @author Autor: Michael Garaj
	     * @brief onKinectStartCancel Start or stop Kinect thread
	     * @param checked If true, thread will start
	     */
	void onKinectStartCancel( bool checked );

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
	void onFaceRecStartCancel( bool checked );

	/**
	     * @author Autor: David Durcak
	     * @brief onMarkerStartCancel Start or stop Aruco thread
	     * @param checked If true, thread will start
	     */
	void onMarkerStartCancel( bool checked );

	/**
	     * @author Autor: David Durcak
	     * @brief onFaceDetBackgrCBClicked Slot for enabling video backgroung from FaceRecongition and disabling from Aruco
	     * @param checked
	     */
	void onFaceDetBackgrCBClicked( bool checked );

	void onKinectSnapshotPBClicked();

	/**
	     * @author Autor: David Durcak
	     * @brief onMarkerBackgrCBClicked Slot for enabling video backgroung from Aruco and disabling from FaceRecongition
	     * @param checked
	     */
	void onMarkerBackgrCBClicked( bool checked );

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

	/**
	 * @brief private SLOT set marker detection
	 * @param speed of movement
	 */
	void setMarkerDetection( bool set );

	/**
	 * @brief private SLOT for switching markerless tracking on camera
	 * @param set (on/off)
	 */
	void setMarkerlessDetectionCamera( bool set );

	/**
	 * @brief private SLOT for switching markerless tracking on kinect
	 * @param set (on/off)
	 */
	void setMarkerlessDetectionKinect( bool set );

private:
	/**
	     * @author Autor: Michael Garaj
	     * @brief closeEvent Reimplement close event. Disconnect all conections
	     * @param event Close event
	     */
	void closeEvent( QCloseEvent* event );

	/**
	     * @author Autor: Michael Garaj
	     * @brief configureWindow Create dialog's gui,( all layouts, widgets and connection inside class)
	     */
	void configureWindow();

	QApplication*	mApp;
	QLabel*			mWindowLabel;

	QRadioButton*	mKinectRB;
	QRadioButton*	mArucoRB;
	QRadioButton*    mFaceRecRB;
	QRadioButton*    mMarkerRB;
	QRadioButton*    mMultiMarkerRB;

	QPushButton*     mFaceRecPB;
	QPushButton*     mMarkerPB;
	QPushButton*     mMultiMarkerPB;
	QPushButton*     mMultiMarkerStartCancelPB;
	QPushButton*		mKinectPB;
	QPushButton*		mUpdateCorParPB;
	QPushButton*		mInterchangeMarkersPB;
	QPushButton*     mKinectSnapshotPB;

	QStackedLayout*  mModulesStackL;
	QStackedLayout*  mSubmodulesStackL;

	QCheckBox*		mNoVideo;
	QCheckBox*		 mMarkerBackgrCB;
	QCheckBox*		 mFaceDetBackgrCB;
	QCheckBox*		 mMarkerBehindCB;
	QCheckBox*		 mCorEnabledCB;
	QCheckBox*	     mMultiMarkerEnableCB;
	QCheckBox*		 mEnableMarkerlessKinectCB;

	QCheckBox*       mDisableCursorCB;
	QCheckBox*       mDisableZoomCursorCB;
	QCheckBox*       mEnableMarkerDetectCB;
	QCheckBox*		 mEnableMarkerlessCameraCB;

	QSlider*         mSpeed;
};

}

#endif //OPENCVWINDOW_H
