#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "Mouse3d/LibMouse3d/ConnectionInterface.h"
#include "QOSG/CoreWindow.h"
namespace Mouse3d{

/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief 3dSoftViz class responsible for establishing connection with LibMouse3d.
*/
class Connector{

public:
    QOSG::CoreWindow* win;

    //constructor
    Connector(QOSG::CoreWindow* win);

    //destructor
    ~Connector();

    void ConnectMouse3d();

private:

};

}
#endif
