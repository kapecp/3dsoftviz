#include "Kinect/KinectWindow.h"

#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QStackedLayout>
#include "QDebug"

#include <opencv2/imgproc/imgproc.hpp>




using namespace Kinect;
using namespace cv;

Kinect::KinectWindow::KinectWindow(QWidget *parent, QApplication * app): QDialog(parent)
{
	mApp=app;
	configureWindow();
}
QLabel *Kinect::KinectWindow::getLabel() const
{
	return mWindowLabel;
}

void Kinect::KinectWindow::configureWindow(void)
{
	setModal( false );
	setWindowTitle( tr("Kinect Recognition"));

	mWindowLabel = new QLabel("", this, 0);
	mKinectColor = new QRadioButton( tr("Face recognition"));
	mKinectDepthMap = new QRadioButton( tr("Marker detection"));

	QHBoxLayout *mainLayout		= new QHBoxLayout;
	QVBoxLayout *buttonLayout	= new QVBoxLayout;

	mKinectColor->setChecked(true);
	buttonLayout->addWidget( mKinectColor );
	buttonLayout->addWidget( mKinectDepthMap );

	buttonLayout->addWidget( mKinectStartStop );
	// set layout
	mainLayout->addLayout( buttonLayout );
	mainLayout->addWidget( mWindowLabel, Qt::AlignCenter );
	mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
	setLayout( mainLayout );
	adjustSize();

	connect(mKinectStartStop,SIGNAL(clicked(bool)),SLOT(onKinectStartCancel(bool)));


}

void Kinect::KinectWindow::closeEvent(QCloseEvent *event)
{
	qDebug() << "Close Kinect Window";
	disconnect();
	event->accept();

}

void Kinect::KinectWindow::setLabel(cv::Mat image)
{
	if( image.empty() ) {
		mWindowLabel->setText( tr("Image empty"));
		return;
	}
	cv::cvtColor( image, image, CV_BGR2RGB );
	QImage qimage( (uchar*) image.data, image.cols, image.rows,(int) image.step, QImage::Format_RGB888);

	mWindowLabel->setPixmap( QPixmap::fromImage(qimage));

	image.~Mat();

}

void Kinect::KinectWindow::onKinectStartCancel(bool checked)
{

	if(checked)
	{
		mKinectStartStop->setText(tr("Stop Kinect"));
		emit startKinect();
	}

	else{
		mKinectStartStop->setEnabled(false);
		emit stopKinect();
	}

}



















