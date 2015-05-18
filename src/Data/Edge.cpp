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

//#include <QTextStream>

#include <QDebug>

Data::Edge::Edge( qlonglong id, QString name, Data::Graph* graph, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented, float scaling, int pos, osg::ref_ptr<osg::Camera> camera )
	:OsgEdge( id,name, graph, isOriented, type, scaling, srcNode, dstNode, camera )
{
	this->appConf = Util::ApplicationConfig::get();
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

osg::ref_ptr<Data::Node> Data::Edge::getSecondNode( osg::ref_ptr<Data::Node> firstNode )
{
	if ( firstNode->getId() == srcNode->getId() ) {
		return dstNode;
	}
	else {
		return srcNode;
	}

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
