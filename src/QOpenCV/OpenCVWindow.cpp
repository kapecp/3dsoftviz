#include <QtGui/QIntValidator>
#include "QOpenCV/OpenCVWindow.h"

#include "OpenCV/CamSelectCore.h"

#include "Util/ApplicationConfig.h"

QOpenCV::OpenCVWindow::OpenCVWindow( QWidget* parent, QApplication* app ) : QDialog( parent )
{
	mApp = app;
	configureWindow();
}

QLabel* QOpenCV::OpenCVWindow::getLabel() const
{
	return mWindowLabel;
}

QCheckBox*	QOpenCV::OpenCVWindow::getMarkerBehindCB() const
{
	return mMarkerBehindCB;
}

QCheckBox*	QOpenCV::OpenCVWindow::getMultiMarkerEnableCB() const
{
	return mMultiMarkerEnableCB;
}

QCheckBox*	QOpenCV::OpenCVWindow::getCorEnabledCB() const
{
	return mCorEnabledCB;
}

QPushButton*	QOpenCV::OpenCVWindow::getUpdateCorParPB() const
{
	return mUpdateCorParPB;
}

QPushButton* QOpenCV::OpenCVWindow::getInterchangeMarkersPB() const
{
	return mInterchangeMarkersPB;
}

void QOpenCV::OpenCVWindow::configureWindow()
{
	setModal( false );
	setWindowTitle( tr( "Kinect and Aruco Window" ) );

	mWindowLabel = new QLabel( "", this, 0 );

	mMultiMarkerRB = new QRadioButton( tr( "Multi Marker" ) );
	mLightDetRB = new QRadioButton( tr( "Light Detection" ) );

	mKinectPB = new QPushButton( tr( "Start Kinect" ) );
	mKinectSnapshotPB = new QPushButton( tr( "Kinect Snapshot" ) );
	mUpdateCorParPB	= new QPushButton( tr( "Update cor. param." ) );
	mInterchangeMarkersPB = new QPushButton( tr( "Change Markers" ) );
	mLightDetPB = new QPushButton( tr( "Start Light Detection" ) );

	mMarkerNoVideo = new QCheckBox( tr( "NoVideo" ) );
	mFaceNoVideo  = new QCheckBox( tr( "NoVideo" ) );

	mMarkerBackgrCB	= new QCheckBox( tr( "Background" ) );
	mFaceDetBackgrCB = new QCheckBox( tr( "Background" ) );
	mMarkerBehindCB	= new QCheckBox( tr( "Marker is behind" ) );
	mCorEnabledCB = new QCheckBox( tr( "Correction" ) );
	mMultiMarkerEnableCB = new QCheckBox( tr( "Multiple markers" ) );
	mEnableMarkerlessCameraCB = new QCheckBox( tr( "Turn on Markerless Detection" ) );

	mRefEnableFaceRecCB = new QCheckBox( tr( "Face Recognition" ) );
	mRefEnableMarkerlessCB = new QCheckBox( tr( "Tracking" ) );

	mDisableCursorCB = new QCheckBox( tr( "Turn off cursor" ) );
	mDisableZoomCursorCB = new QCheckBox( tr( "Turn off zoom" ) );
	mEnableMarkerDetectCB = new QCheckBox( tr( "Turn on Marker Detection" ) );
	mEnableMarkerlessKinectCB = new QCheckBox( tr( "Turn on Markerless Detection" ) );

	mLightDetectShowProcessingCB = new QCheckBox( tr( "Show frame processing" ) );

	mSpeed =  new QSlider( Qt::Horizontal );
	mSpeed->setRange( 5,20 );
	mSpeed->setValue( 10 );
	mSpeed->setPageStep( 1 );
	mSpeed->setFocusPolicy( Qt::NoFocus );
	mSpeed->setToolTip( tr( "Modify speed of movement" ) );

	mFisheyeXS = new QSlider( Qt::Horizontal );
	mFisheyeXS->setRange( 0,512 );
	mFisheyeXS->setValue( 256 );
	mFisheyeXS->setPageStep( 1 );
	mFisheyeXS->setFocusPolicy( Qt::NoFocus );
	mFisheyeXS->setToolTip( tr( "Adjust the center of fisheye lens for calculations" ) );

	mFisheyeYS = new QSlider( Qt::Horizontal );
	mFisheyeYS->setRange( 0,512 );
	mFisheyeYS->setValue( 256 );
	mFisheyeYS->setPageStep( 1 );
	mFisheyeYS->setFocusPolicy( Qt::NoFocus );
	mFisheyeYS->setToolTip( tr( "Adjust the center of fisheye lens for calculations" ) );

	mFisheyeRS = new QSlider( Qt::Horizontal );
	mFisheyeRS->setRange( 0,512 );
	mFisheyeRS->setValue( 256 );
	mFisheyeRS->setPageStep( 1 );
	mFisheyeRS->setFocusPolicy( Qt::NoFocus );
	mFisheyeRS->setToolTip( tr( "Adjust the radius of fisheye lens for calculations" ) );

	mSubmodulesStackL = new QVBoxLayout;

	QHBoxLayout* mainLayout		= new QHBoxLayout;
	QVBoxLayout* buttonLayout	= new QVBoxLayout;

//#ifdef OPENNI2_FOUND
//	mKinectRB->setChecked( true );
//	buttonLayout->addWidget( mKinectRB );
//#endif

	QWidget* kinectPageWid =  new QWidget;
	QWidget* arucoPageWid =  new QWidget;
	QWidget* arucoFaceRecPageWid = new QWidget;
	QWidget* arucoLightDetPageWid = new QWidget;
	QWidget* arucoMarkerPageWid = new QWidget;
	QWidget* arucoMultiMarkerPageWid = new QWidget;
	QWidget* arucoSubPageWid = new QWidget;

	kinectPageWid->setStyleSheet("background-color: lightblue");
	arucoFaceRecPageWid->setStyleSheet("background-color: lightblue");
	arucoMarkerPageWid->setStyleSheet("background-color: lightblue");

	QVBoxLayout*	kinectPageLayout = new QVBoxLayout;
	QVBoxLayout*	arucoPageLayout	= new QVBoxLayout;
	QVBoxLayout* arucoFaceRecPageLayout = new QVBoxLayout;
	QVBoxLayout* arucoLightDetPageLayout = new QVBoxLayout;
	QVBoxLayout* arucoMarkerPageLayout = new QVBoxLayout;
	QVBoxLayout* arucoMultiMarkerPageLayout = new QVBoxLayout;
	QVBoxLayout* arucoSubPageLayout = new QVBoxLayout;

	arucoSubPageLayout->addLayout( mSubmodulesStackL );

	kinectPageLayout->setAlignment( Qt::AlignBottom );
	arucoPageLayout->setAlignment( Qt::AlignBottom );
	arucoFaceRecPageLayout->setAlignment( Qt::AlignBottom );
	arucoLightDetPageLayout->setAlignment( Qt::AlignBottom );
	arucoMarkerPageLayout->setAlignment( Qt::AlignBottom );
	arucoMultiMarkerPageLayout->setAlignment( Qt::AlignBottom );

	buttonLayout->addWidget( kinectPageWid );
	buttonLayout->addWidget( arucoSubPageWid );

	mSubmodulesStackL->addWidget( arucoFaceRecPageWid );
	mSubmodulesStackL->addWidget( arucoMarkerPageWid );
	mSubmodulesStackL->addWidget( arucoLightDetPageWid );


	kinectPageWid->setLayout( kinectPageLayout );
	arucoPageWid->setLayout( arucoPageLayout );
	arucoFaceRecPageWid->setLayout( arucoFaceRecPageLayout );
	arucoLightDetPageWid->setLayout( arucoLightDetPageLayout );
	arucoMarkerPageWid->setLayout( arucoMarkerPageLayout );
	arucoMultiMarkerPageWid->setLayout( arucoMultiMarkerPageLayout );
	arucoSubPageWid->setLayout( arucoSubPageLayout );

	//set up page layouts
	kinectPageLayout->addWidget( mDisableCursorCB );
	kinectPageLayout->addWidget( mDisableZoomCursorCB );
	kinectPageLayout->addWidget( mEnableMarkerDetectCB );
	kinectPageLayout->addWidget( mEnableMarkerlessKinectCB );
	kinectPageLayout->addWidget( mSpeed );
	kinectPageLayout->addWidget( mKinectSnapshotPB );
	kinectPageLayout->addWidget( mKinectPB );

	//arucoPageLayout->addWidget( mMultiMarkerPB );

	arucoFaceRecPageLayout->addWidget( mRefEnableFaceRecCB );
	arucoFaceRecPageLayout->addWidget( mFaceNoVideo );
	arucoFaceRecPageLayout->addWidget( mFaceDetBackgrCB );

	// light detection panel
	arucoLightDetPageLayout->addWidget( mFisheyeXS );
	arucoLightDetPageLayout->addWidget( mFisheyeYS );
	arucoLightDetPageLayout->addWidget( mFisheyeRS );
	arucoLightDetPageLayout->addWidget( mLightDetectShowProcessingCB );
	arucoLightDetPageLayout->addWidget( mLightDetPB );

	arucoMarkerPageLayout->addWidget( mRefEnableMarkerlessCB );
	arucoMarkerPageLayout->addWidget( mMarkerNoVideo );
	arucoMarkerPageLayout->addWidget( mMarkerBackgrCB );
	arucoMarkerPageLayout->addWidget( mMarkerBehindCB );
	arucoMarkerPageLayout->addWidget( mCorEnabledCB );
	arucoMarkerPageLayout->addWidget( mMultiMarkerEnableCB );
	arucoMarkerPageLayout->addWidget( mEnableMarkerlessCameraCB );
	arucoMarkerPageLayout->addWidget( mUpdateCorParPB );
	arucoMarkerPageLayout->addWidget( mInterchangeMarkersPB );

	//arucoMultiMarkerPageLayout->addWidget( mMultiMarkerPB );

	if ( Util::ApplicationConfig::get()->getValue( "Viewer.SkyBox.Noise" ).toInt() < 2 ) {
		mMarkerBackgrCB->setDisabled( true );
	}

	//set layout
	mainLayout->addLayout( buttonLayout );
	mainLayout->addWidget( mWindowLabel, Qt::AlignCenter );
	mainLayout->setSizeConstraint( QLayout::SetMinimumSize );
	setLayout( mainLayout );
	adjustSize();

	//set up Widgets
	mUpdateCorParPB->setEnabled( false );

	mFaceNoVideo->setEnabled( false );
	mFaceDetBackgrCB->setEnabled( false );

	mMarkerNoVideo->setEnabled( false );
	mMarkerBackgrCB->setEnabled( false );
	mMarkerBehindCB->setEnabled( false );
	mCorEnabledCB->setEnabled( false );
	mMultiMarkerEnableCB->setEnabled( false );
	mEnableMarkerlessCameraCB->setEnabled( false );

	mEnableMarkerlessKinectCB->setEnabled( true );
	mEnableMarkerDetectCB->setEnabled( true );

	mLightDetectShowProcessingCB->setEnabled( true );

	mRefEnableFaceRecCB->setEnabled( true );
	mRefEnableMarkerlessCB->setEnabled( true );

//	mMultiMarkerPB->setCheckable( true );
	mKinectPB->setCheckable( true );
	mLightDetPB->setCheckable( true );

	//BUTTONS - set up signals to slots
	connect( mKinectPB, SIGNAL( clicked( bool ) ), this, SLOT( onKinectStartCancel( bool ) ) );
	connect( mKinectSnapshotPB, SIGNAL( clicked() ), this, SLOT( onKinectSnapshotPBClicked() ) );

	connect( mUpdateCorParPB, SIGNAL( clicked() ), this, SLOT( onUpdateCorPar() ) );

	//RADIOBUTTONS - set up signals to slots
	connect( mMultiMarkerRB, SIGNAL( clicked() ), this, SLOT( onSelSubModulChange() ) );

	//CHECKBOXES - set up signals to slots
	connect( mDisableCursorCB, SIGNAL( clicked() ), this, SLOT( stopMovingCursor() ) );
	connect( mDisableZoomCursorCB, SIGNAL( clicked( bool ) ), this, SLOT( stopZoom() ) );
	connect( mEnableMarkerDetectCB, SIGNAL( clicked( bool ) ), this, SLOT( setMarkerDetection( bool ) ) );
	connect( mEnableMarkerlessKinectCB, SIGNAL( clicked( bool ) ), this, SLOT( setMarkerlessDetectionKinect( bool ) ) );
	connect( mLightDetectShowProcessingCB, SIGNAL( clicked( bool ) ), this, SLOT( onLightDetShowProcessingCBClicked( bool ) ) );

	connect( mFisheyeXS, SIGNAL( valueChanged( int ) ), this, SLOT( onFisheyeXChanged( int ) ) );
	connect( mFisheyeYS, SIGNAL( valueChanged( int ) ), this, SLOT( onFisheyeYChanged( int ) ) );
	connect( mFisheyeRS, SIGNAL( valueChanged( int ) ), this, SLOT( onFisheyeRChanged( int ) ) );

	connect( mRefEnableFaceRecCB, SIGNAL( clicked( bool ) ), this, SLOT( onFaceRecStartCancel( bool ) ) );
	connect( mFaceNoVideo,	 SIGNAL( clicked() ), this, SLOT( onSelSubModulChange() ) );
	connect( mFaceDetBackgrCB, SIGNAL( clicked( bool ) ), this, SLOT( onFaceDetBackgrCBClicked( bool ) ) );

	connect( mMarkerNoVideo,	 SIGNAL( clicked() ), this, SLOT( onSelSubModulChange() ) );
	connect( mMarkerBackgrCB, SIGNAL( clicked( bool ) ), this, SLOT( onMarkerBackgrCBClicked( bool ) ) );
	connect( mEnableMarkerlessCameraCB, SIGNAL( clicked( bool ) ), this, SLOT( setMarkerlessDetectionCamera( bool ) ) );
	connect( mRefEnableMarkerlessCB, SIGNAL( clicked( bool ) ), this, SLOT( onMarkerStartCancel( bool ) ) );

}

