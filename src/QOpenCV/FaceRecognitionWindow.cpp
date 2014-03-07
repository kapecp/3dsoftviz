#include "QOpenCV/FaceRecognitionWindow.h"

#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QStackedLayout>

#include "QOpenCV/FaceRecognitionThread.h"
#include "Aruco/arucothread.h"


using namespace QOpenCV;

QOpenCV::FaceRecognitionWindow::FaceRecognitionWindow(QWidget *parent, QApplication * app, QOpenCV::FaceRecognitionThread *thrFaceRec, ArucoModul::ArucoThread *thrAruco)
{
	mThrFaceRec = thrFaceRec;
	mThrAruco	= thrAruco;
	mApp		= app;

	configureWindow();

}

QLabel *QOpenCV::FaceRecognitionWindow::getLabel() const
{
	return mWindowLabel;
}

QCheckBox	*QOpenCV::FaceRecognitionWindow::getMarkerBehindCB() const
{
	return mMarkerBehindCB;
}

QCheckBox	*QOpenCV::FaceRecognitionWindow::getCorEnabledCB() const
{
	return mCorEnabledCB;
}

QPushButton	*QOpenCV::FaceRecognitionWindow::getUpdateCorParPB() const
{
	return mUpdateCorParPB;
}

void QOpenCV::FaceRecognitionWindow::configureWindow()
{
	setModal( false );
	resize( 400, 280 );
	setWindowTitle( tr("Face Recognition and Marker Detection"));

	createGui();
	/*
	mWindowLabel = new QLabel("", this, 0);
	mWindowLabel->setFixedWidth( 320 );
	mWindowLabel->setFixedHeight( 240 );


	mPauseButton = new QPushButton( tr("Pause"));
	connect( mPauseButton, SIGNAL(clicked()), this, SLOT(pauseWindow()) );

	mCancelButton = new QPushButton( tr("Cancel"));
	connect( mCancelButton, SIGNAL(clicked()), this, SLOT(quitWindow()) );

	QVBoxLayout *buttonsLayout		= new QVBoxLayout;   //changed to QVBox layout because of the second button layout we want underneath
	QVBoxLayout *buttonsLayout_2	= new QVBoxLayout;
	QHBoxLayout *frameLayout		= new QHBoxLayout;
	QHBoxLayout *mainLayout			= new QHBoxLayout;


	mFaceRadioButton	= new QRadioButton( tr("Face recognition"));
	mFaceRadioButton->setChecked(true);
	mMarkerRadioButton	= new QRadioButton( tr("Marker detection"));
	QPushButton *switchButton	= new QPushButton( tr("Switch"));
	connect( switchButton, SIGNAL(clicked()), this, SLOT(switchImage()) );

	buttonsLayout->setAlignment( Qt::AlignLeft );
	buttonsLayout_2->setAlignment( Qt::AlignTop );
	buttonsLayout_2->addWidget( mFaceRadioButton );
	buttonsLayout_2->addWidget( mMarkerRadioButton );
	buttonsLayout_2->addWidget( switchButton );

	frameLayout->setAlignment( Qt::AlignCenter );
	frameLayout->addWidget( mWindowLabel );

	buttonsLayout->addLayout( buttonsLayout_2 );  //WTF:D
	mainLayout->addLayout( buttonsLayout );

	//bottom buttons layout
	QVBoxLayout *bottomButtonsLayout_2 = new QVBoxLayout;

	//checkbox to define which of the threads we want running or stopped.
	mFaceCheckbox	= new QCheckBox( tr("Face recognition"));
	mMarkerCheckbox = new QCheckBox( tr("Marker detection"));
	QPushButton *startButton	= new QPushButton( tr("Start"));
	//startButton->setDisabled(true); -needed? think about it.
	QPushButton *stopButton		= new QPushButton( tr("Stop"));
	bottomButtonsLayout_2->addWidget( mFaceCheckbox );
	bottomButtonsLayout_2->addWidget( mMarkerCheckbox );
	bottomButtonsLayout_2->addWidget( startButton );
	connect( startButton, SIGNAL(clicked()), this, SLOT(startThread()) );
	bottomButtonsLayout_2->addWidget( stopButton );
	connect( stopButton, SIGNAL(clicked()), this, SLOT(stopThread()) );
	buttonsLayout->addLayout( bottomButtonsLayout_2 );

	mainLayout->addLayout( frameLayout );
	setLayout( mainLayout );



	connect( mThrFaceRec, SIGNAL(pushImage(cv::Mat)), this, SLOT(setLabel(cv::Mat)) );
	*/
}

