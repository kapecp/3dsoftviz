#include <Network/UserAvatar.h>
#include <Core/Core.h>

namespace Network {

UserAvatar::UserAvatar( QString label )
{
	this->label = label;

	this->initStructure();
}

void UserAvatar::UpdateHands( Leap::HandPalm* leftHand, Leap::HandPalm* rightHand )
{
	this->handManager->updateHands( leftHand, rightHand );
}

void UserAvatar::initStructure()
{
	osg::Cone* cone = new osg::Cone( osg::Vec3( 0,0,0 ), 4.0f, 6.0f );
	osg::ShapeDrawable* coneDrawable = new osg::ShapeDrawable( cone );
	osg::Geode* coneGeode = new osg::Geode();
	coneGeode->addDrawable( coneDrawable );

	osg::ref_ptr<osgText::FadeText> labelNode = new osgText::FadeText;
	osg::Geode* textGeode = new osg::Geode();
	textGeode->addDrawable( labelNode );

	this->addChild( coneGeode );

	auto core = AppCore::Core::getInstance();
	if ( core != NULL ) {
		this->handsGroup = new osg::Group();
		this->handManager = new Leap::CustomLeapManager( core->getCoreWindow()->getCameraManipulator(),
				core->getLayoutThread(),
				core->getCoreGraph(),
				this->handsGroup.get() );
	}

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
