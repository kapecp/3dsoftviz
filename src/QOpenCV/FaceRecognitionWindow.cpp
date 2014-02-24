#include "QOpenCV/FaceRecognitionWindow.h"

using namespace QOpenCV;

QOpenCV::FaceRecognitionWindow::FaceRecognitionWindow(QWidget *parent, QApplication * app, QOpenCV::FaceRecognitionThread *thr): QDialog(parent)
{
	this->thr = thr;
	this->app = app;
	configureWindow();
}

void QOpenCV::FaceRecognitionWindow::configureWindow()
{
	setModal(false);
	resize(400,260);
	setWindowTitle(tr("Face Recognition"));

	this->windowLabel = new QLabel("",this,0);
	this->windowLabel->setFixedWidth(320);
	this->windowLabel->setFixedHeight(240);

	this->pauseButton = new QPushButton(tr("Pause"));
	connect(this->pauseButton, SIGNAL(clicked()), this, SLOT(pauseWindow()));

	this->cancelButton = new QPushButton(tr("Cancel"));
	connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(quitWindow()));

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	QVBoxLayout *buttonsLayout_2 = new QVBoxLayout;
	QHBoxLayout *frameLayout = new QHBoxLayout;
	QHBoxLayout *mainLayout = new QHBoxLayout;

	buttonsLayout->setAlignment(Qt::AlignTop);
	buttonsLayout_2->addWidget(this->pauseButton);
	buttonsLayout_2->addWidget(this->cancelButton);
	frameLayout->setAlignment(Qt::AlignCenter);
	frameLayout->addWidget(this->windowLabel);

	buttonsLayout->addLayout(buttonsLayout_2);
	mainLayout->addLayout(buttonsLayout);
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
	QImage qimage((uchar*) image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);

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