void QOpenCV::FaceRecognitionWindow::createGui(){

	mWindowLabel = new QLabel("", this, 0);
	mWindowLabel->setFixedWidth( 320 );
	mWindowLabel->setFixedHeight( 240 );

	QHBoxLayout *mainLayout			= new QHBoxLayout;
	QVBoxLayout *buttonLayout		= new QVBoxLayout;   //changed to QVBox layout because of the second button layout we want underneath
	QHBoxLayout *frameLayout		= new QHBoxLayout;






	mFaceRecRB				= new QRadioButton( tr("Face recognition"));
	mMarkerRB				= new QRadioButton( tr("Marker detection"));
	mNoVideo				= new QCheckBox( tr("NoVideo"));
	mModulesStackL			= new QStackedLayout;
	mFaceRecStartCancelPB	= new QPushButton( tr("Start FaceRec"));
	mMarkerStartCancelPB	= new QPushButton( tr("Start Marker"));

	mMarkerBehindCB			= new QCheckBox( tr("Marker is behind"));
	mCorEnabledCB			= new QCheckBox( tr("Corection"));
	mUpdateCorParPB			= new QPushButton( tr("Update cor. param."));

	connect( mFaceRecRB, SIGNAL(clicked()), this, SLOT(onSelModulChange()) );
	connect( mMarkerRB,	 SIGNAL(clicked()), this, SLOT(onSelModulChange()) );
	connect( mNoVideo,	 SIGNAL(clicked()), this, SLOT(onSelModulChange()) );

	mFaceRecRB->setChecked(true);
	buttonLayout->addWidget( mFaceRecRB );
	buttonLayout->addWidget( mMarkerRB );
	buttonLayout->addWidget( mNoVideo );
	buttonLayout->addLayout( mModulesStackL );



	QWidget		*faceRecPageWid		= new QWidget;
	QWidget		*markerPageWid		= new QWidget;
	QVBoxLayout	*faceRecPageLayout	= new QVBoxLayout;
	QVBoxLayout	*markerPageLayout	= new QVBoxLayout;
	faceRecPageLayout->setAlignment( Qt::AlignBottom );
	markerPageLayout->setAlignment( Qt::AlignBottom );
	mModulesStackL->addWidget( faceRecPageWid );
	mModulesStackL->addWidget( markerPageWid );
	faceRecPageWid->setLayout( faceRecPageLayout );
	markerPageWid->setLayout( markerPageLayout );


	faceRecPageLayout->addWidget( mFaceRecStartCancelPB );

	markerPageLayout->addWidget( mMarkerBehindCB );
	markerPageLayout->addWidget( mCorEnabledCB );
	markerPageLayout->addWidget( mUpdateCorParPB );
	markerPageLayout->addWidget( mMarkerStartCancelPB );





	frameLayout->setAlignment( Qt::AlignCenter );
	frameLayout->addWidget( mWindowLabel );

	mainLayout->addLayout( buttonLayout );
	mainLayout->addLayout( frameLayout );
	setLayout( mainLayout );

	//connect( , SIGNAL, , SLOT );
	mFaceRecStartCancelPB->setCheckable(true);
	mMarkerStartCancelPB->setCheckable(true);
	mMarkerBehindCB->setEnabled(false);
	mCorEnabledCB->setEnabled(false);
	mUpdateCorParPB->setEnabled(false);

	connect( mUpdateCorParPB, SIGNAL(clicked()), this, SLOT(onUpdateCorPar()) );

	connect( mMarkerStartCancelPB, SIGNAL(clicked(bool)), this, SLOT(onMarkerStartCancel(bool)) );
	connect( mFaceRecStartCancelPB, SIGNAL(clicked(bool)), this, SLOT(onFaceRecStartCancel(bool)) );


}
void QOpenCV::FaceRecognitionWindow::onSelModulChange(){
	if( mNoVideo->isChecked() ){
		emit sendImgFaceRec(false);
		emit sendImgMarker(false);

	} else{
		// face recognition
		if( mFaceRecRB->isChecked() ){
			mModulesStackL->setCurrentIndex(0);
			emit sendImgMarker(false);
			emit sendImgFaceRec(true);
		}
		// marker
		if( mMarkerRB->isChecked() ){
			mModulesStackL->setCurrentIndex(1);
			emit sendImgFaceRec(false);
			emit sendImgMarker(true);
		}

	}
}


