/**
 * Created by Patrik Berger on 13.11.2016.
 */
#include <easylogging++.h>
#include "Leap/HandModule/HandNode.h"

const osg::Vec4f BLUE = osg::Vec4f( 21.0f/255,51.0f/255,252.0/255,1.0f );
const osg::Vec4f GREEN = osg::Vec4f( 23.0f/255,190.0f/255,40.0/255,1.0f );

Leap::HandNode::HandNode() {}

void Leap::HandNode::initStructure() {}

void Leap::HandNode::generateGeometry( float radius, int colorSwitch ) {}


void Leap::HandNode::setColor( int colorSwitch, osg::ref_ptr<osg::ShapeDrawable> handDrawable )
{
	// setting color
	switch ( colorSwitch ) {
		case 1:
			handDrawable.get()->setColor( GREEN );
			break;
		case 2:
			handDrawable.get()->setColor( BLUE );
			break;
		default:
			break;
	}
}
