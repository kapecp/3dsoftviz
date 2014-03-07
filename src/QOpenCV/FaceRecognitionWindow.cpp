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
	: QDialog(parent)
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
	setWindowTitle( tr("Face Recognition and Marker Detection"));

	mWindowLabel			= new QLabel("", this, 0);
	mFaceRecRB				= new QRadioButton( tr("Face recognition"));
	mMarkerRB				= new QRadioButton( tr("Marker detection"));
	mNoVideo				= new QCheckBox( tr("NoVideo"));
	mModulesStackL			= new QStackedLayout;
	mFaceRecStartCancelPB	= new QPushButton( tr("Start FaceRec"));
	mMarkerStartCancelPB	= new QPushButton( tr("Start Marker"));

	mMarkerBehindCB			= new QCheckBox( tr("Marker is behind"));
	mCorEnabledCB			= new QCheckBox( tr("Corection"));
	mUpdateCorParPB			= new QPushButton( tr("Update cor. param."));

	QHBoxLayout *mainLayout		= new QHBoxLayout;
	QVBoxLayout *buttonLayout	= new QVBoxLayout;

	mFaceRecRB->setChecked(true);
	buttonLayout->addWidget( mFaceRecRB );
	buttonLayout->addWidget( mMarkerRB );
	buttonLayout->addWidget( mNoVideo );
	buttonLayout->addLayout( mModulesStackL );


	// aruco and marker controll buttons
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


	// set layout
	mainLayout->addLayout( buttonLayout );
	mainLayout->addWidget( mWindowLabel, Qt::AlignCenter );
	mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
	setLayout( mainLayout );
	adjustSize();

	// this could be changed
	mFaceRecStartCancelPB->setCheckable(true);
	mMarkerStartCancelPB->setCheckable(true);
	mMarkerBehindCB->setEnabled(false);
	mCorEnabledCB->setEnabled(false);
	mUpdateCorParPB->setEnabled(false);

	// connections on private slots
	connect( mFaceRecRB, SIGNAL(clicked()), this, SLOT(onSelModulChange()) );
	connect( mMarkerRB,	 SIGNAL(clicked()), this, SLOT(onSelModulChange()) );
	connect( mNoVideo,	 SIGNAL(clicked()), this, SLOT(onSelModulChange()) );

	connect( mUpdateCorParPB, SIGNAL(clicked()), this, SLOT(onUpdateCorPar()) );
	connect( mMarkerStartCancelPB,  SIGNAL(clicked(bool)), this, SLOT(onMarkerStartCancel(bool)) );
	connect( mFaceRecStartCancelPB, SIGNAL(clicked(bool)), this, SLOT(onFaceRecStartCancel(bool)) );

}


void QOpenCV::FaceRecognitionWindow::onSelModulChange()
{
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

void QOpenCV::FaceRecognitionWindow::onUpdateCorPar()
{
	mUpdateCorParPB->setEnabled(false);
}

void QOpenCV::FaceRecognitionWindow::onCorParUpdated()
{
	mUpdateCorParPB->setEnabled(true);
	mCorEnabledCB->setEnabled(true);
}

void QOpenCV::FaceRecognitionWindow::onFaceRecStartCancel( bool checked )
{
	if( checked ) {
		//qDebug() << "shoul start faceRec";
		mFaceRecStartCancelPB->setText( tr("Stop FaceRec"));
		emit startFaceRec();

	} else {
		//qDebug() << "shoul stop facerec";
		mFaceRecStartCancelPB->setEnabled(false);
		emit stopFaceRec(true);

	}
}

void QOpenCV::FaceRecognitionWindow::onMarkerStartCancel( bool checked )
{
	if( checked ) {
		//qDebug() << "shoul start aruco";
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

void QOpenCV::FaceRecognitionWindow::onFaceRecThrFinished()
{
	//qDebug() << "face rec shoud be stopped";
	mFaceRecStartCancelPB->setText( tr("Start FaceRec"));
	mFaceRecStartCancelPB->setEnabled(true);
}
void QOpenCV::FaceRecognitionWindow::onMarkerThrFinished()
{
	mMarkerStartCancelPB->setText( tr("Start Marker"));
	mMarkerStartCancelPB->setEnabled(true);
}


void QOpenCV::FaceRecognitionWindow::setLabel(cv::Mat image)
{
	if( image.empty() ) {
		mWindowLabel->setText( tr("Image empty"));
		return;
	}

	cv::cvtColor( image, image, CV_BGR2RGB );
	QImage qimage( (uchar*) image.data, image.cols, image.rows,(int) image.step, QImage::Format_RGB888);

	mWindowLabel->setPixmap( QPixmap::fromImage(qimage));

	//image.~Mat();    //?????????
	//mWindowLabel->show();
}


// ---------------------------------




void QOpenCV::FaceRecognitionWindow::quitWindow()
{
	if( mThrFaceRec->isRunning()){
		//emit cancelLoop(true);
		mThrFaceRec->wait();
	}
	if( ! mThrFaceRec->isRunning())
		qDebug() << "thread is stopped";
	delete mThrFaceRec;
	delete this;
}


void QOpenCV::FaceRecognitionWindow::closeEvent(QCloseEvent *event)
{
	if( mThrFaceRec->isRunning()){
		//emit cancelLoop(true);
		mThrFaceRec->wait();
	}
	if( ! mThrFaceRec->isRunning())
		qDebug() << "thread is stopped";
	//delete mThrFaceRec;
	event->accept();
}





