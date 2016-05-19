#include "Mouse3dMacDevice.h"

#include <map>
#include <cstring>

#include <QDebug>

class LessThanClientID
{
public:
	bool operator()(const UInt16 &id1, const UInt16 &id2) const {
		return id1<id2;
	}
};

Mouse3dMacDevice::Mouse3dMacDevice( QOSG::CoreWindow *window ):
	window(window), ClientID(0) {

	OSErr result = InstallConnexionHandlers( MouseHandler, 0L, 0L );

	this->ClientID = RegisterConnexionClient(kConnexionClientWildcard, '\0', kConnexionClientModeTakeOver, kConnexionMaskAll);
	qDebug() << "Mouse3dDevice: Registered with ClientID=" << this->ClientID;

	ClientIDToDeviceObject.insert(std::pair<const UInt16, Mouse3dMacDevice *>(this->ClientID,this));
}

Mouse3dMacDevice::~Mouse3dMacDevice() {
	qDebug() << "Mouse3dDevice: Closing";
	UnregisterConnexionClient(this->ClientID);

	std::map<UInt16,Mouse3dMacDevice *,LessThanClientID>::iterator it=ClientIDToDeviceObject.find(this->ClientID);

	if(it==ClientIDToDeviceObject.end()) {
		qDebug() << "Mouse3dDevice: No matching Mouse3dMacDevice object for clientID=" << this->ClientID;
    }
	else {
		ClientIDToDeviceObject.erase(it);
    }

	if(ClientIDToDeviceObject.empty()) {
		CleanupConnexionHandlers();
	}

	this->ClientID=0;
	this->Initialized=false;
}

void Mouse3dMacDevice::ProcessEvent(const ConnexionDeviceState *s) {
	std::vector<float> motionData;

	if (s->command == kConnexionCmdHandleAxis) {
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
					
		window->OnMove(motionData);
	}
}

unsigned char *Mouse3dMacDevice::CStringToPascalString(const char *s) {
	size_t l = strlen(s);
	unsigned char *result = new unsigned char[l+2];

	result[0]=static_cast<unsigned char>(l);
	result[l+1]=0;
	size_t i=0;
	while( i<l ) {
		result[i+1]=static_cast<unsigned char>(s[i]);
		++i;
	}
	return result;
}

void MouseHandler(io_connect_t connection, natural_t messageType, void *messageArgument) {
	ConnexionDeviceState *s;
	std::map<UInt16, Mouse3dMacDevice *, LessThanClientID>::iterator it;
	Mouse3dMacDevice *device;

	if (mesageType == kConnexionMsgDeviceState){
		s = static_cast<ConnexionDeviceState *>(messageArgument);
		it = ClientIDToDeviceObject.find(s->client);

		if (it == ClientIDToDeviceObject.end()) {
			return;
		}

		device=(*it).second;
		device->ProcessEvent(s);
		break;
	}
}