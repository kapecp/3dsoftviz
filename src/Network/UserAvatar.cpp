#include <Network/UserAvatar.h>

namespace Network {

UserAvatar::UserAvatar( QString label )
{
	this->label = label;
	this->initStructure();
}

void UserAvatar::UpdateHands( QDataStream* stream )
{
	this->leftHand->setFromStream( stream );
	this->rightHand->setFromStream( stream );
}

void UserAvatar::initStructure()
{
	osg::Cone* cone = new osg::Cone( osg::Vec3( 0,0,0 ), 4.0f, 6.0f );
    //cone->setRotation( osg::Quat( double(M_PI/2), osg::Vec3d(1,0,0) ) );
	osg::ShapeDrawable* coneDrawable = new osg::ShapeDrawable( cone );
	osg::Geode* coneGeode = new osg::Geode();
	coneGeode->addDrawable( coneDrawable );

	osg::ref_ptr<osgText::FadeText> labelNode = new osgText::FadeText;
	osg::Geode* textGeode = new osg::Geode();
	textGeode->addDrawable( labelNode );

	this->addChild( coneGeode );

	this->handsGroup = new osg::MatrixTransform();
	this->leftHand = new Leap::HandPalm( 0.1f, this->handsGroup, Leap::HandColors::LEFT );
	this->rightHand = new Leap::HandPalm( 0.1f, this->handsGroup, Leap::HandColors::RIGHT );

	auto mat = this->handsGroup->getMatrix();
	mat.preMultTranslate(osg::Vec3f(0,0,-1));
	mat.preMultRotate(osg::Quat(M_PI/-2, osg::Vec3f(1,0,0)));
	mat.preMultScale(osg::Vec3f(1.5,1.5,1.5));
	this->handsGroup->setMatrix(mat);

	this->addChild( this->handsGroup );

	osg::AutoTransform* autoTransformText = new osg::AutoTransform;
	autoTransformText->setAutoRotateMode( osg::AutoTransform::ROTATE_TO_SCREEN );
	autoTransformText->addChild( textGeode );

	this->addChild( autoTransformText );

	QString fontPath = Util::ApplicationConfig::get()->getValue( "Viewer.Labels.Font" );

	if ( fontPath != NULL && !fontPath.isEmpty() ) {
		labelNode->setFont( fontPath.toStdString() );
	}

	labelNode->setFadeSpeed( 0.03f );

	labelNode->setText( this->label.toStdString() );
	labelNode->setLineSpacing( 0 );
	labelNode->setAxisAlignment( osgText::Text::XY_PLANE );
	labelNode->setCharacterSize( 10 );
	labelNode->setDrawMode( osgText::Text::TEXT );
	labelNode->setAlignment( osgText::Text::CENTER_BOTTOM_BASE_LINE );
	labelNode->setPosition( osg::Vec3( 0, 7, -7 ) );
	labelNode->setColor( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

	osg::StateSet* stateset = new osg::StateSet;
	stateset->setMode( GL_NORMALIZE, osg::StateAttribute::ON );
	this->setStateSet( stateset );
}

}
