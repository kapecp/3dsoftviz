/**
 * Created by Patrik Berger on 13.11.2016.
 */
#ifndef JOINT_H
#define JOINT_H

#include "Leap/HandModule/HandNode.h"
namespace Leap {
class Joint : public HandNode {
public:
    Joint(int level, int position, osg::ref_ptr<osg::Group> fingerJointGroup);
    void initStructure();
    void generateGeometry(float radius);

    Joint* nextJoint;
    osg::ref_ptr<osg::Group> fingerJointGroup;
private:
    // 0-3, vyjadruje kolky klb prsta objekt predstavuje
    int level;
    // 0-5, vyjadruje, v ktorom prste dany klb je
    int position;
};
}
#endif // JOINT_H
