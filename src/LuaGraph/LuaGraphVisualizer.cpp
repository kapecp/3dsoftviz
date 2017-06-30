#include "LuaGraph/LuaGraphVisualizer.h"

#include "LuaGraph/LuaGraphObject.h"

#include "Importer/GraphOperations.h"
#include "Data/Graph.h"
#include "Data/Edge.h"

Lua::LuaGraphVisualizer::LuaGraphVisualizer( Data::Graph* graph, osg::ref_ptr<osg::Camera> camera )
{
	this->nodeType = NULL;
	this->edgeType = NULL;
	currentGraph = graph;
	this->camera = camera;
	Importer::GraphOperations* operations = new Importer::GraphOperations( *currentGraph );
	operations->addDefaultTypes( edgeType, nodeType );
	delete operations;
}

void Lua::LuaGraphVisualizer::setNodeParams( osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject* obj, osg::Vec4f defColor, float defSize )
{

	node->setLabelText( obj->getLabel() );

//	if(obj->getLabel().isEmpty()) {
//		node->setLabelText(obj->getLabel());
//	}else{
//		node->setLabelText(obj->getLabel() + "\n" + QString(obj->getParams().asTable().at("type").asString().c_str()));
//	}

	node->Data::AbsNode::setName( obj->getLabel() );
	float r = obj->getFloatParam( "color", "R", defColor.r() );
	float g = obj->getFloatParam( "color", "G", defColor.g() );
	float b = obj->getFloatParam( "color", "B", defColor.b() );
	float a = obj->getFloatParam( "color", "A", defColor.a() );
	node->setColor( osg::Vec4( r,g,b,a ) );
	float scale = obj->getFloatParam( "size", defSize );
	node->setScale( scale );
	node->reloadConfig();
}

void Lua::LuaGraphVisualizer::setEdgeParams( osg::ref_ptr<Data::Edge> edge, Lua::LuaGraphObject* obj, osg::Vec4f defColor )
{
	//( ( Data::AbsEdge* )edge )->setName( obj->getLabel() );
	( dynamic_cast<Data::AbsEdge*>( edge.get() ) )->setName( obj->getLabel() );
	float r = obj->getFloatParam( "color", "R", defColor.r() );
	float g = obj->getFloatParam( "color", "G", defColor.g() );
	float b = obj->getFloatParam( "color", "B", defColor.b() );
	float a = obj->getFloatParam( "color", "A", defColor.a() );
	edge->setEdgeColor( osg::Vec4( r,g,b,a ) );
	edge->setEdgeStrength( obj->getFloatParam( "edgeStrength", 1 ) );

}
