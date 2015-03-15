#ifndef HYPERGRAPHVISUALIZER_H
#define HYPERGRAPHVISUALIZER_H

#include "LuaGraphVisualizer.h"

namespace Lua {

/**
     * \class HyperGraphVisualizer
     * \brief Implementation of LuaGraphVisualizer that draws edges as nodes in graph
     *
     * \author Frantisek Nagy
     * \date 19.5.2014
     */
class HyperGraphVisualizer : public Lua::LuaGraphVisualizer
{
public:
	/**
	     * \fn HyperGraphVisualizer
	     * \brief constructor
	     *
	     * \params graph    Data::Graph object to fill
	     * \params camera   camera for edges to face
	     */
	HyperGraphVisualizer( Data::Graph* graph, osg::ref_ptr<osg::Camera> camera = NULL );

	void visualize();

	void onUpdate();
};

}

#endif // HYPERGRAPHVISUALIZER_H
