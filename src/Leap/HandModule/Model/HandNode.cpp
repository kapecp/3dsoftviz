/**
 * Created by Patrik Berger on 13.11.2016.
 */

#include "Leap/HandModule/Model/HandNode.h"

#include <osg/ShapeDrawable>

Leap::HandNode::HandNode() {}

void Leap::HandNode::initStructure() {}

void Leap::HandNode::generateGeometry( float radius, HandColors colorSwitch = HandColors::NONE ) {}

void Leap::HandNode::addToStream( QDataStream *stream )
{
	auto mat = getMatrix();
	( * stream ) << ( float )mat( 0,0 ) << ( float )mat( 0,1 ) << ( float )mat( 0,2 ) << ( float )mat( 0,3 )
				 << ( float )mat( 1,0 ) << ( float )mat( 1,1 ) << ( float )mat( 1,2 ) << ( float )mat( 1,3 )
				 << ( float )mat( 2,0 ) << ( float )mat( 2,1 ) << ( float )mat( 2,2 ) << ( float )mat( 2,3 )
				 << ( float )mat( 3,0 ) << ( float )mat( 3,1 ) << ( float )mat( 3,2 ) << ( float )mat( 3,3 );
}

void Leap::HandNode::setFromStream( QDataStream *stream )
{
	if ( stream != nullptr && !stream->atEnd() ) {

		float mat00, mat01, mat02, mat03,
			  mat10, mat11, mat12, mat13,
			  mat20, mat21, mat22, mat23,
			  mat30, mat31, mat32, mat33;

		( * stream )
				>> mat00 >> mat01 >> mat02 >> mat03
				>> mat10 >> mat11 >> mat12 >> mat13
				>> mat20 >> mat21 >> mat22 >> mat23
				>> mat30 >> mat31 >> mat32 >> mat33;

		this->setMatrix( osg::Matrix( mat00, mat01, mat02, mat03,
									  mat10, mat11, mat12, mat13,
									  mat20, mat21, mat22, mat23,
									  mat30, mat31, mat32, mat33 ) );
	}
	else {
		LOG( INFO ) << "Stream is empty";
	}
}


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
