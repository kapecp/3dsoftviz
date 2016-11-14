/**
 * Created by Patrik Berger on 13.11.2016.
 */
#include "Leap/HandModule/Joint.h"
#include <easylogging++.h>

Leap::Joint::Joint(int level, int position){
    this->level = level;
    this->position = position;
    this->nextJoint = NULL;
    initStructure();
}

void Leap::Joint::initStructure() {

    if (this->nextJoint == NULL) {
        if (this->position != 0 && this->level != 3) {
//            LOG( INFO ) << "Leap/HandleModule/Joint/initStrucure()   level: " + std::to_string(this->level) + " position: " + std::to_string(position);
            this->nextJoint = new Joint(this->level + 1, this->position);
            // TODO calculate joint's matrixTransform - !create enum / table for consts!
            // nextJoint->matrixTransform...
            this->nextJoint->initStructure();
        }
    }
}
