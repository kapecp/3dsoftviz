#include "Mouse3d/LibMouse3d/Mouse3d.h"

#if defined(Q_OS_LINUX)
	#include <QX11Info>

	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/Xatom.h>
	#include <X11/keysym.h>

	#define SGI // Required by xdrvlib.h
	extern "C" {
		#include "xdrvlib.h"
	}

	#include "fixx11.h"
#endif

namespace LibMouse3d{

    Mouse3dDevice::Mouse3dDevice( QOSG::CoreWindow *window ) :
    window( window ) {
#if defined(Q_OS_WIN)

        this->mouse = new Mouse3DInput(window);
        QObject::connect(mouse, SIGNAL(Move3d(std::vector<float>&)), window, SLOT(OnMove(std::vector<float>&)));

#elif defined(Q_OS_LINUX)

		initMouse3DLinux();

#elif defined(Q_OS_MAC)
#endif

    }

    Mouse3dDevice::~Mouse3dDevice(){
    }

#if defined(Q_OS_WIN)
#elif defined(Q_OS_LINUX)

	void Mouse3dDevice::translateX11Event( XEvent *event ) {
		qDebug() << "Mouse3dDevice::Mouse3DLinux: eventType = " << event->type;
		/*
		MagellanFloatEvent MagellanEvent;
		if ( MagellanTranslateEvent( this->display, &event, &MagellanEvent, 1.0, 1.0 ) == MagellanInputMotionEvent) {
			MagellanRemoveMotionEvents( display );

			std::vector<float> motionData;
			motionData.push_back( MagellanEvent.MagellanData[ MagellanX ] );
			motionData.push_back( MagellanEvent.MagellanData[ MagellanY ] );
			motionData.push_back( MagellanEvent.MagellanData[ MagellanZ ] * -1.0);
			motionData.push_back( MagellanEvent.MagellanData[ MagellanA ] );
			motionData.push_back( MagellanEvent.MagellanData[ MagellanC ] * -1.0);
			motionData.push_back( MagellanEvent.MagellanData[ MagellanB ] * -1.0);

			// Axi X,Y,Z scaling
			motionData[0] /= 100.0;
			motionData[1] /= 100.0;
			motionData[2] /= 100.0;


//			// Rotation A,B,C normalization to <0.0 - 1.0>
//			float normAxis = sqrt(motionData[3] * motionData[3] + motionData[4] * motionData[4] + motionData[5] * motionData[5]);
//			if (normAxis == 0.0){
//				motionData[3] = 0.0;
//				motionData[4] = 0.0;
//				motionData[5] = 1.0;
//			}
//			else{
//				motionData[3] /= normAxis;
//				motionData[4] /= normAxis;
//				motionData[5] /= normAxis;
//			}

//			motionData[3] /= 10.0;
//			motionData[4] /= 10.0;
//			motionData[5] /= 10.0;


			// Rotation A,B,C scaling
			motionData[3] /= 7000.0;
			motionData[4] /= 7000.0;
			motionData[5] /= 7000.0;

			qDebug() <<  "Mouse3d x=" << motionData[0] <<
						"y=" << motionData[1] <<
						"z=" << motionData[2] <<
						"a=" << motionData[3] <<
						"b=" << motionData[4] <<
						"c=" << motionData[5] << endl;

			//emit Mouse3dDevice::Move3d(motionData);
			window->OnMove(motionData);
		}*/
    }

	void Mouse3dDevice::translateDummy() {
		int rand = qrand() % 3;
		if (rand == 0)
			qDebug() << "Yea, Yea, I'm a dummy!";
		else if (rand == 1)
			qDebug() << "Pretty mad dummy I am!";
		else if (rand == 2)
			qDebug() << "Also, I'm out of knives!";
	}

	int Mouse3dDevice::initMouse3DLinux() {
		this->display = QX11Info::display();
		Window xwindow = window->winId();

		if ( !MagellanInit( this->display, xwindow ) ) {
			qDebug() << "No driver is running. Exit ... " << endl;
			return -1;
		}
		else {
			qDebug() << "Mouse3dDevice::Mouse3DLinux: MagellanInit() = Success!" << endl;
		}

		QCoreApplication *inst = App::ApplicationX11::instance();
		App::ApplicationX11 *app = qobject_cast<App::ApplicationX11*>(inst);

		QObject::connect( app, SIGNAL( passDummy( )), this, SLOT( translateDummy( )));
		app->initConnection( this );

		return 0;
	}

#elif defined(Q_OS_MAC)
#endif
}
