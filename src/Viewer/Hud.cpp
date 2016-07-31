
#include "Viewer/Hud.h"
#include <osg/MatrixTransform>
#include <Util/ApplicationConfig.h>

namespace Vwr {
static const int DEFAULT_HUD_WIDTH = 300;
static const int DEFAULT_HUD_HEIGHT = 300;

Hud::Hud()
{
	modelview = new osg::MatrixTransform();
	modelview->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
	addChild( modelview );

	text = new osgText::Text();
	text->setCharacterSize( 16 );
	text->setAxisAlignment( osgText::Text::SCREEN );
	text->setColor( osg::Vec4( 1, 1, 1, 1.0f ) );

	background = new osg::Geometry();

	osg::StateSet* hudStateSet = new osg::StateSet();
	hudStateSet->setRenderBinDetails( 999999, "DepthSortedBin" );
	hudStateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
	hudStateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
	hudStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

	geode = new osg::Geode();
	geode->addDrawable( text );
	geode->addDrawable( background );
	geode->setStateSet( hudStateSet );

	modelview->addChild( geode );

	const int HUD_WIDTH = Util::ApplicationConfig::get()->getIntValue( "Viewer.HUD.Width", DEFAULT_HUD_WIDTH );
	const int HUD_HEIGHT = Util::ApplicationConfig::get()->getIntValue( "Viewer.HUD.Height", DEFAULT_HUD_HEIGHT );
	setSize( QSize( HUD_WIDTH, HUD_HEIGHT ) );

	refresh();
}

void Hud::setText( const QString& text )
{
	if ( !text.isEmpty() ) {
		this->text->setPosition( osg::Vec3( 10.0f, ( float )size.height() - 20.0f, 1.0f ) );
	}
	this->text->setText( text.toStdString() );
}

void Hud::setSize( const QSize& size )
{
	this->size = size;
	text->setPosition( osg::Vec3( 10.0f, ( float )size.height() - 20.0f, 1.0f ) );
}

void Hud::setWindowSize( const QSize& size )
{
	this->setMatrix( osg::Matrix::ortho2D( 0, size.width(), 0, size.height() ) );
	modelview->setMatrix( osg::Matrix::translate( size.width() - this->size.width(), size.height() - this->size.height(), 0 ) );
}

void Hud::refresh()
{
	geode->removeDrawable( background );

	background = new osg::Geometry();

	osg::Vec3Array* hudVertices = new osg::Vec3Array;
	hudVertices->push_back( osg::Vec3( 0.0f, 0.0f, 0.8f ) );
	hudVertices->push_back( osg::Vec3( ( float )size.width(), 0.0f, 0.8f ) );
	hudVertices->push_back( osg::Vec3( ( float )size.width(), ( float )size.height(), 0.8f ) );
	hudVertices->push_back( osg::Vec3( 0.0f, ( float )size.height(), 0.8f ) );

	osg::Vec4Array* hudColors = new osg::Vec4Array;
	hudColors->push_back( osg::Vec4( 0.8f, 0.8f, 0.8f, 0.5f ) );

	osg::DrawElementsUInt* hudIndices = new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
	hudIndices->push_back( 0 );
	hudIndices->push_back( 1 );
	hudIndices->push_back( 2 );
	hudIndices->push_back( 3 );

	background->addPrimitiveSet( hudIndices );
	background->setVertexArray( hudVertices );
	background->setColorArray( hudColors );
	background->setColorBinding( osg::Geometry::BIND_OVERALL );

	geode->addDrawable( background );
}
}
