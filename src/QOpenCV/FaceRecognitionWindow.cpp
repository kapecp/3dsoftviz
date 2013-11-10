#include "QOpenCV/FaceRecognitionWindow.h"

using namespace QOpenCV;

QOpenCV::FaceRecognitionWindow::FaceRecognitionWindow(QWidget *parent, QApplication * app, QOpenCV::FaceRecognitionThread *thr)
{
    this->thr = thr;
    this->app=app;
    configureWindow();
}

void QOpenCV::FaceRecognitionWindow::configureWindow()
{
    setModal(false);
    resize(600,250);
    setWindowTitle(tr("Face Recognition"));

    windowLabel = new QLabel("",this,0);
    windowLabel->setFixedWidth(640);
    windowLabel->setFixedHeight(480);

    QPushButton * pauseButton = new QPushButton(tr("Pause"));
    connect(pauseButton, SIGNAL(clicked()), this, SLOT(pauseWindow()));

    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    cancelButton->setFocusPolicy(Qt::NoFocus);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(quitWindow()));

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    QVBoxLayout *buttonsLayout_2 = new QVBoxLayout;
    QHBoxLayout *frameLayout = new QHBoxLayout;
    QHBoxLayout *mainLayout = new QHBoxLayout;

    buttonsLayout->setAlignment(Qt::AlignTop);
    buttonsLayout_2->addWidget(pauseButton);
    buttonsLayout_2->addWidget(cancelButton);
    frameLayout->setAlignment(Qt::AlignCenter);
    frameLayout->addWidget(windowLabel);

    buttonsLayout->addLayout(buttonsLayout_2);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addLayout(frameLayout);
    setLayout(mainLayout);

    connect(this->thr,SIGNAL(pushImage(cv::Mat)),this,SLOT(setLabel(cv::Mat)));
}

void QOpenCV::FaceRecognitionWindow::quitWindow()
{
    emit cancelLoop();
    //this->thr->quit();
    this->thr->wait(500);
    this->close();
}

void QOpenCV::FaceRecognitionWindow::pauseWindow()
{
    //emit
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

    this->windowLabel->setPixmap(QPixmap::fromImage(qimage));
    this->windowLabel->show();
}
