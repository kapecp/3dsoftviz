#include "City/Ball.h"
#include <osg/ShapeDrawable>
#include <Util/ApplicationConfig.h>
#include <Data/OsgNode.h>
#include <Manager/ResourceManager.h>
#include <osgText/FadeText>

namespace City {
static float DEFAULT_BALL_BASE_SIZE = 1.0f;
static float DEFAULT_BALL_CAPTION_CHARACTER_SIZE = 3.0f;
static float DEFAULT_BALL_CAPTION_OFFSET = 0.3f;

Ball::Ball( const QString& name )
{
	const float BALL_BASE_SIZE = Util::ApplicationConfig::get()->getFloatValue( "City.Ball.DefaultBaseSize", DEFAULT_BALL_BASE_SIZE );

	setLabel( name );
	this->lieOnGround = true;
	this->labelVisible = false;
	baseSize = minBaseSize = BALL_BASE_SIZE;
}

void Ball::setBaseSize( float size )
{
	baseSize = size;
}

float Ball::getBaseSize() const
{
	return baseSize;
}

void Ball::setLieOnGround( bool state )
{
	lieOnGround = state;
}

void Ball::setLabel( const QString& name )
{
	if ( label.valid() && labelVisible ) {
		removeChild( label );
	}

	this->name = name;
	label.release();
	if ( !name.isEmpty() ) {
		auto config = Util::ApplicationConfig::get();
		const float BALL_CAPTION_CHARACTER_SIZE = config->getFloatValue( "City.Ball.CaptionCharacterSize", DEFAULT_BALL_CAPTION_CHARACTER_SIZE );

		label = new osg::Geode();
		auto text = new osgText::FadeText();
		text->setText( name.toStdString() );
		text->setCharacterSize( BALL_CAPTION_CHARACTER_SIZE );
		text->setAutoRotateToScreen( true );
		label->addDrawable( text );
		if ( labelVisible ) {
			addChild( label );
		}
	}
}

void Ball::showLabel( bool state )
{
	if ( label.valid() ) {
		if ( labelVisible && !state ) {
			removeChild( label );
		}
		else if ( !labelVisible && state ) {
			addChild( label );
		}
	}
	labelVisible = state;
}


osg::BoundingBox Ball::getBoundingBox() const
{
	const float BALL_BASE_SIZE_HALF = getBaseSize() / 2;
	return osg::BoundingBox( -BALL_BASE_SIZE_HALF, -BALL_BASE_SIZE_HALF, 0, BALL_BASE_SIZE_HALF, BALL_BASE_SIZE_HALF, getBaseSize() );
}

void Ball::refresh()
{
	// nacitanie konfiguracii z aplikacie
	auto config = Util::ApplicationConfig::get();
	const float BALL_CAPTION_OFFSET = config->getFloatValue( "City.Ball.CaptionOffset", DEFAULT_BALL_CAPTION_OFFSET );

	// vymazanie starej geometrie
	removeChildren( 0, getNumChildren() );

	auto ball = new osg::PositionAttitudeTransform();

	osg::Sphere* sphere = new osg::Sphere;
	sphere->setRadius( getBaseSize() );

	osg::ref_ptr<osg::ShapeDrawable> nodeSphere = new osg::ShapeDrawable;
	nodeSphere->setShape( sphere );

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( nodeSphere );

	ball->addChild( geode );
	addChild( ball );

	if ( label.valid() ) { // ma nastaveny label
		static_cast<osgText::TextBase*>( label->asGeode()->getDrawable( 0 ) )->setPosition( osg::Vec3( 0.0f, 0.0f, getBaseSize() + BALL_CAPTION_OFFSET ) );
		if ( labelVisible ) {
			addChild( label );
		}
	}
}

void Ball::select( bool selected )
{
	if ( selected && !lastMaterial.valid() ) {
		lastMaterial = static_cast<osg::Material*>( getOrCreateStateSet()->getAttribute( osg::StateAttribute::Type::MATERIAL ) );
		auto color = lastMaterial->getDiffuse( osg::Material::Face::FRONT_AND_BACK );
		getOrCreateStateSet()->setAttribute( Manager::ResourceManager::getInstance()->getMaterial( osg::Vec3( color.r() + 0.4f, color.g() + 0.4f, color.b() + 0.4f ) ), osg::StateAttribute::ON );
	}
	else {
		if ( lastMaterial.valid() ) {
			getOrCreateStateSet()->setAttribute( lastMaterial );
		}
		lastMaterial.release();
	}
}

float Ball::getMinBaseSize() const
{
	return minBaseSize;
}
}
