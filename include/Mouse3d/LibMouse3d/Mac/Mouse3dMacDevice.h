#pragma once

#include <QObject>

#include <3dConnexionClient/ConnexionClientAPI.h>

class Mouse3dMacDevice : public QObject {
	Q_OBJECT
	
public:
	Mouse3dMacDevice::Mouse3dMacDevice();
	
	Mouse3dMacDevice::~Mouse3dMacDevice();

	void MouseHandler(io_connect_t connection, natural_t messageType, void *messageArgument);
	
public slots:
	void PassMotion( std::vector<float> &motion_vector );
		
private:
	UInt16 clientID;
};