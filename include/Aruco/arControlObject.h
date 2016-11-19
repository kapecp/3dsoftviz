#ifndef ARCONTROLOBJECT_H
#define ARCONTROLOBJECT_H

#include <osg/Vec3f>


namespace ArucoModul {

//concrete objects
class ArControlObject : public QObject
{

public:
    ArControlObject( int id, osg::Vec3f position );
    void updatePosition( osg::Vec3f position );

private:
    int id;
    osg::Vec3f position;
    bool focused;

};


//object controller
class ArControlClass : public QObject
{

public:
    ArControlClass();
    void updateObjectPosition( osg::Vec3f position );

private:
    ArucoModul::ArControlObject* controlObject;
};

} // end ArucoModul namespace

#endif // ARCONTROLOBJECT_H
