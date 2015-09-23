#include "Data/OsgEdge.h"
#include "Data/Edge.h"
#include "Data/Graph.h"

#include "Util/ApplicationConfig.h"

#include <osgText/Text>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/PrimitiveSet>
#include <Viewer/TextureWrapper.h>

#include <QTextStream>

#include <QDebug>

Data::OsgEdge::OsgEdge( qlonglong id, QString name, Data::Graph* graph, bool isOriented, Data::Type* type, float scaling, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode,osg::ref_ptr<osg::Camera> camera )
	:DbEdge( id, name, graph, isOriented, type, scaling )
{
	this->srcNode = srcNode;
	this->dstNode = dstNode;
	this->camera = camera;

	coordinates = new osg::Vec3Array();
	edgeTexCoords = new osg::Vec2Array();
	rotation = new osg::Vec3Array();
	center = new osg::Vec3Array();

	createLabel( name );
	//updateCoordinates(getSrcNode()->getTargetPosition(), getDstNode()->getTargetPosition());
	updateCoordinates( getSrcNode()->restrictedTargetPosition(), getDstNode()->restrictedTargetPosition() );

	float r = type->getSettings()->value( "color.R" ).toFloat();
	float g = type->getSettings()->value( "color.G" ).toFloat();
	float b = type->getSettings()->value( "color.B" ).toFloat();
	float a = type->getSettings()->value( "color.A" ).toFloat();
	this->edgeColor = osg::Vec4( r, g, b, a );
}

osg::ref_ptr<osg::Drawable> Data::OsgEdge::createLabel( QString name )
{
	label = new osgText::FadeText;
	label->setFadeSpeed( 0.03f );

	QString fontPath = Util::ApplicationConfig::get()->getValue( "Viewer.Labels.Font" );

	// experimental value
	float scale = 1.375f * this->type->getScale();

	if ( fontPath != NULL && !fontPath.isEmpty() ) {
		label->setFont( fontPath.toStdString() );
	}

	label->setText( name.toStdString() );
	label->setLineSpacing( 0 );
	label->setAxisAlignment( osgText::Text::SCREEN );
	label->setCharacterSize( scale );
	label->setDrawMode( osgText::Text::TEXT );
	label->setAlignment( osgText::Text::CENTER_BOTTOM_BASE_LINE );
	//label->setPosition((this->dstNode->getTargetPosition() + this->srcNode->getTargetPosition()) / 2 );
	label->setPosition( ( this->dstNode->restrictedTargetPosition() + this->srcNode->restrictedTargetPosition() ) / 2 );
	label->setColor( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

	return label;
}

osg::ref_ptr<osg::Drawable> Data::OsgEdge::createEdge( osg::StateSet* bbState )
{
	osg::ref_ptr<osg::Geometry> nodeQuad = new osg::Geometry;

	nodeQuad->setUseDisplayList( false );

	nodeQuad->setVertexArray( coordinates );
	nodeQuad->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::QUADS,0,4 ) );

	nodeQuad->setTexCoordArray( 0, edgeTexCoords );

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );

	osg::ref_ptr<osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 1> > colorIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 1>;
	colorIndexArray->push_back( 0 );

	nodeQuad->setColorArray( colorArray );
#ifdef BIND_PER_PRIMITIVE
	nodeQuad->setColorIndices( colorIndexArray );
#endif
	nodeQuad->setColorBinding( osg::Geometry::BIND_OVERALL );
	nodeQuad->setStateSet( bbState );

	return nodeQuad;
}

osg::ref_ptr<osg::StateSet> Data::OsgEdge::createStateSet( Data::Type* type )
{
	if ( !oriented ) {
		osg::StateSet* edgeStateSet = new osg::StateSet;

		edgeStateSet->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
		edgeStateSet->setTextureAttributeAndModes( 0, Vwr::TextureWrapper::getEdgeTexture(), osg::StateAttribute::ON );
		edgeStateSet->setAttributeAndModes( new osg::BlendFunc, osg::StateAttribute::ON );
		edgeStateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

		edgeStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask( false );
		edgeStateSet->setAttributeAndModes( depth, osg::StateAttribute::ON );

		return edgeStateSet;
	}
	else {
		osg::StateSet* orientedEdgeStateSet = new osg::StateSet;

		orientedEdgeStateSet->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
		orientedEdgeStateSet->setTextureAttributeAndModes( 0, Vwr::TextureWrapper::getOrientedEdgeTexture(), osg::StateAttribute::ON );
		orientedEdgeStateSet->setMode( GL_BLEND, osg::StateAttribute::ON );
		orientedEdgeStateSet->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );

		orientedEdgeStateSet->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask( false );
		orientedEdgeStateSet->setAttributeAndModes( depth, osg::StateAttribute::ON );

		return orientedEdgeStateSet;
	}
}

void Data::OsgEdge::updateCoordinates( osg::Vec3 srcPos, osg::Vec3 dstPos )
{
	coordinates->clear();
	edgeTexCoords->clear();

	osg::Vec3d viewVec( 0, 0, 1 );
	osg::Vec3d up;

	if ( camera != 0 ) {
		osg::Vec3d eye;
		osg::Vec3d center;

		camera->getViewMatrixAsLookAt( eye,center,up );

		viewVec = eye - center;

		//	std::cout << eye.x() << " " << eye.y() << " " << eye.z() << "\n";
		//	std::cout << center.x() << " " << center.y() << " " << center.z() << "\n";
	}

	viewVec.normalize();

	//getting setting for edge scale

	osg::Vec3 x, y;
	x.set( srcPos );
	y.set( dstPos );

	osg::Vec3d edgeDir = x - y;
	length = edgeDir.length();

	up = edgeDir ^ viewVec;
	up.normalize();

	up *= this->scale;

	//updating edge coordinates due to scale
	coordinates->push_back( osg::Vec3d( x.x() + up.x(), x.y() + up.y(), x.z() + up.z() ) );
	coordinates->push_back( osg::Vec3d( x.x() - up.x(), x.y() - up.y(), x.z() - up.z() ) );
	coordinates->push_back( osg::Vec3d( y.x() - up.x(), y.y() - up.y(), y.z() - up.z() ) );
	coordinates->push_back( osg::Vec3d( y.x() + up.x(), y.y() + up.y(), y.z() + up.z() ) );

	float repeatCnt = static_cast<float>( length / ( 2.f * this->scale ) );

	//init edge-text (label) coordinates
	edgeTexCoords->push_back( osg::Vec2( 0,1.0f ) );
	edgeTexCoords->push_back( osg::Vec2( 0,0.0f ) );
	edgeTexCoords->push_back( osg::Vec2( repeatCnt,0.0f ) );
	edgeTexCoords->push_back( osg::Vec2( repeatCnt,1.0f ) );

	if ( label != NULL ) {
		label->setPosition( ( srcPos + dstPos ) / 2 );
	}
	osg::Geometry* geometry = NULL;
//	osg::Geometry* geometry = getDrawable( 0 )->asGeometry();
	if ( geometry != NULL ) {
		geometry->setVertexArray( coordinates );
		geometry->setTexCoordArray( 0, edgeTexCoords );

		osg::Vec4Array* colorArray =  dynamic_cast<osg::Vec4Array*>( geometry->getColorArray() );

		colorArray->pop_back();
		/*osg::Vec4f color;
		color.r()=getEdgeColor()[0];
		color.g()=getEdgeColor()[1];
		color.b()=getEdgeColor()[2];
		color.a()=getEdgeColor()[3];*/
		colorArray->push_back( getEdgeColor() );
	}
}
