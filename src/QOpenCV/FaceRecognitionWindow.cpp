#include "QOpenCV/FaceRecognitionWindow.h"

#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QCheckBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QStackedLayout>
#include <QCloseEvent>
#include "OpenCV/CamSelectCore.h"

#include "Util/ApplicationConfig.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <QDebug>

<<<<<<< Updated upstream
QOpenCV::FaceRecognitionWindow::FaceRecognitionWindow( QWidget* parent, QApplication* app )
	: QDialog( parent )
=======
QOpenCV::FaceRecognitionWindow::FaceRecognitionWindow(QWidget *parent, QApplication * app )
    : QDialog(parent)
>>>>>>> Stashed changes
{
    mApp		= app;
    configureWindow();
}


QLabel* QOpenCV::FaceRecognitionWindow::getLabel() const
{
    return mWindowLabel;
}

QCheckBox*	QOpenCV::FaceRecognitionWindow::getMarkerBehindCB() const
{
    return mMarkerBehindCB;
}

QCheckBox*	QOpenCV::FaceRecognitionWindow::getCorEnabledCB() const
{
    return mCorEnabledCB;
}

QPushButton*	QOpenCV::FaceRecognitionWindow::getUpdateCorParPB() const
{
    return mUpdateCorParPB;
}

QPushButton* QOpenCV::FaceRecognitionWindow::getInterchangeMarkersPB() const
{
    return mInterchangeMarkersPB;
}

