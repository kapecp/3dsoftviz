/**
 * Created by Patrik Berger on 13.11.2016.
 */

#include "Leap/HandModule/Model/HandNode.h"

#include <osg/ShapeDrawable>

Leap::HandNode::HandNode() {}

void Leap::HandNode::initStructure() {}

void Leap::HandNode::generateGeometry( float radius, HandColors colorSwitch = HandColors::NONE ) {}


void Leap::HandNode::setColor( HandColors colorSwitch, osg::ref_ptr<osg::ShapeDrawable> handDrawable )
{
	osg::Vec4f blue = osg::Vec4f( 21.0f/255.0f,
								  51.0f/255.0f,
								  252.0f/255.0f,
								  1.0f ) ;
	osg::Vec4f green = osg::Vec4f( 23.0f/255.0f,
								   190.0f/255.0f,
								   40.0f/255.0f,
								   1.0f ) ;

	// setting color
	switch ( colorSwitch ) {
		case HandColors::RIGHT:
			handDrawable.get()->setColor( green );
			break;
		case HandColors::LEFT:
			handDrawable.get()->setColor( blue );
			break;
		default:
			break;
	}
}
