/*!
 * Edge.cpp
 * Projekt 3DVisual
 */
#include "Data/Edge.h"
#include "Data/Graph.h"

#include "Util/ApplicationConfig.h"

#include <osgText/Text>
#include <osg/ShapeDrawable>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/PrimitiveSet>
#include <osg/LineWidth>
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
	center = new osg::Vec3Array();
	rotation = new osg::Vec3Array();

	this->insertChild( INDEX_LABEL, createLabel( name ) , false );
	this->insertChild( INDEX_QUAD, createEdgeQuad( createStateSet( this->type ) ), false );
	this->insertChild( INDEX_CYLINDER, createEdgeCylinder( createStateSet( this->type ) ), false );
	this->insertChild( INDEX_LINE, createEdgeLine( createStateSet( this->type ) ), false );
	setValue( graph->getEdgeVisual(), true );

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

void Data::Edge::setEdgePieces( QList<osg::ref_ptr<Data::Edge> > edgePieces )
{
	this->edgePieces = edgePieces;
	setInvisible( true );
}

QList<osg::ref_ptr<Data::Edge> > Data::Edge::getEdgePieces()
{
	return edgePieces;
}

void Data::Edge::clearEdgePieces()
{
	this->edgePieces.clear();
	setInvisible( false );
}

void Data::Edge::updateCoordinates( osg::Vec3 srcPos, osg::Vec3 dstPos )
{
	coordinates->clear();
	edgeTexCoords->clear();
	rotation->clear();
	center->clear();

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

	osg::Vec3 cor1 = osg::Vec3d( x.x() + up.x(), x.y() + up.y(), x.z() + up.z() );
	osg::Vec3 cor2 = osg::Vec3d( x.x() - up.x(), x.y() - up.y(), x.z() - up.z() );
	osg::Vec3 cor3 = osg::Vec3d( y.x() - up.x(), y.y() - up.y(), y.z() - up.z() );
	osg::Vec3 cor4 = osg::Vec3d( y.x() + up.x(), y.y() + up.y(), y.z() + up.z() );
	//center between coordinates (1 and 2), (3 and 4)
	osg::Vec3 cor12 = osg::Vec3( ( cor1.x() + cor2.x() )/2, ( cor1.y() + cor2.y() )/2, ( cor1.z() + cor2.z() )/2 );
	osg::Vec3 cor34 = osg::Vec3( ( cor3.x() + cor4.x() )/2, ( cor3.y() + cor4.y() )/2, ( cor3.z() + cor4.z() )/2 );

	osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;
	points->push_back( cor12 );
	points->push_back( cor34 );

	//updating edge coordinates due to scale
	coordinates->push_back( cor1 );
	coordinates->push_back( cor2 );
	coordinates->push_back( cor3 );
	coordinates->push_back( cor4 );

	float repeatCnt = static_cast<float>( length / ( 2.f * this->scale ) );
	//init edge-text (label) coordinates
	edgeTexCoords->push_back( osg::Vec2( 0,1.0f ) );
	edgeTexCoords->push_back( osg::Vec2( 0,0.0f ) );
	edgeTexCoords->push_back( osg::Vec2( repeatCnt,0.0f ) );
	edgeTexCoords->push_back( osg::Vec2( repeatCnt,1.0f ) );

	center->push_back( osg::Vec3( ( cor12.x() + cor34.x() )/2, ( cor12.y() + cor34.y() )/2, ( cor12.z() + cor34.z() )/2 ) );

	//default direction for the cylinders
	osg::Vec3 direction = osg::Vec3( 0,0,1 );
	// diff between cor12 and cor34
	osg::Vec3 diff = ( cor34 - cor12 );
	// CROSS product (the axis of rotation)
	rotation->push_back( direction ^ diff );
	angle = acos( ( direction * diff )/ diff.length() );

	osgText::FadeText* label = dynamic_cast<osgText::FadeText*>( getChild( INDEX_LABEL )->asGeode()->getDrawable( 0 ) );
	if ( label != NULL ) {
		label->setPosition( center->at( 0 ) );
	}

	osg::Geometry* geometryQuad = getChild( INDEX_QUAD )->asGeode()->getDrawable( 0 )->asGeometry();
	if ( geometryQuad != NULL ) {
		geometryQuad->setVertexArray( coordinates );
		geometryQuad->setTexCoordArray( 0, edgeTexCoords );

		osg::Vec4Array* colorArray =  dynamic_cast<osg::Vec4Array*>( geometryQuad->getColorArray() );

		colorArray->pop_back();
		colorArray->push_back( getEdgeColor() );
	}

	osg::ShapeDrawable* drawableCylinder = dynamic_cast<osg::ShapeDrawable*>( getChild( INDEX_CYLINDER )->asGeode()->getDrawable( 0 ) );
	if ( drawableCylinder != NULL ) {
		( ( osg::Cylinder* )( ( drawableCylinder )->getShape() ) )->setHeight( ( float )( length ) );
		( ( osg::Cylinder* )( ( drawableCylinder )->getShape() ) )->setRadius( 2 );
		( ( osg::Cylinder* )( ( drawableCylinder )->getShape() ) )->setCenter( center->at( 0 ) );
		( ( osg::Cylinder* )( ( drawableCylinder )->getShape() ) )->setRotation( osg::Quat( angle, osg::Vec3( rotation->at( 0 ).x(), rotation->at( 0 ).y(), rotation->at( 0 ).z() ) ) );
		drawableCylinder->setColor( getEdgeColor() );
		drawableCylinder->dirtyDisplayList();
	}

	osg::Geometry* geometryLine = getChild( INDEX_LINE )->asGeode()->getDrawable( 0 )->asGeometry();
	if ( geometryLine != NULL ) {
		geometryLine->setVertexArray( points );

		osg::Vec4Array* colorArray =  dynamic_cast<osg::Vec4Array*>( geometryLine->getColorArray() );

		colorArray->pop_back();
		colorArray->push_back( getEdgeColor() );
	}

}

