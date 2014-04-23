#include "QOpenCV/FaceRecognitionWindow.h"

#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QStackedLayout>
#include "OpenCV/CamSelectCore.h"

#include "Util/ApplicationConfig.h"



using namespace QOpenCV;

QOpenCV::FaceRecognitionWindow::FaceRecognitionWindow(QWidget *parent, QApplication * app )
	: QDialog(parent)
{
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

	mMarkerBackgrCB			= new QCheckBox( tr("Background"));
	mFaceDetBackgrCB		= new QCheckBox( tr("Background"));

	mMarkerBehindCB			= new QCheckBox( tr("Marker is behind"));
	mCorEnabledCB			= new QCheckBox( tr("Corection"));
	mUpdateCorParPB			= new QPushButton( tr("Update cor. param."));

	QHBoxLayout *mainLayout		= new QHBoxLayout;
	QVBoxLayout *buttonLayout	= new QVBoxLayout;
	/*
	mFaceDetBackgrCB
	mMarkerBackgrCB
		*/
	mFaceRecRB->setChecked(true);
	buttonLayout->addWidget( mFaceRecRB );
	buttonLayout->addWidget( mMarkerRB );
	buttonLayout->addWidget( mNoVideo );
	buttonLayout->addLayout( mModulesStackL );


	// face detection and marker controll buttons
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

	faceRecPageLayout->addWidget( mFaceDetBackgrCB );
	faceRecPageLayout->addWidget( mFaceRecStartCancelPB );


	markerPageLayout->addWidget( mMarkerBehindCB );
	markerPageLayout->addWidget( mCorEnabledCB );
	markerPageLayout->addWidget( mUpdateCorParPB );

	markerPageLayout->addWidget( mMarkerBackgrCB );
	markerPageLayout->addWidget( mMarkerStartCancelPB );


	if(Util::ApplicationConfig::get()->getValue("Viewer.SkyBox.Noise").toInt() < 2)
	{
		mMarkerBackgrCB->setDisabled(true);
	}


	// set layout
	mainLayout->addLayout( buttonLayout );
	mainLayout->addWidget( mWindowLabel, Qt::AlignCenter );
	mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
	setLayout( mainLayout );
	adjustSize();

	// this could be changed
	mFaceDetBackgrCB->setEnabled(false);
	mMarkerBackgrCB->setEnabled(false);
	mFaceDetBackgrCB->setChecked(false);
	mMarkerBackgrCB->setChecked(false);
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

	connect( mMarkerBackgrCB, SIGNAL(clicked(bool)), this, SLOT(onMarkerBackgrCBClicked(bool)) );
	connect( mFaceDetBackgrCB, SIGNAL(clicked(bool)), this, SLOT(onFaceDetBackgrCBClicked(bool)) );
}

void QOpenCV::FaceRecognitionWindow::onFaceDetBackgrCBClicked(bool checked)
{
	qDebug() << "onFaceDetBackgrCBClicked " << checked;
	emit sendBackgrImgFaceRec( checked );

	if( checked){
		emit sendBackgrImgMarker(false);
		mMarkerBackgrCB->setChecked(false);
	}
}
void QOpenCV::FaceRecognitionWindow::onMarkerBackgrCBClicked(bool checked)
{
	qDebug() << "onMarkerBackgrCBClicked " << checked;
	emit sendBackgrImgMarker( checked );

	if( checked){
		emit sendBackgrImgFaceRec(false);
		mFaceDetBackgrCB->setChecked(false);
	}
}


void QOpenCV::FaceRecognitionWindow::onSelModulChange()
{
	if( mFaceRecRB->isChecked() ){
		mModulesStackL->setCurrentIndex(0);
	}
	if( mMarkerRB->isChecked() ){
		mModulesStackL->setCurrentIndex(1);
	}


	if( mNoVideo->isChecked() ){
		emit sendImgFaceRec(false);
		emit sendImgMarker(false);

	} else{
		// face recognition
		if( mFaceRecRB->isChecked() ){
			emit sendImgMarker(false);
			emit sendImgFaceRec(true);
		}
		// marker
		if( mMarkerRB->isChecked() ){
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
		mFaceRecStartCancelPB->setEnabled(false);
		mFaceRecStartCancelPB->setText( tr("Stop FaceRec"));
		emit setCapVideoFaceRec( OpenCV::CamSelectCore::getInstance()->selectCamera());
		emit startFaceRec();
		mFaceRecStartCancelPB->setEnabled(true);

		if(Util::ApplicationConfig::get()->getValue("Viewer.SkyBox.Noise").toInt() >1)
		{
			mFaceDetBackgrCB->setEnabled(true);

		}
	} else {
		mFaceRecStartCancelPB->setEnabled(false);
		mFaceDetBackgrCB->setEnabled(false);
		emit stopFaceRec(true);

	}
}

void QOpenCV::FaceRecognitionWindow::onMarkerStartCancel( bool checked )
{
	if( checked ) {
		mMarkerStartCancelPB->setEnabled(false);
		mMarkerStartCancelPB->setText( tr("Stop Marker"));
		mMarkerBehindCB->setEnabled(true);
		mUpdateCorParPB->setEnabled(true);
		emit setCapVideoMarker( OpenCV::CamSelectCore::getInstance()->selectCamera());
		emit startMarker();
		mMarkerStartCancelPB->setEnabled(true);

		if(Util::ApplicationConfig::get()->getValue("Viewer.SkyBox.Noise").toInt() >1)
		{
			mMarkerBackgrCB->setEnabled(true);
		}
	} else {
		mMarkerStartCancelPB->setEnabled(false);
		mMarkerBehindCB->setEnabled(false);
		mCorEnabledCB->setEnabled(false);
		mUpdateCorParPB->setEnabled(false);
		mMarkerBackgrCB->setEnabled(false);
		emit stopMarker(true);

	}
}

void QOpenCV::FaceRecognitionWindow::onFaceRecThrFinished()
{
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

	image.~Mat();    //?????????
}

void QOpenCV::FaceRecognitionWindow::setLabelQ( QImage qimage )
{
	if( qimage.isNull() ) {
		mWindowLabel->setText( tr("Image empty"));
		return;
	}

	mWindowLabel->setPixmap( QPixmap::fromImage(qimage) );
}

void QOpenCV::FaceRecognitionWindow::closeEvent(QCloseEvent *event)
{

	qDebug() << "ARWindow closeEvent";

	// stop send images from threads
	mNoVideo->setChecked( true );
	onSelModulChange();

	disconnect();

	//deleteLater();   // its efective not destroy it, but disconnect its only, it will remeber its state

	event->accept();
}





