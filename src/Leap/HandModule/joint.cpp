/**
 * Created by Patrik Berger on 13.11.2016.
 */
#include "Leap/HandModule/Joint.h"
#include <easylogging++.h>

const float RADIUS = 0.07f;

Leap::Joint::Joint(int level, int position, osg::ref_ptr<osg::Group> fingerJointGroup) {
    fingerJointGroup->addChild(dynamic_cast<osg::Node*>(this));
    this->level = level;
    this->position = position;
    this->nextJoint = NULL;
    this->fingerJointGroup = fingerJointGroup;
    this->generateGeometry(RADIUS);
    initStructure();
    this->fingerJointGroup->addChild(static_cast<osg::Node*> (this));
}

void Leap::Joint::initStructure() {

    if (this->nextJoint == NULL) {
        if (this->level != 4) {
            this->nextJoint = new Joint(this->level + 1, this->position, this->fingerJointGroup);
        }
    }
}
