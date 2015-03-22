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
