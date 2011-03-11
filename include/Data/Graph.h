/*!
 * Graph.h
 * Projekt 3DVisual
 */

#ifndef DATA_GRAPH_DEF
#define DATA_GRAPH_DEF 1

#include <osg/ref_ptr>

#include "Data/Node.h"
#include "Data/Type.h"
#include "Data/Edge.h"
#include "Data/MetaType.h"
#include "Data/GraphLayout.h"
#include "Model/GraphDAO.h"
#include "Model/GraphLayoutDAO.h"
#include "Model/TypeDAO.h"
#include "Model/NodeDAO.h"
#include "Model/EdgeDAO.h"


#include <QString>
#include <QTextStream>
#include <QList>
#include <QDebug>
#include <QtSql>
#include <QMutableMapIterator>

#define METASTRENGTH 1

namespace Data
{
    class Node;
    class Type;
    class Edge;
	class MetaType;
    class GraphLayout;
    
	/**
	*  \class Graph
	*  \brief Object represents a graph structure.
	*
	*	Graph object is a representation of an graph. A graph is a set of Nodes connected via Edges. Each Node and/or Edge has its Type. 
	*	A graph is layouted and displayed by the application. Each graph can have 1 or more GraphLayouts. A GraphLayout represents single layout of the Nodes and Edges. Each GraphLayout can have it's own meta-Edges and meta-Nodes 
	* which can be used by the user to influence the layout of the graph.
	*
	*  \author Aurel Paulovic
	*  \date 29. 4. 2010
	*/
	class Graph
    {
    public:
        
		/**
		*  \fn public overloaded constructor  Graph(qlonglong graph_id, QString name, QSqlDatabase* conn, QMap<qlonglong,osg::ref_ptr<Data::Node> > *nodes, QMap<qlonglong,osg::ref_ptr<Data::Edge> > *edges,QMap<qlonglong,osg::ref_ptr<Data::Node> > *metaNodes, QMap<qlonglong,osg::ref_ptr<Data::Edge> > *metaEdges, QMap<qlonglong,Data::Type*> *types)
		*  \brief Creates new Graph Object from provided nodes, edges, types, metaNodes and metaEdges
		*
		*	This constructor is obsolete, use Graph(qlonglong graph_id, QString name, qlonglong layout_id_counter, qlonglong ele_id_counter, QSqlDatabase* conn) instead
		*
		*  \param   graph_id     ID of the graph
		*  \param   name     name of the graph
		*  \param   conn    connection to DB
		*  \param   nodes   provided nodes
		*  \param   edges    provided edges
		*  \param   metaNodes   provided  metaNodes
		*  \param   metaEdges     provided metaEdges
		*  \param   types   provided types
		*/
		Graph(qlonglong graph_id, QString name, QSqlDatabase* conn, QMap<qlonglong,osg::ref_ptr<Data::Node> > *nodes, QMap<qlonglong,osg::ref_ptr<Data::Edge> > *edges,QMap<qlonglong,osg::ref_ptr<Data::Node> > *metaNodes, QMap<qlonglong,osg::ref_ptr<Data::Edge> > *metaEdges, QMap<qlonglong,Data::Type*> *types);
    
		/**
		*  \fn public overloaded constructor  Graph(qlonglong graph_id, QString name, qlonglong layout_id_counter, qlonglong ele_id_counter, QSqlDatabase* conn)
		*  \brief Creates new Graph Object
		*  \param   graph_id     ID of the graph
		*  \param   name     name of the graph
		*  \param   layout_id_counter     current maximum graphLayout ID (for empty graph use 1)
		*  \param   ele_id_counter     current maximum element (Node, Type, Edge) ID (for empty graph use 1)
		*  \param   conn     connection to DB
		*/
		Graph(qlonglong graph_id, QString name, qlonglong layout_id_counter, qlonglong ele_id_counter, QSqlDatabase* conn);

		/**
		*  \fn public destructor  ~Graph
		*  \brief Destroys the Graph object and performs delete on Type objects
		*/
		~Graph(void);


		/**
		*  \fn inline public static  getMetaStrength
		*  \brief Returns the strength of meta-element for layout algorithm
		*  \return float strength of the meta-element
		*/
		float static getMetaStrength() { return (float) METASTRENGTH; }
        

		/**
		*  \fn inline public  getId
		*  \brief Returns ID of the Graph
		*  \return qlonglong ID of the Graph
		*/
		qlonglong getId() { return graph_id; } 

