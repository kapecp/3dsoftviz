/**
 * Created by Patrik Berger on 19.11.2016.
 */
#include "Leap/HandModule/Model/HandBone.h"
#include <easylogging++.h>
#include <osg/ShapeDrawable>
#include <osg/Geode>

const float CYLINDER_RADIUS = 0.03f;
const float HEIGHT = 0.2f;

Softviz::Leap::HandBone::HandBone( int type, osg::ref_ptr<osg::Group> boneGroup )
{
	this->type = type;
	this->boneGroup = boneGroup;
	this->generateGeometry( CYLINDER_RADIUS );
	this->boneGroup->addChild( static_cast<osg::Node*>( this ) );
}

void Softviz::Leap::HandBone::generateGeometry( float radius, HandColors colorSwitch )
{
	osg::ref_ptr<osg::Geode> handGeode( new osg::Geode );
	osg::ref_ptr<osg::Cylinder> handSphere = new osg::Cylinder( osg::Vec3f( 0.0f,0.0f,0.0f ), radius, HEIGHT );

	osg::ref_ptr<osg::ShapeDrawable> handDrawable( new osg::ShapeDrawable( handSphere.get() ) );
	handGeode->addDrawable( handDrawable.get() );

	this->addChild( handGeode.get() );
}
