#include "Mouse3d/ActionProcessor.h"
#include "Mouse3d/LibMouse3d/SignalInterface.h"
#include "QOSG/ViewerQT.h"

QOSG::ViewerQT* viewer;
Mouse3d::ActionProcessor::ActionProcessor(){
viewer = AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
}

Mouse3d::ActionProcessor::~ActionProcessor(){

}
/* @author Jaroslav Gazdik
 * @date 1.12.2015
 * @brief Called to map filtered actions to keyboard buttons
 */
void Mouse3d::ActionProcessor::AlterScene(int action){

    switch (action){
    // action 1 = push left arrow
    case 1:
        viewer->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Left );
        viewer->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Left );
        break;
    // action 2 = push up arrow
    case 2:
        viewer->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Up );
        viewer->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Up );
        break;
    // action 3 = push down arrow
    case 3:
        viewer->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Down );
        viewer->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Down );
        break;
    // action 4 = push right arrow
    case 4:
        viewer->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Right );
        viewer->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Right );
        break;
    // action 5 = push pgup
    case 5:
        viewer->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Up );
        viewer->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Up );
        break;
    // action 6 = push pgdown
    case 6:
        viewer->getEventQueue()->keyPress( osgGA::GUIEventAdapter::KEY_Page_Down );
        viewer->getEventQueue()->keyRelease( osgGA::GUIEventAdapter::KEY_Page_Down );
        break;
    case 7: //TODO rotacia
        break;
    }

}
