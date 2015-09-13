#ifndef LUAGRAPHSIMPLEGRAPHVISUALIZER_H
#define LUAGRAPHSIMPLEGRAPHVISUALIZER_H

#include "LuaGraphVisualizer.h"

namespace Lua {

/**
     * \class SimpleGraphVisualizer
     * \brief Implementation of LuaGraphVisualizer that draws hypergraph as graph
     *
     * Note: edges must connect 2 nodes
     *
     * \author Frantisek Nagy
     * \date 19.5.2014
     */
class SimpleGraphVisualizer : public Lua::LuaGraphVisualizer
{
public:
	/**
	     * \fn HyperGraphVisualizer
	     * \brief constructor
	     *
	     * \params graph    Data::Graph object to fill
	     * \params camera   camera for edges to face
	     */
	SimpleGraphVisualizer( Data::Graph* graph, osg::ref_ptr<osg::Camera> camera = 0 );

	void visualize();

	void onUpdate();
};

}

#endif // LUAGRAPHSIMPLEGRAPHVISUALIZER_H