void QOpenCV::FaceRecognitionWindow::configureWindow()
{
<<<<<<< Updated upstream
	setModal( false );
	setWindowTitle( tr( "Face Recognition and Marker Detection" ) );

	mWindowLabel			= new QLabel( "", this, 0 );
	mFaceRecRB				= new QRadioButton( tr( "Face recognition" ) );
	mMarkerRB				= new QRadioButton( tr( "Marker detection" ) );
	mNoVideo				= new QCheckBox( tr( "NoVideo" ) );
	mModulesStackL			= new QStackedLayout;
	mFaceRecStartCancelPB	= new QPushButton( tr( "Start FaceRec" ) );
	mMarkerStartCancelPB	= new QPushButton( tr( "Start Marker" ) );

	mMarkerBackgrCB			= new QCheckBox( tr( "Background" ) );
	mFaceDetBackgrCB		= new QCheckBox( tr( "Background" ) );

	mMarkerBehindCB			= new QCheckBox( tr( "Marker is behind" ) );
	mCorEnabledCB			= new QCheckBox( tr( "Correction" ) );
	mUpdateCorParPB			= new QPushButton( tr( "Update cor. param." ) );
	mInterchangeMarkersPB	= new QPushButton( tr( "Change Markers" ) );



	QHBoxLayout* mainLayout		= new QHBoxLayout;
	QVBoxLayout* buttonLayout	= new QVBoxLayout;

	mFaceRecRB->setChecked( true );
	buttonLayout->addWidget( mFaceRecRB );
	buttonLayout->addWidget( mMarkerRB );
	buttonLayout->addWidget( mNoVideo );
	buttonLayout->addLayout( mModulesStackL );


	// face detection and marker controll buttons
	QWidget*		faceRecPageWid		= new QWidget;
	QWidget*		markerPageWid		= new QWidget;
	QVBoxLayout*	faceRecPageLayout	= new QVBoxLayout;
	QVBoxLayout*	markerPageLayout	= new QVBoxLayout;
	faceRecPageLayout->setAlignment( Qt::AlignBottom );
	markerPageLayout->setAlignment( Qt::AlignBottom );
	mModulesStackL->addWidget( faceRecPageWid );
	mModulesStackL->addWidget( markerPageWid );
	faceRecPageWid->setLayout( faceRecPageLayout );
	markerPageWid->setLayout( markerPageLayout );

	faceRecPageLayout->addWidget( mFaceDetBackgrCB );
	faceRecPageLayout->addWidget( mFaceRecStartCancelPB );

	markerPageLayout->addWidget( mMarkerBackgrCB );
	markerPageLayout->addWidget( mMarkerBehindCB );
	markerPageLayout->addWidget( mCorEnabledCB );
	markerPageLayout->addWidget( mUpdateCorParPB );
	markerPageLayout->addWidget( mInterchangeMarkersPB );

	markerPageLayout->addWidget( mMarkerStartCancelPB );


	if ( Util::ApplicationConfig::get()->getValue( "Viewer.SkyBox.Noise" ).toInt() < 2 ) {
		mMarkerBackgrCB->setDisabled( true );
	}




	// set layout
	mainLayout->addLayout( buttonLayout );
	mainLayout->addWidget( mWindowLabel, Qt::AlignCenter );
	mainLayout->setSizeConstraint( QLayout::SetMinimumSize );
	setLayout( mainLayout );
	adjustSize();

	// this could be changed
	mFaceDetBackgrCB->setEnabled( false );
	mMarkerBackgrCB->setEnabled( false );
	mFaceDetBackgrCB->setChecked( false );
	mMarkerBackgrCB->setChecked( false );
	mFaceRecStartCancelPB->setCheckable( true );
	mMarkerStartCancelPB->setCheckable( true );
	mMarkerBehindCB->setEnabled( false );
	mCorEnabledCB->setEnabled( false );
	mUpdateCorParPB->setEnabled( false );

	// connections on private slots
	connect( mFaceRecRB, SIGNAL( clicked() ), this, SLOT( onSelModulChange() ) );
	connect( mMarkerRB,	 SIGNAL( clicked() ), this, SLOT( onSelModulChange() ) );
	connect( mNoVideo,	 SIGNAL( clicked() ), this, SLOT( onSelModulChange() ) );

	connect( mUpdateCorParPB, SIGNAL( clicked() ), this, SLOT( onUpdateCorPar() ) );
	connect( mMarkerStartCancelPB,  SIGNAL( clicked( bool ) ), this, SLOT( onMarkerStartCancel( bool ) ) );
	connect( mFaceRecStartCancelPB, SIGNAL( clicked( bool ) ), this, SLOT( onFaceRecStartCancel( bool ) ) );

	connect( mMarkerBackgrCB, SIGNAL( clicked( bool ) ), this, SLOT( onMarkerBackgrCBClicked( bool ) ) );
	connect( mFaceDetBackgrCB, SIGNAL( clicked( bool ) ), this, SLOT( onFaceDetBackgrCBClicked( bool ) ) );
=======
    setModal( false );
    setWindowTitle( tr("Face Recognition and Marker Detection"));

    mWindowLabel			= new QLabel("", this, 0);
    mFaceRecRB				= new QRadioButton( tr("Face recognition"));
    mMarkerRB				= new QRadioButton( tr("Marker detection"));
    mMultiMarkerRB          = new QRadioButton( tr("MultiMarker recognition"));
    mNoVideo				= new QCheckBox( tr("NoVideo"));
    mModulesStackL			= new QStackedLayout;
    mFaceRecStartCancelPB	= new QPushButton( tr("Start FaceRec"));
    mMarkerStartCancelPB	= new QPushButton( tr("Start Marker"));
    //Garaj start
    mMultiMarkerStartCancelPB = new QPushButton( tr("Start MultiMarker"));
    //Garaj end

    mMarkerBackgrCB			= new QCheckBox( tr("Background"));
    mFaceDetBackgrCB		= new QCheckBox( tr("Background"));

    mMarkerBehindCB			= new QCheckBox( tr("Marker is behind"));
    mCorEnabledCB			= new QCheckBox( tr("Correction"));
    mUpdateCorParPB			= new QPushButton( tr("Update cor. param."));
    mInterchangeMarkersPB	= new QPushButton( tr("Change Markers"));



    QHBoxLayout *mainLayout		= new QHBoxLayout;
    QVBoxLayout *buttonLayout	= new QVBoxLayout;

    mFaceRecRB->setChecked(true);
    buttonLayout->addWidget( mFaceRecRB );
    buttonLayout->addWidget( mMarkerRB );
    buttonLayout->addWidget( mMultiMarkerRB );
    buttonLayout->addWidget( mNoVideo );
    buttonLayout->addLayout( mModulesStackL );


    // face detection and marker controll buttons
    QWidget		*faceRecPageWid		= new QWidget;
    QWidget		*markerPageWid		= new QWidget;
    QWidget     *multimarkerPageWid = new QWidget;

    QVBoxLayout	*faceRecPageLayout	= new QVBoxLayout;
    QVBoxLayout	*markerPageLayout	= new QVBoxLayout;
    QVBoxLayout *multimarkerPageLayout = new QVBoxLayout;

    faceRecPageLayout->setAlignment( Qt::AlignBottom );
    markerPageLayout->setAlignment( Qt::AlignBottom );
    multimarkerPageLayout->setAlignment( Qt::AlignBottom );

    mModulesStackL->addWidget( faceRecPageWid );
    mModulesStackL->addWidget( markerPageWid );
    mModulesStackL->addWidget( multimarkerPageWid );

    faceRecPageWid->setLayout( faceRecPageLayout );
    markerPageWid->setLayout( markerPageLayout );
    multimarkerPageWid->setLayout( multimarkerPageLayout );

    faceRecPageLayout->addWidget( mFaceDetBackgrCB );
    faceRecPageLayout->addWidget( mFaceRecStartCancelPB );

    markerPageLayout->addWidget( mMarkerBackgrCB );
    markerPageLayout->addWidget( mMarkerBehindCB );
    markerPageLayout->addWidget( mCorEnabledCB );
    markerPageLayout->addWidget( mUpdateCorParPB );
    markerPageLayout->addWidget( mInterchangeMarkersPB );

    markerPageLayout->addWidget( mMarkerStartCancelPB );

    multimarkerPageLayout->addWidget( mMultiMarkerStartCancelPB );


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
    mMultiMarkerStartCancelPB->setCheckable(true);
    mMarkerBehindCB->setEnabled(false);
    mCorEnabledCB->setEnabled(false);
    mUpdateCorParPB->setEnabled(false);

    // connections on private slots
    connect( mFaceRecRB, SIGNAL(clicked()), this, SLOT(onSelModulChange()) );
    connect( mMarkerRB,	 SIGNAL(clicked()), this, SLOT(onSelModulChange()) );
    connect( mMultiMarkerRB, SIGNAL(clicked()), this, SLOT(onSelModulChange()) );
    connect( mNoVideo,	 SIGNAL(clicked()), this, SLOT(onSelModulChange()) );

    connect( mUpdateCorParPB, SIGNAL(clicked()), this, SLOT(onUpdateCorPar()) );
    connect( mMarkerStartCancelPB,  SIGNAL(clicked(bool)), this, SLOT(onMarkerStartCancel(bool)) );
    connect( mFaceRecStartCancelPB, SIGNAL(clicked(bool)), this, SLOT(onFaceRecStartCancel(bool)) );
    connect( mMultiMarkerStartCancelPB, SIGNAL(clicked(bool)), this, SLOT(onMultiMarkerStartCancel(bool)) );

    connect( mMarkerBackgrCB, SIGNAL(clicked(bool)), this, SLOT(onMarkerBackgrCBClicked(bool)) );
    connect( mFaceDetBackgrCB, SIGNAL(clicked(bool)), this, SLOT(onFaceDetBackgrCBClicked(bool)) );
>>>>>>> Stashed changes
}

