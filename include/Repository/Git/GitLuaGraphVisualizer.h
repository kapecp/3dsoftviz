#ifndef GITLUAGRAPHVISUALIZER_H
#define GITLUAGRAPHVISUALIZER_H

#include "osg/ref_ptr"
#include "osg/Camera"
#include "osg/Vec4f"

#include <QtGlobal>
#include <QString>

namespace Lua {
class LuaGraph;
class LuaGraphObject;
}

namespace Data {
class Graph;
class Node;
class Edge;
}

namespace Repository {
namespace Git {
class GitEvolutionGraph;
class GitFile;
class GitFunction;

class GitLuaGraphVisualizer
{
public:
    GitLuaGraphVisualizer( Data::Graph* currentGraph, Repository::Git::GitEvolutionGraph* evolutionGraph, osg::ref_ptr<osg::Camera> camera );

    void visualize( bool next );
private:
    void setNodeParams( osg::ref_ptr<Data::Node> node, Lua::LuaGraphObject* obj, osg::Vec4f defColor, float defSize );
    void setEdgeParams( osg::ref_ptr<Data::Edge> edge, Lua::LuaGraphObject* obj, osg::Vec4f defColor );

    void updateCurrentGraphNodesId();

    bool addFileToGraph( Repository::Git::GitFile* file, QString rootIdentifier );

    bool processFunctionsFromFile( Repository::Git::GitFile* file, bool next );

    bool addFunctionToGraph( Repository::Git::GitFunction* function, QString masterIdentifier );

    bool removeFunctionFromGraph( Repository::Git::GitFunction* function, QString masterIdentifier, bool next );

    bool removeFileFromGraph( Repository::Git::GitFile* file, bool next );

    bool addModuleFromGlobalFunction( Repository::Git::GitFunction* function );

    bool removeModuleFromGlobalFunction( Repository::Git::GitFunction* function, bool next );

    void addDefaultTypes();

    void addCustomTypes();

    void processRemovedNodesAndEdges();

    /**
     * Data::Graph* currentGraph
     * @brief Visualized graph
     */
    Data::Graph* currentGraph;

    /**
     * Repository::Git::GitEvolutionGraph* evolutionGraph
     * @brief Evolution graph
     */
    Repository::Git::GitEvolutionGraph* evolutionGraph;

    /**
     * Lua::LuaGraph* luaGraph
     * @brief Lua graph
     */
    Lua::LuaGraph* luaGraph;

    osg::ref_ptr<osg::Camera> camera;

}; // class
} // namespace
}
#endif // GITLUAGRAPHVISUALIZER_H
