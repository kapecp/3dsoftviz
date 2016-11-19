#include "Aruco/arControlObject.h"
#include "Util/ApplicationConfig.h"
#include <QDebug>


namespace ArucoModul {


ArControlObject::ArControlObject( int id, osg::Vec3f position )
{
    this->id = id;
    this->position = position;
    this->focused = false;
}

void ArControlObject::updatePosition( osg::Vec3f position ){
    this->position = position;
}




ArControlClass::ArControlClass()
{
    this->controlObject = NULL;
}

void ArControlClass::updateObjectPosition( osg::Vec3f position ){
    if( controlObject == NULL ){
        controlObject = new ArControlObject( 1, position);
    }
    else{
        controlObject->updatePosition( position );
    }

    qDebug() << position.x();
}

} // namespace ArucoModul
