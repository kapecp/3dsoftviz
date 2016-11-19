/**
 * Created by Patrik Berger on 13.11.2016.
 */
#include "Leap/HandModule/Joint.h"
#include <easylogging++.h>

const float RADIUS = 0.07f;

Leap::Joint::Joint(int level, int position, osg::ref_ptr<osg::Group> fingerJointGroup) {
    this->level = level;
    this->position = position;
    this->nextJoint = NULL;
    this->fingerJointGroup = fingerJointGroup;
    this->generateGeometry(RADIUS);
    this->fingerJointGroup->addChild(static_cast<osg::Node*> (this));
    initStructure();  
}

void Leap::Joint::initStructure() {
    if (this->nextJoint == NULL) {
        if (this->level != 4) {
            this->nextJoint = new Joint(this->level + 1, this->position, this->fingerJointGroup);
        }
    }
}

void Leap::Joint::generateGeometry(float radius) {
    osg::ref_ptr<osg::Geode> handGeode( new osg::Geode );
    osg::ref_ptr<osg::Sphere> handSphere = new osg::Sphere( osg::Vec3f( 0.0f,0.0f,0.0f ), radius );

    osg::ref_ptr<osg::ShapeDrawable> handDrawable( new osg::ShapeDrawable( handSphere.get() ) );
    handGeode->addDrawable( handDrawable.get() );

    this->addChild( handGeode.get() );
}