void QOpenCV::OpenCVWindow::stopMovingCursor()
{
	if ( mDisableCursorCB->isChecked() ) {
		emit setMovementCursor( false );
	}
	else {
		emit setMovementCursor( true );
	}
}

void QOpenCV::OpenCVWindow::stopZoom()
{
	if ( mDisableZoomCursorCB->isChecked() ) {
		emit setZoom( false );
	}
	else {
		emit setZoom( true );
	}
}

void QOpenCV::OpenCVWindow::setMarkerDetection( bool set )
{
	emit setKinectMarkerDetection( set );
}

void QOpenCV::OpenCVWindow::setMarkerlessDetectionCamera( bool set )
{
	emit setCameraMarkerlessDetection( set );
}

void QOpenCV::OpenCVWindow::setMarkerlessDetectionKinect( bool set )
{
	emit setKinectMarkerlessDetection( set );
}

void QOpenCV::OpenCVWindow::setSpeedKinect( int speed )
{
	double _speed= speed/10.0 ;
	emit sendSpeedKinect( _speed );
}

void QOpenCV::OpenCVWindow::onKinectStartCancel( bool checked )
{
	if ( checked ) {
		qDebug() << "Startujem Kinect Thread";
		mKinectPB->setText( tr( "Pause Kinect" ) );
		emit startKinect();
		emit sendImageKinect( true );
	}
	else {
		qDebug() << "Pausujem Kinect Thread";
		mKinectPB->setText( tr( "Start Kinect" ) );
		emit sendImageKinect( false );
	}
}

