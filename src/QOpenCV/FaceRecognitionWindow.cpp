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

    //radio buttons for switching the camera
    QRadioButton *radio1 = new QRadioButton("Face recognition");
    QRadioButton *radio2 = new QRadioButton("Marker detection");
    QPushButton *switchButton = new QPushButton("Switch");

    buttonsLayout->setAlignment(Qt::AlignLeft);
    buttonsLayout_2->setAlignment(Qt::AlignTop);
    //buttonsLayout_2->addWidget(this->pauseButton);
    //buttonsLayout_2->addWidget(this->cancelButton);
    buttonsLayout_2->addWidget(radio1);
    buttonsLayout_2->addWidget(radio2);
    buttonsLayout_2->addWidget(switchButton);

	frameLayout->setAlignment(Qt::AlignCenter);
	frameLayout->addWidget(this->windowLabel);

    buttonsLayout->addLayout(buttonsLayout_2);  //WTF:D
    mainLayout->addLayout(buttonsLayout);

    //bottom buttons layout
    QHBoxLayout *bottomButtonsLayout = new QHBoxLayout;
    bottomButtonsLayout->setAlignment(Qt::AlignBottom);
    QVBoxLayout *bottomButtonsLayout_2 = new QVBoxLayout;

    //checkbox to define which of the threads we want running or stopped.
    QCheckBox *checkbox1 = new QCheckBox(tr("Face recognition"));
    QCheckBox *checkbox2 = new QCheckBox(tr("Marker detection"));
    QPushButton *startButton = new QPushButton("Start");
    bottomButtonsLayout_2->addWidget(checkbox1);
    bottomButtonsLayout_2->addWidget(checkbox2);
    bottomButtonsLayout_2->addWidget(startButton);
    buttonsLayout->addLayout(bottomButtonsLayout_2);
   // mainLayout->addLayout(bottomButtonsLayout);

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