void QOpenCV::FaceRecognitionWindow::onUpdateCorPar(){
	mUpdateCorParPB->setEnabled(false);
}

void QOpenCV::FaceRecognitionWindow::onCorParUpdated(){
	mUpdateCorParPB->setEnabled(true);
	mCorEnabledCB->setEnabled(true);
}

void QOpenCV::FaceRecognitionWindow::onFaceRecStartCancel( bool checked ){
	if( checked ) {
		mFaceRecStartCancelPB->setText( tr("Stop FaceRec"));
		emit startFaceRec();

	} else {
		mFaceRecStartCancelPB->setEnabled(false);
		emit stopFaceRec(true);

	}
}

void QOpenCV::FaceRecognitionWindow::onMarkerStartCancel( bool checked ){
	if( checked ) {
		qDebug() << "shoul start aruco";
		mMarkerStartCancelPB->setText( tr("Stop Marker"));
		mMarkerBehindCB->setEnabled(true);
		mUpdateCorParPB->setEnabled(true);
		emit startMarker();



	} else {
		qDebug() << "shoul start stoping aruco";
		mMarkerStartCancelPB->setEnabled(false);
		mMarkerBehindCB->setEnabled(false);
		mCorEnabledCB->setEnabled(false);
		mUpdateCorParPB->setEnabled(false);
		emit stopMarker(true);

	}
}

void QOpenCV::FaceRecognitionWindow::onFaceRecThrFinished(){
	mFaceRecStartCancelPB->setText( tr("Start FaceRec"));
	mFaceRecStartCancelPB->setEnabled(true);
}
void QOpenCV::FaceRecognitionWindow::onMarkerThrFinished(){
	mMarkerStartCancelPB->setText( tr("Start Marker"));
	mMarkerStartCancelPB->setEnabled(true);
}






void QOpenCV::FaceRecognitionWindow::quitWindow()
{
	if( mThrFaceRec->isRunning()){
		emit cancelLoop(true);
		mThrFaceRec->wait();
	}
	if( ! mThrFaceRec->isRunning())
		qDebug() << "thread is stopped";
	delete mThrFaceRec;
	delete this;
}

void QOpenCV::FaceRecognitionWindow::pauseWindow()
{
	if( mPauseButton->text().toStdString().compare( tr("Pause").toStdString()) == 0 )
	{
		mPauseButton->setText( tr("Continue"));
		emit cancelLoop(true);
	}
	else
	{
		emit cancelLoop(false);
		mPauseButton->setText( tr("Pause"));
		mThrFaceRec->start();
	}
}

void QOpenCV::FaceRecognitionWindow::closeEvent(QCloseEvent *event)
{
	if( mThrFaceRec->isRunning()){
		emit cancelLoop(true);
		mThrFaceRec->wait();
	}
	if( ! mThrFaceRec->isRunning())
		qDebug() << "thread is stopped";
	delete mThrFaceRec;
	delete this;
}

void QOpenCV::FaceRecognitionWindow::setLabel(cv::Mat image)
{
	if( image.empty() ) {
		mWindowLabel->setText( tr("nenacitalo"));
		return;
	}

	cv::cvtColor( image, image, CV_BGR2RGB );
	QImage qimage( (uchar*) image.data, image.cols, image.rows,(int) image.step, QImage::Format_RGB888);

	mWindowLabel->setPixmap( QPixmap::fromImage(qimage));

	//image.~Mat();    //?????????
	//mWindowLabel->show();
}



void QOpenCV::FaceRecognitionWindow::switchImage()
{
	if( mFaceRadioButton->isChecked() ){
		//TBD: add connect/disconnect functionality for opencv - check if it is connected!
	}
	else if( mMarkerRadioButton->isChecked() ){
		//TBD: add connect/disconnect functionality for aruco - check if it is connected!
	}
}

void QOpenCV::FaceRecognitionWindow::startThread()
{
	if( mFaceCheckbox->isChecked() ){
		if( ! mThrFaceRec->isRunning()){
			emit cancelLoop(false);
			mThrFaceRec->start();
		}
	}
	if( mMarkerCheckbox->isChecked()) {
		//TBD: add the same for aruco as is for marker. Need to add aruco first.
	}
}

void QOpenCV::FaceRecognitionWindow::stopThread()
{
	if( mFaceCheckbox->isChecked() ){
		if( mThrFaceRec->isRunning() ){
			emit cancelLoop(true);
		}
	}
	if( mMarkerCheckbox->isChecked() ){
		//TBD: add the same for aruco as is for marker. Need to add aruco first.
	}
}
