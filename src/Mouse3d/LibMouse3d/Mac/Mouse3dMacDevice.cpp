#include "Mouse3d/LibMouse3d/Mac/Mouse3dMacDevice.h"

#include <QDebug>

Mouse3dMacDevice::Mouse3dMacDevice(): clientID(0) {
	InstallConnexionHandlers( MouseHandler, 0L, 0L );
	this->clientID = RegisterConnexionClient(kConnexionClientWildcard, '\0', kConnexionClientModeTakeOver, kConnexionMaskAll);
	qDebug() << "Mouse3dDevice: Registered with ClientID=" << this->clientID;
}

Mouse3dMacDevice::~Mouse3dMacDevice() {
	UnregisterConnexionClient(this->clientID);
	CleanupConnexionHandlers();
	qDebug() << "Mouse3dDevice: Unregistered";
}

void MouseHandler(io_connect_t connection, natural_t messageType, void *messageArgument) {
	if (mesageType == kConnexionMsgDeviceState) {
		ConnexionDeviceState *s;
		s = static_cast<ConnexionDeviceState *>(messageArgument);
		
		if (s->command == kConnexionCmdHandleAxis) {
			std::vector<float> motionData;
			motionData.push_back(s->axis[0]); // X
			motionData.push_back(-s->axis[1]); // Y
			motionData.push_back(-s->axis[2]); // Z
			
			motionData.push_back(s->axis[3]); // A
			motionData.push_back(s->axis[4]); // B
			motionData.push_back(s->axis[5]); // C
			
			qDebug() <<  "Mouse3dDevice: x=" << motionData[0] <<
						"y=" << motionData[1] <<
						"z=" << motionData[2] <<
						"a=" << motionData[3] <<
						"b=" << motionData[4] <<
						"c=" << motionData[5];
						
			PassMotion( motionData );
		}
	}
}