void QOpenCV::OpenCVWindow::onKinectSnapshotPBClicked()
{
	qDebug() << "Sending signal to Kinect to take snapshot";
	emit setKinectCaptureImage( true );
}

void QOpenCV::OpenCVWindow::onFaceDetBackgrCBClicked( bool checked )
{
	qDebug() << "onFaceDetBackgrCBClicked " << checked;
	emit sendBackgrImgFaceRec( checked );
	if ( checked ) {
		emit sendBackgrImgMarker( false );
		mMarkerBackgrCB->setChecked( false );
	}
}
void QOpenCV::OpenCVWindow::onMarkerBackgrCBClicked( bool checked )
{
	qDebug() << "onMarkerBackgrCBClicked " << checked;
	emit sendBackgrImgMarker( checked );
	if ( checked ) {
		emit sendBackgrImgFaceRec( false );
		mFaceDetBackgrCB->setChecked( false );
	}
}


void QOpenCV::OpenCVWindow::onLightDetShowProcessingCBClicked( bool checked )
{
	qDebug() << "onLightDetShowProcessingCBClicked " << checked;
	emit sendShowProcessingCB( checked );
}

void QOpenCV::OpenCVWindow::onFisheyeXChanged( int value )
{
	emit sendFishEyeCenterX( value );
}