        /**
        * \fn qlonglong setId(qlonglong graph_id)
        * \brief Sets new ID of Graph unless it is already in DB
        * \param 		graph_id    new ID of the Graph. 
        * \return qlonglong resultant ID of Graph
        */
        qlonglong setId(qlonglong graph_id) { if(!inDB) this->graph_id = graph_id; return this->graph_id; } 
        

		/**
		*  \fn public  setName(QString name)
		*  \brief Sets new name
		*  \param   name     new Name for the Graph
		*  \return QString resultant name of the Graph
		*/
		QString setName(QString name);  

		/**
		*  \fn inline public  getName
		*  \brief Returns the name of the Graph
		*  \return QString name of the Graph
		*/
		QString getName() { return name; } 
        

		/**
		*  \fn inline public  getEleIdCounter
		*  \brief Returns current value of element ID counter
		*  \return qlonglong current value of element ID counter
		*/
		qlonglong getEleIdCounter() { return ele_id_counter; } 

		/**
		*  \fn inline public  getLayoutIdCounter
		*  \brief Returns current value of graphLayout ID counter
		*  \return qlonglong current value of graphLayout ID counter
		*/
		qlonglong getLayoutIdCounter() { return layout_id_counter; } 
        
        

		/**
		*  \fn public  getLayouts(bool* error)
		*  \brief Returns QMap of graphLayouts belonging to the Graph
		*  \param   error  error flag, will be set true, if the method encounters an error
		*  \return QMap<qlonglong,Data::GraphLayout*> graphLayouts belonging to the Graph
		*/
		QMap<qlonglong, Data::GraphLayout*> getLayouts(bool* error); 

		/**
		*  \fn public  addLayout(QString layout_name)
		*  \brief Creates new GraphLayout and adds it to the Graph
		*  \param       layout_name    name of the new GraphLayout 
		*  \return Data::GraphLayout * new GraphLayout
		*/
		Data::GraphLayout* addLayout(QString layout_name); 

		/**
		*  \fn public  removeLayout(Data::GraphLayout* layout)
		*  \brief Deletes an GraphLayout belonging to the Graph
		*
		*	Does not perform delete on the GraphLayout
		*
		*  \param  layout    GraphLayout to be deleted
		*  \return bool true, if the GraphLayout was successfully deleted
		*/
		bool removeLayout(Data::GraphLayout* layout);
        

		/**
		*  \fn inline public  getSelectedLayout
		*  \brief Returns currently selected GraphLayout (used by all MetaTypes)
		*  \return Data::GraphLayout * currently selected GraphLayout
		*/
		Data::GraphLayout* getSelectedLayout() { return selectedLayout; } 

		/**
		*  \fn public  selectLayout(Data::GraphLayout* layout)
		*  \brief Selects GraphLayout
		*  \param  layout    GraphLayout that should be selected
		*  \return Data::GraphLayout * resulting selected GraphLayout
		*/
		Data::GraphLayout* selectLayout(Data::GraphLayout* layout); 
        

		/**
		*  \fn public  addNode(QString name, Data::Type* type, osg::Vec3f position = osg::Vec3f(0,0,0))
		*  \brief Creates new Node and adds it to the Graph
		*  \param   name    name of the Node 
		*  \param   type   Type of the Node
		*  \param   position     position of the Node
		*  \return osg::ref_ptr the added Node
		*/
		osg::ref_ptr<Data::Node> addNode(QString name, Data::Type* type, osg::Vec3f position = osg::Vec3f(0,0,0)); 
        
		/**
		*  \fn public  createNestedGraph(osg::ref_ptr<Data::Node> srcNode)
		*  \brief creates nested graph where can be added nested nodes and edges
		*/
		void Data::Graph::createNestedGraph(osg::ref_ptr<Data::Node> srcNode);

		/**
		*  \fn public  closeNestedGraph()
		*  \brief close nested graph after creation
		*/
		void Data::Graph::closeNestedGraph();

		/**
		*  \fn public isNestedGraph()
		*  \brief recognize if current graph is nested or not
		*  \return bool value if current graph is nested
		*/
		bool Data::Graph::isNestedGraph();

		/**
		*  \fn public  addEdge(QString name, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented)
		*  \brief Creates new Edge and adds it to the Graph
		*  \param   name     name of the Edge
		*  \param   srcNode    starting Node of the Edge
		*  \param   dstNode     ending Node of the Edge
		*  \param   type    Type of the Edge
		*  \param   isOriented   true, if the Edge is oriented  
		*  \return osg::ref_ptr the added Edge
		*/
		osg::ref_ptr<Data::Edge> addEdge(QString name, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented); 
        

