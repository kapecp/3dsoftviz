#include "Kinect/KinectWindow.h"

#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QStackedLayout>
#include "QDebug"

using namespace Kinect;
using namespace cv;

Kinect::KinectWindow::KinectWindow(QWidget *parent, QApplication * app, KinectThread *thr): QDialog(parent)
{
	mApp=app;
	this->thr=thr;
	configureWindow();
}

QLabel *Kinect::KinectWindow::getLabel() const
{
	return mWindowLabel;
}

void Kinect::KinectWindow::configureWindow(void)
{
	setModal( false );
	resize(400,260);
	setWindowTitle( tr("Kinect Recognition"));

	mWindowLabel = new QLabel("",this,0);
	mWindowLabel->setFixedWidth(320);
	mWindowLabel->setFixedHeight(240);

	mKinectPause=new QPushButton(tr("Start"));
	connect(mKinectPause,SIGNAL(clicked()),this,SLOT(pausewindows()));

	mDisableCursor = new QCheckBox(tr("Turn off cursor"));
	connect(mDisableCursor,SIGNAL(clicked()),this,SLOT(stopMovingCursor()));

	//TODO add two different options for view kinect video
	mKinectColor = new QRadioButton( tr("Color Map"));
	mKinectDepthMap = new QRadioButton( tr("Depth map"));


	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	QVBoxLayout *buttonsLayout_2 = new QVBoxLayout;
	QHBoxLayout *frameLayout = new QHBoxLayout;
	QHBoxLayout *mainLayout = new QHBoxLayout;


	buttonsLayout->setAlignment(Qt::AlignTop);
	buttonsLayout->setAlignment(Qt::AlignTop);
	buttonsLayout_2->addWidget(mKinectPause);
	buttonsLayout_2->addWidget(mDisableCursor);
	frameLayout->setAlignment(Qt::AlignCenter);
	frameLayout->addWidget(mWindowLabel);

	buttonsLayout->addLayout(buttonsLayout_2);
	mainLayout->addLayout(buttonsLayout);
	mainLayout->addLayout(frameLayout);
	setLayout(mainLayout);

	adjustSize();

}

void Kinect::KinectWindow::stopMovingCursor()
{
	if(mDisableCursor->isChecked())
	{
		setMovementCursor(false);
	}
	else
	{
		setMovementCursor(true);
	}


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
void Kinect::KinectWindow::setLabelQ( QImage qimage )
{
	if( qimage.isNull() ) {
		mWindowLabel->setText( tr("Image empty"));
		return;
	}

	mWindowLabel->setPixmap( QPixmap::fromImage(qimage) );
}

void Kinect::KinectWindow::pausewindows()
{
	if (mKinectPause->text().toStdString().compare(tr("Start").toStdString())==0)
	{
		mKinectPause->setText(tr("Pause"));
		emit startKinect();
	}
	else if(mKinectPause->text().toStdString().compare(tr("Pause").toStdString())==0)
	{
		mKinectPause->setText(tr("Continue"));
		emit stopKinect(true);
	}
	else
	{
		mKinectPause->setText(tr("Pause"));
		emit stopKinect(false);
	}

}

void Kinect::KinectWindow::quitWindows()
{
	if(thr->isRunning())
	{
		emit stopKinect(true);
		thr->wait();
	}
	delete thr;
	delete this;

}

void Kinect::KinectWindow::closeEvent(QCloseEvent *event)
{
	qDebug() << "Kinect close event";

	emit sendImageKinect(false);
	emit stopKinect(true);

	disconnect();
	event->accept();
}

