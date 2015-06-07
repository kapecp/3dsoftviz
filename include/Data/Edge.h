/*!
* Edge.h
* Projekt 3DVisual
*/
#ifndef DATA_EDGE_DEF
#define DATA_EDGE_DEF 1

#include "Data/Node.h"
#include "Data/OsgEdge.h"
#include "Data/AbsEdge.h"

#include <QString>
#include <QtCore/QMap>

#include <osg/Camera>

namespace Util {
class ApplicationConfig;
}

namespace Data {
class Node;
class Graph;
class Type;

/**
    *  \class Edge
    *  \brief Object represents an edge in a Graph.
    *  \author Aurel Paulovic, Michal Paprcka
    *  \date 29. 4. 2010
    */
class Edge : public OsgEdge
{
public:
	// Marak start
	void setEdgeDefaultColor();
	// Marak end

	/**
	    * \fn public constructor Edge(qlonglong id, QString name, Data::Graph* graph, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented, int pos = 0, osg::ref_ptr<osg::Camera> camera = 0)
	    * \brief  Creates new Edge object connecting two Nodes
	    * \param id	ID of the Edge
	    * \param name 		name of the Edge
	    * \param graph 	Graph to which the Edge belongs
	    * \param srcNode 		starting Node
	    * \param dstNode 	ending Node
	    * \param type 		Type of the Edge.
	    * \param isOriented 		true if the Edge is oriented.
	    * \param pos int		first coordinate in Drawable coordinates array
	    * \param camera 	current camera used in viewer
	    */
	Edge( qlonglong id, QString name, Data::Graph* graph, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented, float scaling, int pos = 0, osg::ref_ptr<osg::Camera> camera = 0 );

	/**
	    *  \fn public destructor  ~Edge
	    *  \brief Destroys the Edge object
	    */
	~Edge( void );

	/**
	    * \fn public getSecondNode(osg::ref_ptr<Data::Node> firstNode)
	    * \brief  Returns the second ending Node of the Edge
	    * \return osg::ref_ptr<Data::Node> the second ending Node of the Edge
	*/
	osg::ref_ptr<Data::Node> getSecondNode( osg::ref_ptr<Data::Node> firstNode );

	/**
	    * \fn public linkNodes(QMap<qlonglong, osg::ref_ptr<Data::Edge> > *edges)
	    * \brief  Links the Edge to it's Nodes and adds itself to the edges
	    * \param  edges
	    */
	void linkNodes( QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges );

	/**
	    * \fn public unlinkNodes
	    * \brief Unlinks the Edge from the Nodes
	    */
	void unlinkNodes();

	/**
	    * \fn public unlinkNodesAndRemoveFromGraph
	    * \brief Unlinks the Edge from the Nodes and removes the Edge from it's Graph
	    */
	void unlinkNodesAndRemoveFromGraph();

	/**
	    *  \fn inline public constant  getCooridnates
	    *  \brief Returns coordinates of the Edge
	    *  \return osg::ref_ptr coordinates of the Edge
	    */
	osg::ref_ptr<osg::Vec3Array> getCooridnates() const
	{
		return coordinates;
	}

	/**
	    *  \fn inline public constant  getEdgeTexCoords
	    *  \brief Returns Texture coordinates array.
	    *  \return osg::ref_ptr<osg::Vec2Array>
	    */
	osg::ref_ptr<osg::Vec2Array> getEdgeTexCoords() const
	{
		return edgeTexCoords;
	}

	Data::Node* getOtherNode( const Data::Node* node ) const;

	void reloadLabel();
	void showLabel( bool visible );

private:

	/**
	    *  Util::ApplicationConfig * appConf
	    *  \brief ApplicationConfig
	    */
	Util::ApplicationConfig* appConf;
};
}

#endif
