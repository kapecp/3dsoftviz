/**
 * Created by Patrik Berger on 13.11.2016.
 */
#ifndef JOINT_H
#define JOINT_H

#include "Leap/HandModule/HandNode.h"

class Joint : HandNode {
public:
    Joint(int level, int position);
    void initStructure();
    Joint* nextJoint;
private:
    // 0-3, vyjadruje kolky klb prsta objekt predstavuje
    int level;
    // 0-5, vyjadruje, v ktorom prste dany klb je
    int position;
};
#endif // JOINT_H