		/**
		*  \fn public  addType(QString name, QMap <QString, QString> *settings = 0)
		*  \brief Creates new Type and adds it to the Graph
		*  \param    name     name of the Type
		*  \param   settings     settings of the Type
		*  \return Data::Type * the added Type
		*/

		Data::Type* addType(QString name, QMap <QString, QString> *settings = 0); //implemented

		/**
		*  \fn public  addMetaType(QString name, QMap <QString, QString> *settings = 0)
		*  \brief Creates new MetaType and adds it to the Graph
		*  \param   name     name of the MetaType
		*  \param settings     settings of the MetaType
		*  \return Data::MetaType * the added MetaType
		*/
        Data::MetaType* addMetaType(QString name, QMap <QString, QString> *settings = 0); //implemented
        

		/**
		*  \fn public  removeNode(osg::ref_ptr<Data::Node> node)
		*  \brief Removes a Node from the Graph
		*  \param      node   the Node to be removed from the Graph
		*/
		void removeNode(osg::ref_ptr<Data::Node> node);
        
		/**
		*  \fn public  addMultiEdge(QString name, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented, osg::ref_ptr<Data::Edge> replacedSingleEdge)
		*  \brief Adds an Edge of MultiEdge type to Graph
		*  \param   name     name of the Edge
		*  \param   srcNode    starting Node of the Edge
		*  \param   dstNode     ending Node of the Edge
		*  \param   type    Type of the Edge
		*  \param   isOriented   true, if the Edge is oriented  
		*  \return osg::ref_ptr the added Edge
		*/
		void addMultiEdge(QString name, osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode, Data::Type* type, bool isOriented, osg::ref_ptr<Data::Edge> replacedSingleEdge);

		/**
		*  \fn public isParralel(osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode)
		* brief tests if edge between srcNode and dstNode is MultiNode type
		*  \param   srcNode    starting Node of the Edge
		*  \param   dstNode     ending Node of the Edge
		*  \return bool value, if edge is MultiType
		*/
		bool isParralel(osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> dstNode);

		/**
		*  \fn public getMultiEdgeNeighbour(osg::ref_ptr<Data::Edge> multiEdge)
		* brief gets closer neighbour Node of non MultiNode type
		*  \param   multiEdge    multiEdge type Edge whose non Multi Node is searched
		*  \return osg::ref_ptr the found non Multi Node neigbour
		*/
		osg::ref_ptr<Data::Node> getMultiEdgeNeighbour(osg::ref_ptr<Data::Edge> multiEdge);

		/**
		*  \fn public  removeEdge(osg::ref_ptr<Data::Edge> edge)
		*  \brief Removes an Edge from the Graph
		*  \param       edge the Edge to be removed from the Graph  
		*/
		void removeEdge(osg::ref_ptr<Data::Edge> edge);
        

		/**
		*  \fn public  removeType(Data::Type* type)
		*  \brief Removes a Type from the Graph
		*  \param  type the Type to be removed from the Graph   
		*/
		void removeType(Data::Type* type);
        

		/**
		*  \fn public  saveGraphToDB
		*  \brief Saves Graph to the database
		*  \return bool true, if the Graph was successfully saved
		*/
		bool saveGraphToDB(); 

		/**
		*  \fn inline public constant  getNodes
		*  \brief Returns QMap of the Nodes assigned to the Graph
		*  \return QMap<qlonglong,osg::ref_ptr<Data::Node> > * Nodes assigned to the Graph
		*/
		QMap<qlonglong, osg::ref_ptr<Data::Node> >* getNodes() const { return nodes; } 


		/**
		*  \fn inline public constant  getEdges
		*  \brief Returns QMap of the Edges assigned to the Graph
		*  \return QMap<qlonglong,osg::ref_ptr<Data::Edge> > * Edges assigned to the Graph
		*/
		QMap<qlonglong, osg::ref_ptr<Data::Edge> >* getEdges() const { return edges; } 


		/**
		*  \fn inline public constant  getMetaNodes
		*  \brief Returns QMap of the meta-Nodes assigned to the Graph
		*  \return QMap<qlonglong,osg::ref_ptr<Data::Node> > * meta-Nodes assigned to the Graph
		*/
		QMap<qlonglong, osg::ref_ptr<Data::Node> >* getMetaNodes() const { return metaNodes; } 


		/**
		*  \fn inline public constant  getMetaEdges
		*  \brief Returns meta-Edges assigned to the Graph
		*  \return QMap<qlonglong,osg::ref_ptr<Data::Edge> > * meta-Edges assigned to the Graph
		*/
		QMap<qlonglong, osg::ref_ptr<Data::Edge> >* getMetaEdges() const { return metaEdges; } 


