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
#include <osg/Switch>
#include <QtCore/QMap>

#include <osg/Camera>

#include <QTextStream>

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
	    * \fn public setEdgePieces(QList<osg::ref_ptr<Data::Edge> > edgePieces)
	    * \brief  set pieces of splitted edge. the pieces are displayed instead of original edge
	    * \param  edgePieces list of edge pieces
	    */
	void setEdgePieces( QList<osg::ref_ptr<Data::Edge> > edgePieces );

	/**
	    * \fn public getEdgePieces()
	    * \return QList<osg::ref_ptr<Data::Edge> > edge pieces
	    */
	QList<osg::ref_ptr<Data::Edge> > getEdgePieces();

	/**
	    * \fn public clearEdgePieces()
	    * \brief  remove pieces of splitted edge. the original edge is displayed
	    */
	void clearEdgePieces();

	/**
	    * \fn public getEdgeParent()
	    * \return osg::ref_ptr<Data::Edge> edgeParent
	    */
	osg::ref_ptr<Data::Edge> getEdgeParent()
	{
		return edgeParent;
	}

	/**
	    * \fn setEdgeParent(osg::ref_ptr<Data::Edge> edgeParent)
	    * \brief  set edge parent of which is this edge piece
	    */
	void setEdgeParent( osg::ref_ptr<Data::Edge> edgeParent )
	{
		this->edgeParent = edgeParent;
	}

	/**
	    * \fn inline public constant toString
	    * \brief  Returns human-readable string representation of the Edge
	    * \return QString
	    */

	QString toString() const ;

	/**
	    *  \fn public  updateCoordinates(osg::Vec3 srcPos, osg::Vec3 dstNode)
	    *  \brief Updates coordinates of the Edge
	    *  \param    srcPos    new coordinates of the starting Node
	    *  \param   dstNode    new coordinates of the ending Node
	    */
	void updateCoordinates( osg::Vec3 srcPos, osg::Vec3 dstNode );

	/**
	    *  \fn public  createLabel(QString name)
	    *  \brief Creates new label
	    *  \param      name     new label
	    *  \return osg::ref_ptr added label
	    */
	osg::ref_ptr<osg::Geode> createLabel( QString name );

	Data::Node* getOtherNode( const Data::Node* node ) const;

    /**
     * void setLuaIdentifier( QString identifier )
     * @brief Sets lua identifier
     * @param lua identifier
     */
    void setLuaIdentifier( QString identifier ) {
        this->luaIdentifier = identifier;
    }

    /**
     * QString getLuaIdentifier()
     * @brief Gets lua identifer
     * @return lua identifier
     */
    QString getLuaIdentifier() {
        return this->luaIdentifier;
    }

    void reloadColor();

	void reloadLabel();
	void showLabel( bool visible );
	void setVisual( int index );

	static const int INDEX_LABEL = 0;
	static const int INDEX_QUAD = 1;
	static const int INDEX_CYLINDER = 2;
	static const int INDEX_LINE = 3;
	static const int INDEX_CURVE = 4;
private:
	/**
	    *  QList<osg::ref_ptr<Data::Edge> > edgePieces
	    *  \brief list contains pieces of edge if edge is splitted
	    */
	QList<osg::ref_ptr<Data::Edge> > edgePieces;

	/**
	    *  osg::ref_ptr<Data::Edge> edgeParent
	    *  \brief reference to original edge if edge is part of splitted edge
	    */
	osg::ref_ptr<Data::Edge> edgeParent;

    /**
     * QString luaIdentifier
     * @brief lua identifier
     */
    QString luaIdentifier;

	/**
	    *  Util::ApplicationConfig * appConf
	    *  \brief ApplicationConfig
	    */
	Util::ApplicationConfig* appConf;

	osg::ref_ptr<osg::StateSet> createStateSet( Data::Type* type );
	osg::ref_ptr<osg::Geode> createEdgeQuad( osg::StateSet* bbState );
	osg::ref_ptr<osg::Geode> createEdgeCylinder( osg::StateSet* bbState );
	osg::ref_ptr<osg::Geode> createEdgeLine( osg::StateSet* bbState );
	osg::ref_ptr<osg::Geode> createEdgeCurve( osg::StateSet* bbState );
};
}

#endif