void QOpenCV::OpenCVWindow::onFisheyeYChanged( int value )
{
	emit sendFishEyeCenterY( value );
}

void QOpenCV::OpenCVWindow::onFisheyeRChanged( int value )
{
	emit sendFishEyeRadius( value );
}

void QOpenCV::OpenCVWindow::onSelModulChange()
{

}

void QOpenCV::OpenCVWindow::onSelSubModulChange() //---------------------------------------------------------
{
	if ( mMarkerNoVideo->isChecked() ) {
		emit sendImgMarker( false );
		emit setMultiMarker( false );
	}
	else {
		emit sendImgMarker( true );
	}

	if ( mFaceNoVideo->isChecked() ) {
		emit sendImgFaceRec( false );
	} else {
		emit sendImgFaceRec( true );
	}
}

void QOpenCV::OpenCVWindow::onUpdateCorPar()
{
	mUpdateCorParPB->setEnabled( false );
}

void QOpenCV::OpenCVWindow::onCorParUpdated()
{
	mUpdateCorParPB->setEnabled( true );
	mCorEnabledCB->setEnabled( true );
}

void QOpenCV::OpenCVWindow::onFaceRecStartCancel( bool checked )
{
	qDebug() << "OpenCVWindow::onFaceRecStartCancel:" << checked;

	if ( checked ) {

		emit setCapVideoFaceRec( OpenCV::CamSelectCore::getInstance()->selectCamera() );
		emit startFaceRec();

		if ( Util::ApplicationConfig::get()->getValue( "Viewer.SkyBox.Noise" ).toInt() >1 ) {
			mFaceDetBackgrCB->setEnabled( true );

		}
	}
	else {

		mFaceDetBackgrCB->setEnabled( false );
		emit stopFaceRec( true );

	}

	mFaceNoVideo->setEnabled( checked );

}