		/**
		*  \fn inline public constant  getTypes
		*  \brief Returns QMap of the Types assigned to the Graph
		*  \return QMap<qlonglong,Data::Type*> * Tyeps assigned to the Graph
		*/
		QMap<qlonglong, Data::Type*>* getTypes() const { return types; }


		/**
		*  \fn inline public constant  isFrozen
		*  \brief Returns true, if the Graph is frozen (for layout algorithm)
		*  \return bool true, if the Graph is frozen
		*/
		bool isFrozen() const { return frozen; } 

		/**
		*  \fn inline public  setFrozen(bool val)
		*  \brief Sets the frozen flag of the Graph to val
		*  \param      val     true, if the graph shall be frozen
		*/
		void setFrozen(bool val) { frozen = val; } 
        

		/**
		*  \fn inline public  isInDB
		*  \brief Returns true, if the Graph Object is in database
		*
		*	Returns true if the Graph Object (not counting it's nodes, egdes, types, layouts, ...) is in database.
		*
		*  \return bool true, if the Graph is in DB
		*/
		bool isInDB() { return inDB; } 

		/**
		*  \fn inline public  setIsInDB
		*  \brief Sets the inDB flag of the Graph to true
		*/
		void setIsInDB() { inDB = true; }; 
        

		/**
		*  \fn public  getTypesByName(QString name)
		*  \brief Returns all Types assigned to the Graph with name
		*  \param       name     name of the searched Type
		*  \return QList<Data::Type*> searched Types
		*/
		QList<Data::Type*> getTypesByName(QString name); 


		/**
		*  \fn public constant  toString
		*  \brief Returns human-readable string representing the Graph
		*  \return QString 
		*/
		QString toString() const; 
        

		/**
		*  \fn public  getNodeMetaType
		*  \brief Returns MetaType for meta-Nodes
		*  \return Data::Type * MetaType for the Nodes
		*/
		Data::Type* getNodeMetaType(); 

		/**
		*  \fn public  getNodeMultiType
		*  \brief Returns MultiType for multi-Nodes (in Multi Edge case)
		*  \return Data::Type * MultiType for the Nodes
		*/
		Data::Type* getNodeMultiType();

		/**
		*  \fn public  getEdgeMetaType
		*  \brief Returns MetaType for meta-Edges
		*  \return Data::Type * MetaType for the Edges
		*/
		Data::Type* getEdgeMetaType(); 
    private:

		/**
		*  \fn inline private  incEleIdCounter
		*  \brief Increments and returns the incremented value of element ID counter
		*  \return qlonglong incremented value of element counter
		*/
		qlonglong incEleIdCounter() { return ++ele_id_counter; } 

		/**
		*  \fn inline private  incLayoutIdCounter
		*  \brief Increments and returns the incremented value of GraphLayout ID counter
		*  \return qlonglong 
		*/
		qlonglong incLayoutIdCounter() { return ++layout_id_counter; } 

		/**
		* \fn	private removeAllEdgesOfType(Data::Type* type)
		* \brief Removes all Edges that are of type
		*  \param      type   Type
		*/
		void removeAllEdgesOfType(Data::Type* type); 

        /**
        * \fn private removeAllNodesOfType(Data::Type* type)
		* \brief Removes all Nodes that are of type
        *  \param      type     Type
        */
        void removeAllNodesOfType(Data::Type* type); 
       

		/**
		*  \fn private  getMaxEleIdFromElements
		*  \brief Returns the maximum value of IDs of all elements of the Graph
		*  \return qlonglong returns maximum ID value of elements
		*/
		qlonglong getMaxEleIdFromElements();

		/**
		*  qlonglong graph_id
		*  \brief ID of the Graph
		*/
        qlonglong graph_id;

		/**
		*	qlonglong parent_id
		*	\brief ID of the parent Node
		*/
		QList<osg::ref_ptr<Data::Node>> parent_id;
        
		/**
		*  QString name
		*  \brief Name of the Graph
		*/
        QString name;
        
		/**
		*  qlonglong ele_id_counter
		*  \brief Element ID counter
		*/
        qlonglong ele_id_counter;
        
		/**
		*  qlonglong layout_id_counter
		*  \brief GraphLayout ID counter
		*/
        qlonglong layout_id_counter;
        
		/**
		*  bool inDB
		*  \brief flag if the Graph is frozen or not (used by layout algorithm)
		*/
        bool inDB;
        

		/**
		*  Data::GraphLayout * selectedLayout
		*  \brief Currently selected GraphLayout
		*/
		Data::GraphLayout* selectedLayout;
        

