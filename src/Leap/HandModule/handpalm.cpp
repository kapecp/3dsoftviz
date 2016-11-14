/**
 * Created by Patrik Berger on 13.11.2016.
 */
#include <easylogging++.h>
#include "Leap/HandModule/HandPalm.h"


Leap::HandPalm::HandPalm(osg::ref_ptr<osg::MatrixTransform> matrixTransform) {
    this->matrixTransform = matrixTransform;
    this->initStructure();
}

void Leap::HandPalm::initStructure() {
    LOG( INFO ) << "Leap/HandleModule/HandPalm/initStrucure()";
    if (this->coreJoints.empty()) {
        int i = 0;

        // Create core joints
        for(i = 0; i <= 5; i++) {
            Joint* joint = new Joint(0, i);
            // TODO calculate joint's matrixTransform - !create enum / table for consts!
            // joint->matrixTransform...
            this->coreJoints.push_back(joint);
        }

        // Create fingers
        for (Joint* coreJoint : this->coreJoints) {
            coreJoint->initStructure();
        }
    }
}