osg::ref_ptr<osg::Geode> Data::Edge::createLabel( QString name )
{
	osg::ref_ptr<osgText::FadeText> label = new osgText::FadeText;
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

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( label );

	return geode;
}
float Data::Edge::getEdgeStrength() const
{
	return edgeStrength;
}

void Data::Edge::setEdgeStrength( float value )
{
	edgeStrength = value;
}

bool Data::Edge::setInvisible( bool invisible )
{
	setValue( graph->getEdgeVisual(), !invisible );
	isInvisible=invisible;
	//-poriesit invisible pre label
	return true;
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

osg::ref_ptr<osg::Geode> Data::Edge::createEdgeQuad( osg::StateSet* bbState )
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

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( nodeQuad );

	return geode;
}

osg::ref_ptr<osg::Geode> Data::Edge::createEdgeCylinder( osg::StateSet* bbState )
{
	osg::ref_ptr<osg::ShapeDrawable> nodeCylinder = new osg::ShapeDrawable();

	nodeCylinder->setUseDisplayList( false );

	osg::ref_ptr<osg::Cylinder> cylinder = new osg::Cylinder();
	nodeCylinder->setShape( cylinder );

	nodeCylinder->setColor( osg::Vec4( 1.0f, 1.0f, 1.0f, 0.5f ) );
	nodeCylinder->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
	nodeCylinder->getStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::ON );
	nodeCylinder->getStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::ON );
	nodeCylinder->getStateSet()->setAttributeAndModes( new osg::BlendFunc, osg::StateAttribute::ON );
	nodeCylinder->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	nodeCylinder->getStateSet()->setRenderBinDetails( 11, "RenderBin" );

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable( nodeCylinder );

	return geode;
}

osg::ref_ptr<osg::Geode> Data::Edge::createEdgeLine( osg::StateSet* bbState )
{
	osg::ref_ptr<osg::Geometry> nodeLine = new osg::Geometry;

	nodeLine->setUseDisplayList( false );

	nodeLine->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES, 0, 2, 1 ) );

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
	nodeLine->setColorArray( colorArray );
	nodeLine->setColorBinding( osg::Geometry::BIND_OVERALL );

	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth( 1.0f );

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->getOrCreateStateSet()->setAttributeAndModes( linewidth,
			osg::StateAttribute::ON );

	geode->addDrawable( nodeLine );

	return geode;
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
	setValue( INDEX_LABEL, visible );
}

void Data::Edge::reloadLabel()
{
	removeChild( INDEX_LABEL, 1 );
	insertChild( INDEX_LABEL, createLabel( name ), false );
}

void Data::Edge::setVisual( int index )
{
	setValue( INDEX_QUAD, false );
	setValue( INDEX_CYLINDER, false );
	setValue( INDEX_LINE, false );
	setValue( index, true );
}
