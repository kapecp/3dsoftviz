#include "QOpenCV/FaceRecognitionWindow.h"
#include <QVBoxLayout>

using namespace QOpenCV;

QOpenCV::FaceRecognitionWindow::FaceRecognitionWindow(QWidget *parent, QApplication * app, QOpenCV::FaceRecognitionThread *thr)
{
    this->thr = thr;
	this->app = app;
	configureWindow();
}

void QOpenCV::FaceRecognitionWindow::configureWindow()
{
	setModal(false);
    resize(400,280);
    setWindowTitle(tr("Face Recognition and Marker Detection"));

	this->windowLabel = new QLabel("",this,0);
	this->windowLabel->setFixedWidth(320);
	this->windowLabel->setFixedHeight(240);

	this->pauseButton = new QPushButton(tr("Pause"));
	connect(this->pauseButton, SIGNAL(clicked()), this, SLOT(pauseWindow()));

	this->cancelButton = new QPushButton(tr("Cancel"));
	connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(quitWindow()));

    QVBoxLayout *buttonsLayout = new QVBoxLayout;   //changed to QVBox layout because of the second button layout we want underneath
	QVBoxLayout *buttonsLayout_2 = new QVBoxLayout;
	QHBoxLayout *frameLayout = new QHBoxLayout;
	QHBoxLayout *mainLayout = new QHBoxLayout;


    this->faceRadioButton = new QRadioButton("Face recognition");
    this->faceRadioButton->setChecked(true);
    this->markerRadioButton = new QRadioButton("Marker detection");
    QPushButton *switchButton = new QPushButton("Switch");
    connect(switchButton,SIGNAL(clicked()),this,SLOT(switchImage()));

    buttonsLayout->setAlignment(Qt::AlignLeft);
    buttonsLayout_2->setAlignment(Qt::AlignTop);
    //buttonsLayout_2->addWidget(this->pauseButton);
    //buttonsLayout_2->addWidget(this->cancelButton);
    buttonsLayout_2->addWidget(this->faceRadioButton);
    buttonsLayout_2->addWidget(this->markerRadioButton);
    buttonsLayout_2->addWidget(switchButton);

	frameLayout->setAlignment(Qt::AlignCenter);
	frameLayout->addWidget(this->windowLabel);
    this->thr->start();

    buttonsLayout->addLayout(buttonsLayout_2);  //WTF:D
    mainLayout->addLayout(buttonsLayout);

    //bottom buttons layout
    QVBoxLayout *bottomButtonsLayout_2 = new QVBoxLayout;

    //checkbox to define which of the threads we want running or stopped.
    this->faceCheckbox = new QCheckBox(tr("Face recognition"));
    this->markerCheckbox = new QCheckBox(tr("Marker detection"));
    QPushButton *startButton = new QPushButton("Start");
    //startButton->setDisabled(true); -needed? think about it.
    QPushButton *stopButton = new QPushButton("Stop");
    bottomButtonsLayout_2->addWidget(this->faceCheckbox);
    bottomButtonsLayout_2->addWidget(this->markerCheckbox);
    bottomButtonsLayout_2->addWidget(startButton);
    connect(startButton,SIGNAL(clicked()),this,SLOT(startThread()));
    bottomButtonsLayout_2->addWidget(stopButton);
    connect(stopButton,SIGNAL(clicked()),this,SLOT(stopThread()));
    buttonsLayout->addLayout(bottomButtonsLayout_2);

	mainLayout->addLayout(frameLayout);
	setLayout(mainLayout);



    connect(this->thr,SIGNAL(pushImage(cv::Mat)),this,SLOT(setLabel(cv::Mat)));

}

void QOpenCV::FaceRecognitionWindow::quitWindow()
{
	if(this->thr->isRunning()){
		emit cancelLoop(true);
		this->thr->wait();
	}
	if(!this->thr->isRunning())
		qDebug() << "thread is stopped";
	delete this->thr;
	delete this;
}

void QOpenCV::FaceRecognitionWindow::pauseWindow()
{
	if (this->pauseButton->text().toStdString().compare(tr("Pause").toStdString())==0)
	{
		this->pauseButton->setText(tr("Continue"));
		emit cancelLoop(true);
	}
	else
	{
		emit cancelLoop(false);
        this->pauseButton->setText(tr("Pause"));
		this->thr->start();
	}
}

QLabel *QOpenCV::FaceRecognitionWindow::getLabel()
{
	return this->windowLabel;
}

void QOpenCV::FaceRecognitionWindow::setLabel(cv::Mat image)
{
	//cv:: Mat image = this->thr->mCapVideo->getFrame();
	if (image.empty()==true) {
		this->windowLabel->setText("nenacitalo");
		return;
	}

	cv::cvtColor(image,image,CV_BGR2RGB);
	QImage qimage((uchar*) image.data, image.cols, image.rows,(int) image.step, QImage::Format_RGB888);

	image.~Mat();
	this->windowLabel->setPixmap(QPixmap::fromImage(qimage));
	this->windowLabel->show();
}

void QOpenCV::FaceRecognitionWindow::closeEvent(QCloseEvent *event)
{
	if(this->thr->isRunning()){
		emit cancelLoop(true);
		this->thr->wait();
	}
	if(!this->thr->isRunning())
		qDebug() << "thread is stopped";
	delete this->thr;
	delete this;
}

void QOpenCV::FaceRecognitionWindow::switchImage()
{
    if(this->faceRadioButton->isChecked()){
        //TBD: add connect/disconnect functionality for opencv - check if it is connected!
    }
    else if(this->markerRadioButton->isChecked()){
        //TBD: add connect/disconnect functionality for aruco - check if it is connected!
    }
}

void QOpenCV::FaceRecognitionWindow::startThread()
{
    if(this->faceCheckbox->isChecked()){
        if(!this->thr->isRunning()){
            emit cancelLoop(false);
            this->thr->start();
        }
    }
    if(this->markerCheckbox->isChecked()){
        //TBD: add the same for aruco as is for marker. Need to add aruco first.
    }
}

void QOpenCV::FaceRecognitionWindow::stopThread()
{
    if(this->faceCheckbox->isChecked()){
        if(this->thr->isRunning()){
            emit cancelLoop(true);
        }
    }
    if(this->markerCheckbox->isChecked()){
        //TBD: add the same for aruco as is for marker. Need to add aruco first.
    }
}
