#pragma once

#include <3dConnexionClient/ConnexionClientAPI.h>

#include "QOSG/CoreWindow.h"

class Mouse3dMacDevice 
{
public:
	Mouse3dMacDevice::Mouse3dMacDevice( QOSG::CoreWindow *window );
	Mouse3dMacDevice::~Mouse3dMacDevice();

	void ProcessEvent(const ConnexionDeviceState *s);

	unsigned char *CStringToPascalString(const char *s);

	void MouseHandler(io_connect_t connection, natural_t messageType, void *messageArgument);
		
private:
	QOSG::CoreWindow *window;
	
	UInt16 ClientID;
	std::map<UInt16, Mouse3dMacDevice *, LessThanClientID> ClientIDToDeviceObject;
};