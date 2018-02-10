#ifndef LUAGRAPHVISUALIZER_H
#define LUAGRAPHVISUALIZER_H

#include "LuaGraph/LuaGraphObserver.h"

#include <osg/Camera>
#include <osg/ref_ptr>

namespace Data {
class Graph;
class Type;
class Node;
class Edge;
}

namespace Lua {

class LuaGraphObject;

/**
* \class LuaGraphVisualizer
* \brief Abstract class for differetn vizualisation of hypergraph from Lua
*
* \author Frantisek Nagy
* \date 19.5.2014
*/
class LuaGraphVisualizer : public Lua::LuaGraphObserver
{
public:
	/**
	     * \fn LuaGraphVisualizer
	     * \brief constructor
	     *
	     * \params graph    Data::Graph object to fill
	     * \params camera   camera for edges to face
	     */
	LuaGraphVisualizer( Data::Graph* graph, osg::ref_ptr<osg::Camera> camera = 0 );
	virtual ~LuaGraphVisualizer() = default;

	/**
	     * \fn visualize
	     * \brief Abstract method to fill the graph with hypergraph loaded from Lua
	     */
	virtual void visualize() = 0;
protected:
	/**
	    *   Data::Graph *currentGraph
	    *  \brief vizualised graph
	    */
	Data::Graph* currentGraph;

	/**
	    *   osg::ref_ptr<osg::Camera> camera
	    *  \brief camera of vizualisation
	    */
	osg::ref_ptr<osg::Camera> camera;

	/**
	    *   Data::Type *edgeType
	    *  \brief edge type for creating edges
	    */
	Data::Type* edgeType; // = NULL;

	/**
	    *   Data::Type *nodeType
	    *  \brief node type for creating nodes
	    */
	Data::Type* nodeType; // = NULL;

	/**
	     * \fn setNodeParams
	     * \brief set visual params to node
	     *
	     * \params osg::ref_ptr<Data::Node> node in scene to set params for
	     * \params Lua::LuaGraphObject  node, edge or incidence in hypergraph to load params
	     * \params osg::Vec4f defColor  default color
	     * \params float defSize    default size
	     */
	void setNodeParams( osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject* obj, osg::Vec4f defColor, float defSize );

	/**
	     * \fn setEdgeParams
	     * \brief set visual params to edge
	     *
	     * \params osg::ref_ptr<Data::Edge> edge in scene to set params for
	     * \params Lua::LuaGraphObject  node, edge or incidence  in hypergraph to load params
	     * \params osg::Vec4f defColor  default color
	     */
	void setEdgeParams( osg::ref_ptr<Data::Edge> edge, Lua::LuaGraphObject* obj, osg::Vec4f defColor );
};

}

#endif // LUAGRAPHVISUALIZER_H
