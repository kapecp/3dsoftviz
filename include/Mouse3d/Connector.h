#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <QDebug>
#include <QThread>

#include "QOSG/CoreWindow.h"

#include "Mouse3d/LibMouse3d/EventThread.h"

namespace Mouse3d{

/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief 3dSoftViz class responsible for establishing connection with LibMouse3d.
*/


class Connector{

public:
    //constructor
    Connector(QOSG::CoreWindow* window);

    //destructor
    ~Connector();

    void CreateConnection();

private:
	QOSG::CoreWindow* window;
	LibMouse3d::EventThread* thread;
};

}
#endif
