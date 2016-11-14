/**
 * Created by Patrik Berger on 13.11.2016.
 */
#ifndef HANDNODE_H
#define HANDNODE_H


#include <osg/ref_ptr>
#include <osg/Matrix>
#include <osg/MatrixTransform>

namespace Leap {

class HandNode{
public:
    HandNode();
    osg::ref_ptr<osg::MatrixTransform> matrixTransform;
    virtual void initStructure();
};
}
#endif // HANDNODE_H
