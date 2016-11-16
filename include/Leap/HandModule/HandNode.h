/**
 * Created by Patrik Berger on 13.11.2016.
 */
#ifndef HANDNODE_H
#define HANDNODE_H


#include <osg/ref_ptr>
#include <osg/Matrix>
#include <osg/MatrixTransform>
#include <osg/Node>

namespace Leap {

class HandNode : public osg::Node{
public:
    HandNode();
    void generateGeometry(float radius);
    virtual void initStructure();

    osg::ref_ptr<osg::MatrixTransform> matrixTransform;
};
}
#endif // HANDNODE_H
