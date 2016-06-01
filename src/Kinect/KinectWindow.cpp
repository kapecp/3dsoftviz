#include "Kinect/KinectWindow.h"
#include "Kinect/KinectThread.h"

#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QStackedLayout>
#include <QtGui/QSlider>
#include <QCloseEvent>
#include "QDebug"

Kinect::KinectWindow::KinectWindow( QWidget* parent, QApplication* app, KinectThread* thr ): QDialog( parent )
{
	mApp=app;
	this->thr=thr;
	// create window - parts
	configureWindow();
}

QLabel* Kinect::KinectWindow::getLabel() const
{
	return mWindowLabel;
}

void Kinect::KinectWindow::configureWindow( void )
{
	setModal( false );
	resize( 400,260 );
	setWindowTitle( tr( "Kinect Recognition" ) );

	mWindowLabel = new QLabel( "",this,0 );
	mWindowLabel->setFixedWidth( 320 );
	mWindowLabel->setFixedHeight( 240 );

	mKinectPause=new QPushButton( tr( "Start" ) );
	connect( mKinectPause,SIGNAL( clicked() ),this,SLOT( pausewindows() ) );

	mDisableCursor = new QCheckBox( tr( "Turn off cursor" ) );
	connect( mDisableCursor,SIGNAL( clicked() ),this,SLOT( stopMovingCursor() ) );
	mDisableZoomCursor = new QCheckBox( tr( "Turn off zoom" ) );
	connect( mDisableZoomCursor,SIGNAL( clicked() ),this,SLOT( stopZoom() ) );

	mSpeed= new QSlider( Qt::Vertical );
	mSpeed->setRange( 5,20 );
	mSpeed->setValue( 10 );
	mSpeed->setPageStep( 1 );
	mSpeed->setFocusPolicy( Qt::NoFocus );
	mSpeed->setToolTip( "Modify speed of movement" );
	connect( mSpeed,SIGNAL( valueChanged( int ) ),this,SLOT( setSpeedKinect( int ) ) );


	//TODO add two different options for view kinect video
	mKinectColor = new QRadioButton( tr( "Color Map" ) );
	mKinectDepthMap = new QRadioButton( tr( "Depth map" ) );


	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	QVBoxLayout* buttonsLayout_2 = new QVBoxLayout;
	QHBoxLayout* frameLayout = new QHBoxLayout;
	QHBoxLayout* mainLayout = new QHBoxLayout;


	buttonsLayout->setAlignment( Qt::AlignTop );
	buttonsLayout->setAlignment( Qt::AlignTop );
	buttonsLayout_2->addWidget( mKinectPause );
	buttonsLayout_2->addWidget( mDisableCursor );
	buttonsLayout_2->addWidget( mDisableZoomCursor );
	buttonsLayout_2->addWidget( mSpeed );
	frameLayout->setAlignment( Qt::AlignCenter );
	frameLayout->addWidget( mWindowLabel );

	buttonsLayout->addLayout( buttonsLayout_2 );
	mainLayout->addLayout( buttonsLayout );
	mainLayout->addLayout( frameLayout );
	setLayout( mainLayout );

	adjustSize();

}

void Kinect::KinectWindow::stopMovingCursor()
{
	if ( mDisableCursor->isChecked() ) {
		emit setMovementCursor( false );
	}
	else {
		emit setMovementCursor( true );
	}
}

void Kinect::KinectWindow::stopZoom()
{
	if ( mDisableZoomCursor->isChecked() ) {
		emit setZoom( false );
	}
	else {
		emit setZoom( true );
	}
}

void Kinect::KinectWindow::setSpeedKinect( int speed )
{
	double _speed= speed/10.0;
	emit sendSpeedKinect( _speed );
}

void Kinect::KinectWindow::setLabel( cv::Mat image )
{
	if ( image.empty() ) {
		mWindowLabel->setText( tr( "Image empty" ) );
		return;
	}
	//convert to QImage - for Label
	cv::cvtColor( image, image, CV_BGR2RGB );
	QImage qimage( image.data, image.cols, image.rows, static_cast<int>( image.step ), QImage::Format_RGB888 );

	mWindowLabel->setPixmap( QPixmap::fromImage( qimage ) );

	image.~Mat();

}
void Kinect::KinectWindow::setLabelQ( QImage qimage )
{
	if ( qimage.isNull() ) {
		mWindowLabel->setText( tr( "Image empty" ) );
		return;
	}

	mWindowLabel->setPixmap( QPixmap::fromImage( qimage ) );
}

void Kinect::KinectWindow::pausewindows()
{
	bool start=false;
	//check button name, based on that stop pause and start
	if ( mKinectPause->text().toStdString().compare( tr( "Start" ).toStdString() )==0 ) {
		//inicialize openni and start device
		//	start = thr->inicializeKinect();
		if ( start ) {
			emit startKinect();

			emit sendImageKinect( true );
			mKinectPause->setText( tr( "Pause" ) );
		}
	}
	else if ( mKinectPause->text().toStdString().compare( tr( "Pause" ).toStdString() )==0 ) {
		mKinectPause->setText( tr( "Continue" ) );
		emit sendImageKinect( false );
		//this->thr->closeActionOpenni();
	}
	else {
		//inicialize openni and start device
		//start = thr->inicializeKinect();
		if ( start ) {
			mKinectPause->setText( tr( "Pause" ) );
			emit sendImageKinect( true );
		}
	}

}

void Kinect::KinectWindow::quitWindows()
{
	// stop and destroy object //TODO use better closeEvent
	if ( this->thr->isRunning() ) {
		emit stopKinect( true );
		this->thr->wait();
	}
	delete this->thr;
	delete this;

}

void Kinect::KinectWindow::closeEvent( QCloseEvent* event )
{
	qDebug() << "Kinect close event";

	emit sendImageKinect( false );
	emit stopKinect( true );

	disconnect();
	event->accept();
	this->deleteLater();
}