void QOpenCV::OpenCVWindow::onLightDetStartCancel( bool checked )
{
	qDebug() << "OpenCVWindow::onLightDetStartCancel:" << checked;

	if ( checked ) {
		mLightDetPB->setEnabled( false );
		mLightDetPB->setText( tr( "Stop Light Det." ) );
		emit setCapVideoLightDet( OpenCV::CamSelectCore::getInstance()->selectCamera() );
		emit startLightDet();
		mLightDetPB->setEnabled( true );
	}
	else {
		mLightDetPB->setEnabled( false );
		emit stopLightDet( true );
		mLightDetPB->setEnabled( true );
	}
}

void QOpenCV::OpenCVWindow::onMarkerStartCancel( bool checked )
{
	if ( checked ) {

		emit setCapVideoMarker( OpenCV::CamSelectCore::getInstance()->selectCamera() );
		emit startMarker();
		emit arucoRunning( true );

		if ( Util::ApplicationConfig::get()->getValue( "Viewer.SkyBox.Noise" ).toInt() ==0 ) {
			mMarkerBackgrCB->setEnabled( true );
		}
	}
	else {

		mCorEnabledCB->setEnabled( false );
		mMarkerBackgrCB->setEnabled( false );
		emit stopMarker( true );
		emit arucoRunning( false );
	}

	mMarkerNoVideo->setEnabled( checked );
	mMarkerBehindCB->setEnabled( checked );
	mUpdateCorParPB->setEnabled( checked );
	mMultiMarkerEnableCB->setEnabled( checked );
	mEnableMarkerlessCameraCB->setEnabled( checked );
	mInterchangeMarkersPB->setEnabled( checked );

}

void QOpenCV::OpenCVWindow::onFaceRecThrFinished()
{

}

void QOpenCV::OpenCVWindow::onLightDetThrFinished()
{
	mLightDetPB->setText( tr( "Start Light Det." ) );
	mLightDetPB->setEnabled( true );
}

void QOpenCV::OpenCVWindow::onMarkerThrFinished()
{

}

void QOpenCV::OpenCVWindow::closeEvent( QCloseEvent* event )
{
	qDebug() << "Closing OpenCV Window";

	disconnect();

	event->accept();
}

void QOpenCV::OpenCVWindow::onMultiMarkerStartCancel( bool checked )
{
	qDebug() << "OpenCVWindow::onMultiMarkerStartCancel " << checked;
	if ( checked ) {
		mMultiMarkerPB->setText( tr( "Stop Aruco" ) );
		emit setMultiMarker( true );
		emit setCapVideoMarker( OpenCV::CamSelectCore::getInstance()->selectCamera() );
		emit startMultiMarker();
	}
	else {
		mMultiMarkerPB->setText( tr( "Start Aruco" ) );
		emit stopMultiMarker( true );
		emit setMultiMarker( false );
	}
}

void QOpenCV::OpenCVWindow::setLabel( cv::Mat image )
{
	if ( image.empty() ) {
		mWindowLabel->setText( tr( "Image empty" ) );
		return;
	}

	QImage qimage( image.data, image.cols, image.rows,static_cast<int>( image.step ), QImage::Format_RGB888 );

	mWindowLabel->setPixmap( QPixmap::fromImage( qimage ) );

	image.~Mat();
}

void QOpenCV::OpenCVWindow::receiveCamResolution( int width, int height )
{

}
