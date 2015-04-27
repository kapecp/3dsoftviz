/*!
 * Edge.cpp
 * Projekt 3DVisual
 */
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

Data::Edge::Edge( qlonglong id, QString name, Data::Graph* graph, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented, float scaling, int pos, osg::ref_ptr<osg::Camera> camera )
{
	this->id = id;
	this->name = name;
	this->graph = graph;
	this->srcNode = srcNode;
	this->dstNode = dstNode;
	this->type = type;
	this->oriented = isOriented;
	this->camera = camera;
	this->selected = false;
	this->setSharedCoordinates( false, false, false );
	this->inDB = false;
	this->scale = scaling;
	float r = type->getSettings()->value( "color.R" ).toFloat();
	float g = type->getSettings()->value( "color.G" ).toFloat();
	float b = type->getSettings()->value( "color.B" ).toFloat();
	float a = type->getSettings()->value( "color.A" ).toFloat();

	this->edgeColor = osg::Vec4( r, g, b, a );

	this->edgeStrength = 1;

	this->appConf = Util::ApplicationConfig::get();
	coordinates = new osg::Vec3Array();
	edgeTexCoords = new osg::Vec2Array();

	this->addDrawable( createEdge( createStateSet( this->type ) ) );
	createLabel( name );
	//updateCoordinates(getSrcNode()->getTargetPosition(), getDstNode()->getTargetPosition());
	updateCoordinates( getSrcNode()->restrictedTargetPosition(), getDstNode()->restrictedTargetPosition() );
}


Data::Edge::~Edge( void )
{
	this->graph = NULL;
	if ( this->srcNode!=NULL ) {
		this->srcNode->removeEdge( this );
		this->srcNode = NULL;
	}

	if ( this->dstNode!=NULL ) {
		this->dstNode->removeEdge( this );
		this->dstNode = NULL;
	}

	this->type = NULL;
	this->appConf = NULL;
}

void Data::Edge::linkNodes( QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges )
{
	edges->insert( this->id, this );
	this->dstNode->addEdge( this );
	this->srcNode->addEdge( this );
}

void Data::Edge::unlinkNodes()
{
	this->dstNode->removeEdge( this );
	this->srcNode->removeEdge( this );
	this->srcNode = NULL;
	this->dstNode = NULL;
}

void Data::Edge::unlinkNodesAndRemoveFromGraph()
{
	//unlinkNodes will be called from graph->removeEdge !!
	this->graph->removeEdge( this );
}

void Data::Edge::updateCoordinates( osg::Vec3 srcPos, osg::Vec3 dstPos )
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

	osg::Geometry* geometry = getDrawable( 0 )->asGeometry();
	if ( geometry != NULL ) {
		geometry->setVertexArray( coordinates );
		geometry->setTexCoordArray( 0, edgeTexCoords );

		osg::Vec4Array* colorArray =  dynamic_cast<osg::Vec4Array*>( geometry->getColorArray() );

		colorArray->pop_back();
		colorArray->push_back( getEdgeColor() );
	}
}

osg::ref_ptr<osg::Drawable> Data::Edge::createLabel( QString name )
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
float Data::Edge::getEdgeStrength() const
{
	return edgeStrength;
}

void Data::Edge::setEdgeStrength( float value )
{
	edgeStrength = value;
}


osg::ref_ptr<Data::Node> Data::Edge::getSecondNode( osg::ref_ptr<Data::Node> firstNode )
{
	if ( firstNode->getId() == srcNode->getId() ) {
		return dstNode;
	}
	else {
		return srcNode;
	}

}


QString Data::Edge::toString() const
{
	QString str;
	QTextStream( &str ) << "edge id:" << id << " name:" << name;
	return str;
}

Data::Node* Data::Edge::getOtherNode( const Data::Node* node ) const
{
	if ( node == NULL ) {
		return NULL;
	}
	if ( node->getId() == srcNode->getId() ) {
		return dstNode;
	}
	if ( node->getId() == dstNode->getId() ) {
		return srcNode;
	}
	qWarning() << "Node not incident to this edge";
	return NULL;
}

osg::ref_ptr<osg::Drawable> Data::Edge::createEdge( osg::StateSet* bbState )
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

osg::ref_ptr<osg::StateSet> Data::Edge::createStateSet( Data::Type* type )
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

void Data::Edge::showLabel( bool visible )
{
	//nastavenie zobrazenia popisku uzla
	if ( visible && !this->containsDrawable( label ) ) {
		this->addDrawable( label );
	}
	else if ( !visible ) {
		this->removeDrawable( label );
	}
}

void Data::Edge::reloadLabel()
{
	osg::ref_ptr<osg::Drawable> newLabel = createLabel( name );

	if ( this->containsDrawable( label ) ) {
		this->setDrawable( this->getDrawableIndex( label ), newLabel );
	}
}

//Marak start
void Data::Edge::setEdgeDefaultColor()
{
	float r = type->getSettings()->value( "color.R" ).toFloat();
	float g = type->getSettings()->value( "color.G" ).toFloat();
	float b = type->getSettings()->value( "color.B" ).toFloat();
	float a = type->getSettings()->value( "color.A" ).toFloat();

	this->edgeColor = osg::Vec4( r, g, b, a );
}
//Marak end
