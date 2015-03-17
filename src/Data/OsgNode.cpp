#include "Data/OsgNode.h"

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
}

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


