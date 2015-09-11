#include "Data/OsgNode.h"
#include "Data/Node.h"
#include "Data/Graph.h"
#include "Data/Cluster.h"

#include "Util/ApplicationConfig.h"

#include <osg/Geometry>
#include <osg/Depth>
#include <osg/CullFace>
#include <osgText/FadeText>

#include <QTextStream>

#include "Data/OsgNode.h"

Data::OsgNode::OsgNode( qlonglong id, QString name, Data::Type* type, Data::Graph* graph, float scaling, osg::Vec3f position )
	:DbNode( id,name,type,graph,scaling )
{
	this->mTargetPosition = position;
	this->currentPosition = position * Util::ApplicationConfig::get()->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();

	this->setBall( NULL );
	this->setParentBall( NULL );

	this->square = createNode( this->scale * 4, OsgNode::createStateSet( this->type ) );
	this->focusedSquare = createNode( this->scale * 16, OsgNode::createStateSet( this->type ) );
//    this->label = createLabel( this->type->getScale(), labelText );
	this->force = osg::Vec3f();
	this->velocity = osg::Vec3f( 0,0,0 );
	this->selected = false;

	this->usingInterpolation = true;

	//nastavenie farebneho typu
	float r = type->getSettings()->value( "color.R" ).toFloat();
	float g = type->getSettings()->value( "color.G" ).toFloat();
	float b = type->getSettings()->value( "color.B" ).toFloat();
	float a = type->getSettings()->value( "color.A" ).toFloat();

	this->colorOfNode=osg::Vec4( r, g, b, a );
	this->setColor( colorOfNode );

}

Data::OsgNode::~OsgNode( void )
{
}
/*
osg::ref_ptr<osg::Drawable> Data::OsgNode::createLabel( const float& scale, QString name )
{
    //vytvorenie popisu uzla
    osg::ref_ptr<osgText::FadeText> label = new osgText::FadeText;
    label->setFadeSpeed( 0.03f );

    QString fontPath = Util::ApplicationConfig::get()->getValue( "Viewer.Labels.Font" );

    // experimental value
    float newScale = 1.375f * scale;

    if ( fontPath != NULL && !fontPath.isEmpty() ) {
        label->setFont( fontPath.toStdString() );
    }

    label->setText( name.toStdString() );
    label->setLineSpacing( 0 );
    label->setAxisAlignment( osgText::Text::SCREEN );
    label->setCharacterSize( newScale );
    label->setDrawMode( osgText::Text::TEXT );
    label->setAlignment( osgText::Text::CENTER_BOTTOM_BASE_LINE );
    label->setPosition( osg::Vec3( 0, newScale, 0 ) );
    label->setColor( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

    return label;
}*/

osg::ref_ptr<osg::Drawable> Data::OsgNode::createNode( const float& scaling, osg::StateSet* bbState )
{
	//vytvorenie uzla, scaling urcuje jeho velkost
	float width = scaling;
	float height = scaling;

	osg::ref_ptr<osg::Geometry> nodeQuad = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> nodeVerts = new osg::Vec3Array( 4 );

	//velkost uzla
	( *nodeVerts )[0] = osg::Vec3( -width / 2.0f, -height / 2.0f, 0 );
	( *nodeVerts )[1] = osg::Vec3( width / 2.0f, -height / 2.0f, 0 );
	( *nodeVerts )[2] = osg::Vec3( width / 2.0f,	height / 2.0f, 0 );
	( *nodeVerts )[3] = osg::Vec3( -width / 2.0f,  height / 2.0f, 0 );

	nodeQuad->setUseDisplayList( false );

	nodeQuad->setVertexArray( nodeVerts );
	nodeQuad->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS,0,4 ) );

	osg::ref_ptr<osg::Vec2Array> nodeTexCoords = new osg::Vec2Array( 4 );
	//umiestnenie popisu uzla
	( *nodeTexCoords )[0].set( 0.0f,0.0f );
	( *nodeTexCoords )[1].set( 1.0f,0.0f );
	( *nodeTexCoords )[2].set( 1.0f,1.0f );
	( *nodeTexCoords )[3].set( 0.0f,1.0f );

	nodeQuad->setTexCoordArray( 0, nodeTexCoords );

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

	osg::ref_ptr<ColorIndexArray> colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 1>;
	colorIndexArray->push_back( 0 );

	nodeQuad->setColorArray( colorArray );
#ifdef BIND_PER_PRIMITIVE
	nodeQuad->setColorIndices( colorIndexArray );
#endif
	nodeQuad->setColorBinding( osg::Geometry::BIND_OVERALL );
	nodeQuad->setStateSet( bbState );

	return nodeQuad;
}

