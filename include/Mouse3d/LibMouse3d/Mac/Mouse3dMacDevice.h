#pragma once

#include <QObject>

#include <3dConnexionClient/ConnexionClientAPI.h>

#include "QOSG/CoreWindow.h"

class Mouse3dMacDevice : public QObject
{
	Q_OBJECT

public:
	Mouse3dMacDevice( QOSG::CoreWindow* window );

	~Mouse3dMacDevice();

	static void MouseHandler( unsigned int connection, unsigned int messageType, void* messageArgument );

	/*public slots:
		void PassMotion( std::vector<float> &motion_vector );
	*/

private:
	uint16_t clientID;
};
