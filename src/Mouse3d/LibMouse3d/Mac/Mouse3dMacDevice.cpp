#include "Mouse3d/LibMouse3d/Mac/Mouse3dMacDevice.h"

#include <QDebug>
#include <QtGlobal>

#include <easylogging++.h>

namespace Mouse3dMacDeviceGlob {
QOSG::CoreWindow* ptr = nullptr;
}

Mouse3dMacDevice::Mouse3dMacDevice( QOSG::CoreWindow* window ): clientID( 0 )
{
	Q_ASSERT( Mouse3dMacDeviceGlob::ptr != nullptr );
	Mouse3dMacDeviceGlob::ptr = window;

	InstallConnexionHandlers( MouseHandler, 0L, 0L );
	uint32_t signature = kConnexionClientWildcard;
	uint8_t* name = nullptr;
	uint16_t mode = kConnexionClientModeTakeOver;
	uint32_t mask = kConnexionMaskAll;
	this->clientID = RegisterConnexionClient( signature, name, mode, mask );
	LOG( INFO ) << "[OSX] Mouse3dDevice: Registered with ClientID =" << this->clientID;
}

Mouse3dMacDevice::~Mouse3dMacDevice()
{
	UnregisterConnexionClient( this->clientID );
	CleanupConnexionHandlers();
	LOG( INFO ) << "[OSX] Mouse3dDevice: Unregistered";
	Mouse3dMacDeviceGlob::ptr = nullptr;
}

void Mouse3dMacDevice::MouseHandler( unsigned int connection, unsigned int messageType, void* messageArgument )
{
	if ( messageType == kConnexionMsgDeviceState ) {
		ConnexionDeviceState* s;
		s = static_cast<ConnexionDeviceState*>( messageArgument );

		if ( s->command == kConnexionCmdHandleAxis ) {
			std::vector<double> motionData;
			motionData.push_back( s->axis[0] ); // X
			motionData.push_back( -s->axis[1] ); // Y
			motionData.push_back( -s->axis[2] ); // Z

			motionData.push_back( s->axis[3] ); // A
			motionData.push_back( s->axis[4] ); // B
			motionData.push_back( s->axis[5] ); // C

			// Axis X,Y,Z scaling
			motionData[0] /= 20000.0;
			motionData[1] /= -20000.0;
			motionData[2] /= 20000.0;

			// Rotation A,B,C scaling
			motionData[3] /= 7000.0;
			motionData[4] /= 7000.0;
			motionData[5] /= 7000.0;

			qDebug() << "Movement: x =" << motionData[0] <<
					 "y =" << motionData[1] <<
					 "z =" << motionData[2];
			qDebug() << "Rotation: a =" << motionData[3] <<
					 "b =" << motionData[4] <<
					 "c =" << motionData[5] << endl;

			//Mouse3dMacDeviceGlob::ptr->PassMotion( motionData );
			Mouse3dMacDeviceGlob::ptr->OnMove( motionData );
		}
	}
}
