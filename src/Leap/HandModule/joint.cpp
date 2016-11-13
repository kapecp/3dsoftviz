/**
 * Created by Patrik Berger on 13.11.2016.
 */
#include "Leap/HandModule/Joint.h"


void Joint::initStructure() {
    if (this->nextJoint == NULL) {
        if (this->position != 0 && this->level != 3) {
            this->nextJoint = new Joint(this->level + 1, this->position);
            // TODO calculate joint's matrixTransform - !create enum / table for consts!
            // nextJoint->matrixTransform...
            this->nextJoint->initStructure();
        }
    }
}
