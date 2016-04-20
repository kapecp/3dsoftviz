#include "Mouse3d/LibMouse3d/Mouse3d.h"

#include <QtGlobal>
#include <QDebug>

#include "QOSG/CoreWindow.h"

namespace LibMouse3d{

    Mouse3dDevice::Mouse3dDevice(QOSG::CoreWindow* window){
#if defined(Q_OS_WIN)

        this->mouse = new Mouse3DInput(window);
        QObject::connect(mouse, SIGNAL(Move3d(std::vector<float>&)), window, SLOT(OnMove(std::vector<float>&)));

#elif defined(Q_OS_LINUX)

        Mouse3DLinux(window);

#elif defined(Q_OS_MAC)

		// Mac call mouse

#endif
    }

    Mouse3dDevice::~Mouse3dDevice(){

    }

#if defined(Q_OS_WIN)

#elif defined(Q_OS_LINUX)
	/*!
		Called with the processed motion data when a 3D mouse event is received

		The default implementation emits a Move3d signal with the motion data
	*/
    void Mouse3dDevice::SendSignal(std::vector<float>& motionData)
    {
       // emit Mouse3dDevice::Move3d(motionData);
    }

	void Mouse3dDevice::Mouse3DLinux(QOSG::CoreWindow* window){

		//XInitThreads();

        Display *display;
        Window xwindow = window->winId();
        std::vector<float> signal_data;

        XEvent report;
        MagellanFloatEvent MagellanEvent;

		bool MagellanLoop = true;

		display = QX11Info::display();


		/************************* Create 3D Event Types ***************************/
		if ( !MagellanInit( display, xwindow ) ) {
			qDebug() << "No driver is running. Exit ... " << endl;
			exit(EXIT_FAILURE);
		}
		else {
			qDebug() << "Mouse3dDevice::Mouse3DLinux: MagellanInit() = Success!" << endl;
		}
		qDebug() << "Mouse3dDevice::Mouse3DLinux: winId() = " << xwindow << endl;


		/************************* Main Loop ***************************************/
		//XSelectInput( display, xwindow, KeyPressMask | KeyReleaseMask );

		while( MagellanLoop ) {
			//if(XCheckWindowEvent(display, window, KeyPressMask | KeyReleaseMask,  &report ))
			if( ! window->x11Event( &report ) ) {
				//XNextEvent( display, &report );
				qDebug() << "EventType = " << report.type << endl;
				if ( report.type == ClientMessage ) {
					if ( MagellanTranslateEvent( display, &report, &MagellanEvent, 1.0, 1.0 ) == MagellanInputMotionEvent) {
						MagellanRemoveMotionEvents( display );

						signal_data.clear();
						signal_data.push_back(MagellanEvent.MagellanData[ MagellanX ]);
						signal_data.push_back(MagellanEvent.MagellanData[ MagellanY ]);
						signal_data.push_back(MagellanEvent.MagellanData[ MagellanZ ]);
						signal_data.push_back(MagellanEvent.MagellanData[ MagellanA ]);
						signal_data.push_back(MagellanEvent.MagellanData[ MagellanB ]);
						signal_data.push_back(MagellanEvent.MagellanData[ MagellanC ]);

						qDebug() <<  "x=" << signal_data[0] <<
									"y=" << signal_data[1] <<
									"z=" << signal_data[2] <<
									"a=" << signal_data[3] <<
									"b=" << signal_data[4] <<
									"c=" << signal_data[5] << endl;

						//emit Mouse3dDevice::Move3d(signal_data);
						//window->OnMove(signal_data);
						//sleep(500);
					}
					else {
						qDebug() << "Mouse3dDevice::Mouse3DLinux: Unrecognized ClientMessage Event - Not MagellanInputMotionEvent" << endl;
					}
				}
			}
		}
   }
#elif defined(Q_OS_MAC)

	// Mac custom methods here

#endif
}