void QOpenCV::FaceRecognitionWindow::onFaceDetBackgrCBClicked( bool checked )
{
    qDebug() << "onFaceDetBackgrCBClicked " << checked;
    emit sendBackgrImgFaceRec( checked );

<<<<<<< Updated upstream
	if ( checked ) {
		emit sendBackgrImgMarker( false );
		mMarkerBackgrCB->setChecked( false );
	}
=======
    if( checked){
        emit sendBackgrImgMarker(false);
        mMarkerBackgrCB->setChecked(false);
    }
>>>>>>> Stashed changes
}
void QOpenCV::FaceRecognitionWindow::onMarkerBackgrCBClicked( bool checked )
{
    qDebug() << "onMarkerBackgrCBClicked " << checked;
    emit sendBackgrImgMarker( checked );

<<<<<<< Updated upstream
	if ( checked ) {
		emit sendBackgrImgFaceRec( false );
		mFaceDetBackgrCB->setChecked( false );
	}
=======
    if( checked){
        emit sendBackgrImgFaceRec(false);
        mFaceDetBackgrCB->setChecked(false);
    }
>>>>>>> Stashed changes
}


void QOpenCV::FaceRecognitionWindow::onSelModulChange()
{
<<<<<<< Updated upstream
	if ( mFaceRecRB->isChecked() ) {
		mModulesStackL->setCurrentIndex( 0 );
	}
	if ( mMarkerRB->isChecked() ) {
		mModulesStackL->setCurrentIndex( 1 );
	}


	if ( mNoVideo->isChecked() ) {
		emit sendImgFaceRec( false );
		emit sendImgMarker( false );

	}
	else {
		// face recognition
		if ( mFaceRecRB->isChecked() ) {
			emit sendImgMarker( false );
			emit sendImgFaceRec( true );
		}
		// marker
		if ( mMarkerRB->isChecked() ) {
			emit sendImgFaceRec( false );
			emit sendImgMarker( true );
		}
	}
=======
    if( mFaceRecRB->isChecked() ){
        mModulesStackL->setCurrentIndex(0);
    }
    if( mMarkerRB->isChecked() ){
        mModulesStackL->setCurrentIndex(1);
    }
    if( mMultiMarkerRB->isChecked() ){
        mModulesStackL->setCurrentIndex(2);
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
>>>>>>> Stashed changes

}

void QOpenCV::FaceRecognitionWindow::onUpdateCorPar()
{
<<<<<<< Updated upstream
	mUpdateCorParPB->setEnabled( false );
=======
    mUpdateCorParPB->setEnabled(false);
>>>>>>> Stashed changes
}

void QOpenCV::FaceRecognitionWindow::onCorParUpdated()
{
<<<<<<< Updated upstream
	mUpdateCorParPB->setEnabled( true );
	mCorEnabledCB->setEnabled( true );
=======
    mUpdateCorParPB->setEnabled(true);
    mCorEnabledCB->setEnabled(true);
>>>>>>> Stashed changes
}

void QOpenCV::FaceRecognitionWindow::onFaceRecStartCancel( bool checked )
{
<<<<<<< Updated upstream
	if ( checked ) {
		mFaceRecStartCancelPB->setEnabled( false );
		mFaceRecStartCancelPB->setText( tr( "Stop FaceRec" ) );
		emit setCapVideoFaceRec( OpenCV::CamSelectCore::getInstance()->selectCamera() );
		emit startFaceRec();
		mFaceRecStartCancelPB->setEnabled( true );

		if ( Util::ApplicationConfig::get()->getValue( "Viewer.SkyBox.Noise" ).toInt() >1 ) {
			mFaceDetBackgrCB->setEnabled( true );

		}
	}
	else {
		mFaceRecStartCancelPB->setEnabled( false );
		mFaceDetBackgrCB->setEnabled( false );
		emit stopFaceRec( true );

	}
=======
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
>>>>>>> Stashed changes
}

void QOpenCV::FaceRecognitionWindow::onMarkerStartCancel( bool checked )
{
<<<<<<< Updated upstream
	if ( checked ) {
		mMarkerStartCancelPB->setEnabled( false );
		mMarkerStartCancelPB->setText( tr( "Stop Marker" ) );
		mMarkerBehindCB->setEnabled( true );
		mUpdateCorParPB->setEnabled( true );
		emit setCapVideoMarker( OpenCV::CamSelectCore::getInstance()->selectCamera() );
		emit startMarker();
		mMarkerStartCancelPB->setEnabled( true );

		if ( Util::ApplicationConfig::get()->getValue( "Viewer.SkyBox.Noise" ).toInt() >1 ) {
			mMarkerBackgrCB->setEnabled( true );
		}
	}
	else {
		mMarkerStartCancelPB->setEnabled( false );
		mMarkerBehindCB->setEnabled( false );
		mCorEnabledCB->setEnabled( false );
		mUpdateCorParPB->setEnabled( false );
		mMarkerBackgrCB->setEnabled( false );
		emit stopMarker( true );

	}
=======
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
>>>>>>> Stashed changes
}

void QOpenCV::FaceRecognitionWindow::onFaceRecThrFinished()
{
<<<<<<< Updated upstream
	mFaceRecStartCancelPB->setText( tr( "Start FaceRec" ) );
	mFaceRecStartCancelPB->setEnabled( true );
=======
    mFaceRecStartCancelPB->setText( tr("Start FaceRec"));
    mFaceRecStartCancelPB->setEnabled(true);
>>>>>>> Stashed changes
}

void QOpenCV::FaceRecognitionWindow::onMarkerThrFinished()
{
<<<<<<< Updated upstream
	mMarkerStartCancelPB->setText( tr( "Start Marker" ) );
	mMarkerStartCancelPB->setEnabled( true );
=======
    mMarkerStartCancelPB->setText( tr("Start Marker"));
    mMarkerStartCancelPB->setEnabled(true);
}

void QOpenCV::FaceRecognitionWindow::onMultiMarkerStartCancel(bool checked) {
    if( checked ){
        qDebug() << "Clicked MultiMarkerButton" << checked;
        mMultiMarkerStartCancelPB->setText( tr("Stop MultiMarker"));
        emit setMultiMarker( true );
        emit setCapVideoMarker( OpenCV::CamSelectCore::getInstance()->selectCamera());
        emit startMultiMarker();
    } else {
        qDebug() << "Clicked MultiMarkerButton" << checked;
        mMultiMarkerStartCancelPB->setText( tr("Start MultiMarker"));
        emit stopMultiMarker(true);
        emit setMultiMarker( false );
    }
>>>>>>> Stashed changes
}

void QOpenCV::FaceRecognitionWindow::setLabel( cv::Mat image )
{
<<<<<<< Updated upstream
	if ( image.empty() ) {
		mWindowLabel->setText( tr( "Image empty" ) );
		return;
	}

	QImage qimage( reinterpret_cast<uchar*>( image.data ), image.cols, image.rows,static_cast<int>( image.step ), QImage::Format_RGB888 );

	mWindowLabel->setPixmap( QPixmap::fromImage( qimage ) );
=======
    if( image.empty() ) {
        mWindowLabel->setText( tr("Image empty"));
        return;
    }

    QImage qimage( reinterpret_cast<uchar*>(image.data), image.cols, image.rows,static_cast<int>(image.step), QImage::Format_RGB888);

    mWindowLabel->setPixmap( QPixmap::fromImage(qimage));
>>>>>>> Stashed changes

    image.~Mat();
}

void QOpenCV::FaceRecognitionWindow::setLabelQ( QImage qimage )
{
<<<<<<< Updated upstream
	if ( qimage.isNull() ) {
		mWindowLabel->setText( tr( "Image empty" ) );
		return;
	}

	mWindowLabel->setPixmap( QPixmap::fromImage( qimage ) );
=======
    if( qimage.isNull() ) {
        mWindowLabel->setText( tr("Image empty"));
        return;
    }

    mWindowLabel->setPixmap( QPixmap::fromImage(qimage) );
>>>>>>> Stashed changes

}

void QOpenCV::FaceRecognitionWindow::closeEvent( QCloseEvent* event )
{

    qDebug() << "ARWindow closeEvent";

    // stop send images from threads
    mNoVideo->setChecked( true );
    onSelModulChange();

    disconnect();  // disconnect unnecessary connection, while dialog is hidden(disabling sending image to window)

    event->accept();
}





