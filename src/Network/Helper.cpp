#include "Network/Helper.h"
#include "Util/ApplicationConfig.h"
#include "Leap/CustomLeapManager.h"

#include <osg/Matrix>
#include <osgText/FadeText>
#include <osg/AutoTransform>
#include <osg/ShapeDrawable>

namespace Network {

osg::Quat Helper::lookAt( osg::Vec3 from, osg::Vec3 to )
{
	osg::Quat quad;
	osg::Matrix matrix;
	matrix.makeLookAt( from, to, osg::Z_AXIS );
	//matrix.get( quad ); // this is obsolete API
	quad = matrix.getRotate();
	return quad.inverse();
}

//osg::PositionAttitudeTransform* Helper::generateAvatar( QString label )
//{

//	osg::Cone* cone = new osg::Cone( osg::Vec3( 0,0,0 ), 4.0f, 6.0f );
//	osg::ShapeDrawable* coneDrawable = new osg::ShapeDrawable( cone );
//	osg::Geode* coneGeode = new osg::Geode();
//	coneGeode->addDrawable( coneDrawable );

//	osg::ref_ptr<osgText::FadeText> labelNode = new osgText::FadeText;
//	osg::Geode* textGeode = new osg::Geode();
//	textGeode->addDrawable( labelNode );

//	osg::PositionAttitudeTransform* PAtransform = new osg::PositionAttitudeTransform();
//	PAtransform->addChild( coneGeode );

//	osg::AutoTransform* autoTransformText = new osg::AutoTransform;
//	autoTransformText->setAutoRotateMode( osg::AutoTransform::ROTATE_TO_SCREEN );
//	autoTransformText->addChild( textGeode );



//	PAtransform->addChild( autoTransformText );

//	QString fontPath = Util::ApplicationConfig::get()->getValue( "Viewer.Labels.Font" );

//	if ( fontPath != NULL && !fontPath.isEmpty() ) {
//		labelNode->setFont( fontPath.toStdString() );
//	}

//	labelNode->setFadeSpeed( 0.03f );

//	labelNode->setText( label.toStdString() );
//	labelNode->setLineSpacing( 0 );
//	labelNode->setAxisAlignment( osgText::Text::XY_PLANE );
//	labelNode->setCharacterSize( 10 );
//	labelNode->setDrawMode( osgText::Text::TEXT );
//	labelNode->setAlignment( osgText::Text::CENTER_BOTTOM_BASE_LINE );
//	labelNode->setPosition( osg::Vec3( 0, 7, -7 ) );
//	labelNode->setColor( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

//	osg::StateSet* stateset = new osg::StateSet;
//	stateset->setMode( GL_NORMALIZE, osg::StateAttribute::ON );
//	PAtransform->setStateSet( stateset );

//	return PAtransform;
//}

}