osg::ref_ptr<osg::Drawable> Data::OsgNode::createSquare( const float& scale, osg::StateSet* bbState )
{
	//vytvorenie textury uzla
	float width = 2.0f;
	float height = 2.0f;

	width *= scale;
	height *= scale;

	osg::ref_ptr<osg::Geometry> nodeRect = new osg::Geometry;
	osg::ref_ptr<osg::Vec3Array> nodeVerts = new osg::Vec3Array( 5 );

	( *nodeVerts )[0] = osg::Vec3( -width / 2.0f, -height / 2.0f, 0 );
	( *nodeVerts )[1] = osg::Vec3( width / 2.0f, -height / 2.0f, 0 );
	( *nodeVerts )[2] = osg::Vec3( width / 2.0f,	height / 2.0f, 0 );
	( *nodeVerts )[3] = osg::Vec3( -width / 2.0f,  height / 2.0f, 0 );
	( *nodeVerts )[4] = osg::Vec3( -width / 2.0f, -height / 2.0f, 0 );

	nodeRect->setVertexArray( nodeVerts );
	nodeRect->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINE_STRIP,0,5 ) );

	osg::ref_ptr<ColorIndexArray> colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 1>;
	colorIndexArray->push_back( 0 );

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back( osg::Vec4( 1.0f, 0.0f, 0.0f, 0.5f ) );

	nodeRect->setColorArray( colorArray );
#ifdef BIND_PER_PRIMITIVE
	nodeRect->setColorIndices( colorIndexArray );
#endif
	nodeRect->setColorArray( colorArray );
	nodeRect->setColorBinding( osg::Geometry::BIND_OVERALL );

	nodeRect->setStateSet( bbState );

	return nodeRect;
}

osg::ref_ptr<osg::StateSet> Data::OsgNode::createStateSet( Data::Type* type )
{
	osg::ref_ptr<osg::StateSet> stateSet = new osg::StateSet;

	stateSet->setDataVariance( osg::Object::DYNAMIC );
	stateSet->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

	if ( type != 0 ) {
		stateSet->setTextureAttributeAndModes( 0, type->getTypeTexture(), osg::StateAttribute::ON );
	}

	stateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
	stateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );

	stateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask( false );
	stateSet->setAttributeAndModes( depth, osg::StateAttribute::ON );

	osg::ref_ptr<osg::CullFace> cull = new osg::CullFace();
	cull->setMode( osg::CullFace::BACK );
	stateSet->setAttributeAndModes( cull, osg::StateAttribute::ON );

	return stateSet;
}

osg::Vec3f Data::OsgNode::getCurrentPosition( bool calculateNew, float interpolationSpeed )
{
	//zisime aktualnu poziciu uzla v danom okamihu
	if ( calculateNew ) {
		float graphScale = Util::ApplicationConfig::get()->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();

		//osg::Vec3 directionVector = osg::Vec3(targetPosition.x(), targetPosition.y(), targetPosition.z()) * graphScale - currentPosition;
		osg::Vec3 directionVector = osg::Vec3( mRestrictedTargetPosition.x(), mRestrictedTargetPosition.y(), mRestrictedTargetPosition.z() ) * graphScale - currentPosition;
		this->currentPosition = osg::Vec3( directionVector * ( usingInterpolation ? interpolationSpeed : 1 ) + this->currentPosition );
	}

	return osg::Vec3( this->currentPosition );
}

void Data::OsgNode::setDrawableColor( int pos, osg::Vec4 color )
{
	//nastavenie farby uzla
	osg::Geometry* geometry = NULL;

	if ( geometry != NULL ) {
		osg::Vec4Array* colorArray =  dynamic_cast<osg::Vec4Array*>( geometry->getColorArray() );

		colorArray->pop_back();
		colorArray->push_back( color );
	}
}

bool Data::OsgNode::setInvisible( bool invisible )
{
	setValue( graph->getNodeVisual(), !invisible );
	//-poriesit invisible pre label
	return true;
}

osg::Vec3f Data::OsgNode::getTargetPosition() const
{
	return mTargetPosition;
}
osg::Vec3f Data::OsgNode::targetPosition() const
{
	return mTargetPosition;
}
const osg::Vec3f& Data::OsgNode::targetPositionConstRef() const
{
	return mTargetPosition;
}
void Data::OsgNode::setTargetPosition( const osg::Vec3f& position )
{
	mTargetPosition = position;
}
osg::Vec3f Data::OsgNode::restrictedTargetPosition() const
{
	return mRestrictedTargetPosition;
}
const osg::Vec3f& Data::OsgNode::restrictedTargetPositionConstRef() const
{
	return mRestrictedTargetPosition;
}
void Data::OsgNode::setRestrictedTargetPosition( const osg::Vec3f& position )
{
	mRestrictedTargetPosition = position;
}


