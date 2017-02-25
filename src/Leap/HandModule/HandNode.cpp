/**
 * Created by Patrik Berger on 13.11.2016.
 */
#include <easylogging++.h>
#include "Leap/HandModule/HandNode.h"
#include <osg/ShapeDrawable>

Leap::HandNode::HandNode() {}

void Leap::HandNode::initStructure() {}

void Leap::HandNode::generateGeometry( float radius, int colorSwitch ) {}


void Leap::HandNode::setColor( int colorSwitch, osg::ref_ptr<osg::ShapeDrawable> handDrawable )
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
		case 1:
			handDrawable.get()->setColor( green );
			break;
		case 2:
			handDrawable.get()->setColor( blue );
			break;
		default:
			break;
	}
}