		/**
		*  QSqlDatabase * conn
		*  \brief Connection to database
		*/
		QSqlDatabase* conn;
        

		/**
		*  QMap<qlonglong,Data::GraphLayout*> layouts
		*  \brief GraphLayouts of the Graph
		*/
		QMap<qlonglong, Data::GraphLayout*> layouts;
        

		/**
		*  QMultiMap<QString,Data::Type*> * typesByName
		*  \brief Types sorted by their name attribute
		*/
		QMultiMap<QString, Data::Type*>* typesByName;
        

		/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Node> > newNodes
		*  \brief New Nodes that have been added to the Graph but are not yet in database
		*/
		QMap<qlonglong, osg::ref_ptr<Data::Node> > newNodes;

		/**
		*  QMap<qlonglong,Data::Type*> newTypes
		*  \brief New Types that have been added to the Graph but are not yet in database
		*/
		QMap<qlonglong, Data::Type*> newTypes;

		/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Edge> > newEdges
		*  \brief New Edges that have been added to the Graph but are not yet in database
		*/
		QMap<qlonglong, osg::ref_ptr<Data::Edge> > newEdges;


		/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Node> > * nodes
		*  \brief Nodes in the Graph
		*/
		QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes;

		/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Edge> > * edges
		*  \brief Edges in the Graph
		*/
		QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges;

		/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Node> > * metaNodes
		*  \brief Meta-Nodes in the Graph
		*/
		QMap<qlonglong, osg::ref_ptr<Data::Node> >* metaNodes;

		/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Edge> > * metaEdges
		*  \brief Meta-Edges in the Graph
		*/
		QMap<qlonglong, osg::ref_ptr<Data::Edge> >* metaEdges;

		/**
		*  QMap<qlonglong,Data::Type*> * types
		*  \brief Types in the Graph
		*/
		QMap<qlonglong, Data::Type*>* types;

		/**
		*  bool frozen
		*  \brief Flag if the Graph is frozen or not (used by layout algorithm)
		*/
		bool frozen;
		
		/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Edge> > edgesByType
		*  \brief Edges in the Graph sorted by their Type
		*/
		QMultiMap<qlonglong, osg::ref_ptr<Data::Edge> > edgesByType;

		/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Node> > nodesByType
		*  \brief Nodes in the Graph sorted by their Type
		*/
		QMultiMap<qlonglong, osg::ref_ptr<Data::Node> > nodesByType;

		/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Edge> > metaEdgesByType
		*  \brief Meta-Edges in the Graph sorted by their Type
		*/
        QMultiMap<qlonglong, osg::ref_ptr<Data::Edge> > metaEdgesByType;

		/**
		*  QMap<qlonglong,osg::ref_ptr<Data::Node> > metaNodesByType
		*  \brief Meta-Nodes in the Graph sorted by their Type
		*/
        QMultiMap<qlonglong, osg::ref_ptr<Data::Node> > metaNodesByType;
	};
}

#endif

/* POZNAMKY

QUERY na vyber stromovej hierarchie uzlov s urcenym ci je uzol meta na zaklade parenta (type_id) ktory je metauzlom

WITH RECURSIVE q AS (
	SELECT node_id, type_id, "name", meta, CAST("name" As varchar(1000)) As fullname
	FROM nodes 
	WHERE node_id = type_id
UNION ALL
	SELECT n.node_id, n.type_id, n."name", (CASE WHEN n.meta OR q.meta THEN true ELSE false END) AS meta, 
	CAST(q.fullname || '->' || n."name" As varchar(1000)) As fullname
	FROM nodes AS n
	INNER JOIN q ON
	n.type_id = q.node_id AND n.type_id != n.node_id
) 
SELECT node_id, type_id, "name", meta, fullname
FROM q
ORDER BY fullname


WITH RECURSIVE q AS (
	SELECT node_id, type_id, "name", meta, layout_id
	FROM nodes 
	WHERE node_id = type_id
UNION ALL
	SELECT n.node_id, n.type_id, n."name", 
	(n.meta OR q.meta) AS meta, 
	COALESCE(n.layout_id, q.layout_id) AS layout_id
	FROM nodes AS n
	INNER JOIN q ON
	n.type_id = q.node_id AND n.type_id != n.node_id
) 
SELECT node_id, type_id, "name", meta, layout_id
FROM q

*/

/*
EXPLAIN ANALYZE SELECT * FROM crosstab(
'SELECT graph_id, val_name, val FROM graph_settings ORDER BY 1,2',
'SELECT val_name FROM graph_settings GROUP BY val_name'
) AS (graph_id bigint, layout_count text,"name" text,file_size text,path text)
*/
