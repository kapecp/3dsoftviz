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

			mKinectSTop=new QPushButton(tr("Cancel"));
	connect(mKinectSTop,SIGNAL(clicked()),this,SLOT(quitWindows()));

	//mModulesStackL			= new QStackedLayout;
	mKinectColor = new QRadioButton( tr("Color Map"));
	mKinectDepthMap = new QRadioButton( tr("Depth map"));

	//mModulesStackL->addWidget( mKinectColor );
	//mModulesStackL->addWidget( mKinectDepthMap );


	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	QVBoxLayout *buttonsLayout_2 = new QVBoxLayout;
	QHBoxLayout *frameLayout = new QHBoxLayout;
	QHBoxLayout *mainLayout = new QHBoxLayout;


	buttonsLayout->setAlignment(Qt::AlignTop);
	buttonsLayout->setAlignment(Qt::AlignTop);
	buttonsLayout_2->addWidget(mKinectPause);
	buttonsLayout_2->addWidget(mKinectSTop);
	frameLayout->setAlignment(Qt::AlignCenter);
	frameLayout->addWidget(mWindowLabel);

	buttonsLayout->addLayout(buttonsLayout_2);
	mainLayout->addLayout(buttonsLayout);
	mainLayout->addLayout(frameLayout);
	setLayout(mainLayout);


	/*mWindowLabel = new QLabel("", this, 0);
	mKinectColor = new QRadioButton( tr("Color Map"));
	mKinectDepthMap = new QRadioButton( tr("Depth map"));

	QHBoxLayout *mainLayout		= new QHBoxLayout;
	QVBoxLayout *buttonLayout	= new QVBoxLayout;

	mKinectColor->setChecked(true);
	buttonLayout->addWidget( mKinectColor );
	buttonLayout->addWidget( mKinectDepthMap );

	//buttonLayout->addWidget( mKinectStartStop );
	// set layout
	mainLayout->addLayout( buttonLayout );
	mainLayout->addWidget( mWindowLabel, Qt::AlignCenter );
	mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
	setLayout( mainLayout );*/
	adjustSize();

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


void Kinect::KinectWindow::onKinectStartCancel(bool checked)
{

	if(checked)
	{
		mKinectStartStop->setText(tr("Stop Kinect"));
		emit startKinect();
	}
	else
	{
		mKinectStartStop->setEnabled(false);
		emit stopKinect(false);
	}

}

void Kinect::KinectWindow::pausewindows()
{
	if (mKinectPause->text().toStdString().compare(tr("Pause").toStdString())==0)
	{
		mKinectPause->setText(tr("Continue"));
		emit startKinect();
	}
	else
	{
		emit startKinect();
		mKinectPause->setText(tr("Pause"));

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

