#include "QOpenCV/OpenCVWindow.h"

#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QStackedLayout>
#include <QCloseEvent>

QOpenCV::OpenCVWindow::OpenCVWindow( QWidget *parent, QApplication *app ) : QDialog( parent )
{
    mApp = app;
    configureWindow();
}

QLabel* QOpenCV::OpenCVWindow::getLabel() const
{
    return mWindowLabel;
}

void QOpenCV::OpenCVWindow::configureWindow()
{
    setModal( false );
    setWindowTitle( tr("Kinect and Aruco Window") );

    mWindowLabel = new QLabel( "", this, 0 );
    mKinectRB = new QRadioButton( tr("Kinect") );
    mArucoRB = new QRadioButton( tr("Aruco") );

    mFaceRecRB = new QRadioButton( tr("Face Recognition") );
    mMarkerRB = new QRadioButton( tr("Marker") );
    mMultiMarkerRB = new QRadioButton( tr("Multi Marker") );

    mArucoPB = new QPushButton( tr("Start Aruco") );
    mKinectPB = new QPushButton( tr("Start Kinect") );

    mModulesStackL = new QStackedLayout;
    mSubmodulesStackL = new QStackedLayout;

    QHBoxLayout *mainLayout		= new QHBoxLayout;
    QVBoxLayout *buttonLayout	= new QVBoxLayout;

    mKinectRB->setChecked( true );
    buttonLayout->addWidget( mKinectRB );
    buttonLayout->addWidget( mArucoRB );
    buttonLayout->addLayout( mModulesStackL );

    QWidget *kinectPageWid =  new QWidget;
    QWidget *arucoPageWid =  new QWidget;

    QVBoxLayout	*kinectPageLayout	= new QVBoxLayout;
    QVBoxLayout	*arucoPageLayout	= new QVBoxLayout;

    kinectPageLayout->setAlignment( Qt::AlignBottom );
    arucoPageLayout->setAlignment( Qt::AlignBottom );

    mModulesStackL->addWidget( kinectPageWid );
    mModulesStackL->addWidget( arucoPageWid );

    kinectPageWid->setLayout( kinectPageLayout );
    arucoPageWid->setLayout( arucoPageLayout );

    kinectPageLayout->addWidget( mKinectPB );
    arucoPageLayout->addWidget( mArucoPB );

    //set layout
    mainLayout->addLayout( buttonLayout );
    mainLayout->addWidget( mWindowLabel, Qt::AlignCenter );
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
    setLayout( mainLayout );
    adjustSize();

    mArucoPB->setEnabled( true );
    mKinectPB->setEnabled( false );

    mArucoPB->setCheckable( true );

    connect( mKinectRB, SIGNAL(clicked()), this, SLOT(onSelModulChange()) );
    connect( mArucoRB, SIGNAL(clicked()), this, SLOT(onSelModulChange()) );

    connect( mArucoPB, SIGNAL(clicked(bool)), this, SLOT(onMultiMarkerStartCancel(bool)) );
}

void QOpenCV::OpenCVWindow::onSelModulChange()
{
    if( mKinectRB->isChecked() )
    {
        mModulesStackL->setCurrentIndex(0);
    }

    if( mArucoRB->isChecked() )
    {
        mModulesStackL->setCurrentIndex(1);
    }
}

void QOpenCV::OpenCVWindow::closeEvent( QCloseEvent *event )
{
    qDebug() << "Closing OpenCV Window";

    disconnect();

    event->accept();
}

void QOpenCV::OpenCVWindow::onMultiMarkerStartCancel( bool checked )
{
    qDebug() << "checked = " << checked;
    if( checked )
    {
        mArucoPB->setText( tr("Stop Aruco") );
        emit setMultiMarker( true );
        emit setCapVideoMarker( OpenCV::CamSelectCore::getInstance()->selectCamera() );
        emit startMultiMarker();
    } else {
        mArucoPB->setText( tr("Start Aruco") );
        emit stopMultiMarker(true);
        emit setMultiMarker( false );

    }
}

void QOpenCV::OpenCVWindow::setLabel( cv::Mat image )
{
    if( image.empty() ) {
        mWindowLabel->setText( tr("Image empty"));
        return;
    }

    QImage qimage( reinterpret_cast<uchar*>(image.data), image.cols, image.rows,static_cast<int>(image.step), QImage::Format_RGB888);

    mWindowLabel->setPixmap( QPixmap::fromImage(qimage));

    image.~Mat();
}
