#ifndef FULLHYPERGRAPHVISUALIZER_H
#define FULLHYPERGRAPHVISUALIZER_H

#include "LuaGraphVisualizer.h"

namespace Lua {

/**
     * \class FullHyperGraphVisualizer
     * \brief Implementation of LuaGraphVisualizer that draws incidences as nodes in graph
     *
     * \author Frantisek Nagy
     * \date 19.5.2014
     */
class FullHyperGraphVisualizer : public Lua::LuaGraphVisualizer
{
public:
    /**
         * \fn FullHyperGraphVisualizer
         * \brief constructor
         *
         * \params graph    Data::Graph object to fill
         * \params camera   camera for edges to face
         */
    FullHyperGraphVisualizer(Data::Graph *graph, osg::Camera *camera = NULL);

    void visualize();

    void onUpdate();
};

}


#endif // FULLHYPERGRAPHVISUALIZER_H
