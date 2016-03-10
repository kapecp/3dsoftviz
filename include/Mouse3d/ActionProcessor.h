#ifndef ACTIONPROCESSOR_H
#define ACTIONPROCESSOR_H

#include "Mouse3d/LibMouse3d/SignalInterface.h"
#include "QOSG/CoreWindow.h"
#include "Core/Core.h"

namespace Mouse3d{

/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief 3dSoftViz class responsible for processing received messages from lib to mainWindow
*/
class ActionProcessor{

public:

    //constructor
    ActionProcessor();

    //destructor
    ~ActionProcessor();

    void AlterScene(int);

private:
     QOSG::ViewerQT* viewer;
};

}
#endif
