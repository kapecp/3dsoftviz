/**
 * Created by Patrik Berger on 13.11.2016.
 */
#include "Leap/HandModule/Model/Joint.h"

#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Shape>

#include <easylogging++.h>

const float RADIUS = 0.07f;

Softviz::Leap::Joint::Joint( int level, int position, osg::ref_ptr<osg::Group> fingerJointGroup, HandColors colorSwitch )
	:level( level ), position( position ), colorSwitch( colorSwitch ), nextJoint( nullptr ), fingerJointGroup( fingerJointGroup )
{
	this->generateGeometry( RADIUS, colorSwitch );
	this->fingerJointGroup->addChild( static_cast<osg::Node*>( this ) );
	initStructure();
}

void Softviz::Leap::Joint::initStructure()
{
	if ( this->nextJoint == nullptr ) {
		if ( this->level != 4 ) {
			this->nextJoint = new Joint( this->level + 1, this->position, this->fingerJointGroup, this->colorSwitch );
			//LOG( INFO ) <<"Leap/Joint/initStructure() level" + std::to_string( level );
		}
	}
}

void Softviz::Leap::Joint::generateGeometry( float radius, HandColors colorSwitch )
{
	osg::ref_ptr<osg::Geode> handGeode( new osg::Geode );
	osg::ref_ptr<osg::Sphere> handSphere = new osg::Sphere( osg::Vec3f( 0.0f,0.0f,0.0f ), radius );

	osg::ref_ptr<osg::ShapeDrawable> handDrawable( new osg::ShapeDrawable( handSphere.get() ) );
	this->setColor( colorSwitch, handDrawable );
	handGeode->addDrawable( handDrawable.get() );

	this->addChild( handGeode.get() );
}
