/*!
 * NodeDAO.h
 * Projekt 3DVisual
 */
#ifndef MODEL_NODEDAO_DEF
#define MODEL_NODEDAO_DEF 1


#include "Data/Node.h"
#include <QtSql>

namespace Data {
class Node;
class GraphLayout;
}

namespace Model {
/**
        *  \class NodeDAO
        *  \brief Class represents a database layer for Data::Node objects
        *  \author Aurel Paulovic
        *  \date 29. 4. 2010
        */
class NodeDAO
{
public:

	/**
	    *  \fn public static  addNodesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes)
	    *  \brief	Add nodes to DB
	    *  \param   conn   connection to the database
	    *  \param   nodes  nodes from actual graph
	    *  \return	bool true, if nodes were successfully added to DB
	    */
	static bool addNodesToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes );

	/**
	    *  \fn public static  addMetaNodesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID)
	    *  \brief	Add nodes to DB
	    *  \param   conn   connection to the database
	    *  \param   nodes  nodes from actual graph
	    *  \param   layout  layout from actual graph
	    *  \param	newMetaNodeID	new ID of meta nodes (because of unique ID in DB)
	    *  \return	bool true, if nodes were successfully added to DB
	    */
	static bool addMetaNodesToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID );

	/**
	    *  \fn public static  addNodesPositionsToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta)
	    *  \brief	Add nodes positions to DB
	    *  \param   conn   connection to the database
	    *  \param   nodes  nodes from actual graph
	    *  \param   layout  layout from actual graph
	    *  \param	newMetaNodeID	new ID of meta nodes (because of unique ID in DB)
	    *  \param	meta	true, if nodes are meta type
	    *  \return	bool true, if nodes were successfully added to DB
	    */
	static bool addNodesPositionsToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta );

	/**
	    *  \fn public static  addNodesColorToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta)
	    *  \brief	Add color of nodes to DB
	    *  \param   conn   connection to the database
	    *  \param   nodes  nodes from actual graph
	    *  \param   layout  layout from actual graph
	    *  \param	newMetaNodeID	new ID of meta nodes (because of unique ID in DB)
	    *  \param	meta	true, if nodes are meta type
	    *  \return	bool true, if color of nodes was successfully added to DB
	    */
	static bool addNodesColorToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta );

	/**
	    *  \fn public static  addNodesScaleToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta, float defaultScale)
	    *  \brief	Add scale of nodes to DB
	    *  \param   conn   connection to the database
	    *  \param   nodes  nodes from actual graph
	    *  \param   layout  layout from actual graph
	    *  \param	newMetaNodeID	new ID of meta nodes (because of unique ID in DB)
	    *  \param	meta	true, if nodes are meta type
	    *  \param	defaultScale	default size of nodes in graph
	    *  \return	bool true, if scale of nodes was successfully added to DB
	    */
	static bool addNodesScaleToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta, float defaultScale );

	/**
	    *  \fn public static  addNodesMaskToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta)
	    *  \brief	Add scale of nodes to DB
	    *  \param   conn   connection to the database
	    *  \param   nodes  nodes from actual graph
	    *  \param   layout  layout from actual graph
	    *  \param	newMetaNodeID	new ID of meta nodes (because of unique ID in DB)
	    *  \param	meta	true, if nodes are meta type
	    *  \return	bool true, if scale of nodes was successfully added to DB
	    */
	static bool addNodesMaskToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta );

	/**
	    *  \fn public static  addNodesParentToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta)
	    *  \brief	Add parent attribute of nodes to DB
	    *  \param   conn   connection to the database
	    *  \param   nodes  nodes from actual graph
	    *  \param   layout  layout from actual graph
	    *  \param	newMetaNodeID	new ID of meta nodes (because of unique ID in DB)
	    *  \param	meta	true, if nodes are meta type
	    *  \return	bool true, if attribute of nodes was successfully added to DB
	    */
	static bool addNodesParentToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta );

	/**
	    *  \fn public static  getNodesQuery(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID, qlonglong parentID)
	    *  \brief	Return nodes query from DB
	    *  \param   conn   connection to the database
	    *  \param   error  error flag, will be set to true, if the method encounters an error
	    *  \param   graphID  graph ID
	    *  \param   layoutID  layout ID
	    *  \param   parentID  parent ID
	    *  \return	QSqlQuery* nodes query from DB
	    */
	static QSqlQuery* getNodesQuery( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID, qlonglong parentID );

	/**
	    *  \fn public static  getNodesPositions(QSqlDatabase* conn, bool* error, qlonglong graphID)
	    *  \brief	Return nodes positions map from DB
	    *  \param   conn   connection to the database
	    *  \param   error  error flag, will be set to true, if the method encounters an error
	    *  \param   graphID  graph ID
	    *  \param   layoutID  layout ID
	    *  \return	QMap<qlonglong, osg::Vec3f> nodes positions map from DB
	    */
	static QMap<qlonglong, osg::Vec3f> getNodesPositions( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID );

	/**
	    *  \fn public static  getNumberOfNodes(QSqlDatabase* conn, bool* error)
	    *  \brief	Return list of nodes - graph ID
	    *  \param   conn   connection to the database
	    *  \param   error  error flag, will be set to true, if the method encounters an error
	    *  \return	QList<qlonglong> list of nodes in DB in graph
	    */
	static QList<qlonglong> getListOfNodes( QSqlDatabase* conn, bool* error );

	/**
	    *  \fn public static  checkIfExists(Data::Node* node, QSqlDatabase* conn)
	    *  \brief Checks if the Node exists in database
	    *  \param   node   Node
	    *  \param   conn   connection to the database
	    *  \return bool true, if the Node is in database
	    */
	static bool checkIfExists( Data::Node* node, QSqlDatabase* conn );


	/**
	    *  \fn public static  removeNode(Data::Node* node, QSqlDatabase* conn)
	    *  \brief Removes Node from the database
	    *  \param  node   Node
	    *  \param  conn   connection to the database
	    *  \return bool true, if the Node was successfully removed from the database
	    */
	static bool removeNode( Data::Node* node, QSqlDatabase* conn );

	/**
	    *  \fn public static  removeNodes(qlonglong graphID, QSqlDatabase* conn)
	    *  \brief Removes Nodes from the database
	    *  \param  graphID   ID of graph
	    *  \param  conn   connection to the database
	    *  \return bool true, if the Nodes ware successfully removed from the database
	    */
	static bool removeNodes( qlonglong graphID, QSqlDatabase* conn );

	/**
	    *  \fn public static  removeNodes(qlonglong graphID, qlonglong layoutID, QSqlDatabase* conn)
	    *  \brief Removes Nodes from the database
	    *  \param  graphID   ID of graph
	    *  \param  layoutID   ID of layout
	    *  \param  conn   connection to the database
	    *  \return bool true, if the Nodes ware successfully removed from the database
	    */
	static bool removeNodes( qlonglong graphID, qlonglong layoutID, QSqlDatabase* conn );

	/**
	    * \fn public static getSettings(Data::Node* node, QSqlDatabase* conn, bool* error)
	    * \brief Returns setting of the Node
	    * \param  graph	Node
	    * \param  conn  connection to the database
	    * \param  error	error flag, will be set to true, if the method encounters an error
	    * \return QMap<QString,QString> settings of the Node
	    */
	static QMap<QString,QString> getSettings( Data::Node* node, QSqlDatabase* conn, bool* error );

	/**
	    *  \fn public static  getSettings(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID, QString attributeName)
	    *  \brief	Return nodes settings map from DB
	    *  \param   conn   connection to the database
	    *  \param   error  error flag, will be set to true, if the method encounters an error
	    *  \param   graphID  graph ID
	    *  \param   layoutID  layout ID
	    *  \param   attributeName  name of node attribute
	    *  \return	QMap<qlonglong, QString> settings of the Node
	    */
	static QMap<qlonglong, QString> getSettings( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID, QString attributeName );

	/**
	    *  \fn public static  getColors(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID, QString attributeName)
	    *  \brief	Return nodes colors map from DB
	    *  \param   conn   connection to the database
	    *  \param   error  error flag, will be set to true, if the method encounters an error
	    *  \param   graphID  graph ID
	    *  \param   layoutID  layout ID
	    *  \return	QMap<qlonglong, osg::Vec4f> colors of the Nodes
	    */
	static QMap<qlonglong, osg::Vec4f> getColors( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID );

	/**
	    *  \fn public static  getScales(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID)
	    *  \brief	Return nodes scales map from DB
	    *  \param   conn   connection to the database
	    *  \param   error  error flag, will be set to true, if the method encounters an error
	    *  \param   graphID  graph ID
	    *  \param   layoutID  layout ID
	    *  \return	QMap<qlonglong, float> scales of the Nodes
	    */
	static QMap<qlonglong, float> getScales( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID );

	/**
	    *  \fn public static  getMasks(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID)
	    *  \brief	Return masks scales map from DB
	    *  \param   conn   connection to the database
	    *  \param   error  error flag, will be set to true, if the method encounters an error
	    *  \param   graphID  graph ID
	    *  \param   layoutID  layout ID
	    *  \return	QMap<qlonglong, int> masks of the Nodes
	    */
	static QMap<qlonglong, int> getMasks( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID );

	/**
	    *  \fn public static  getParents(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID)
	    *  \brief	Return list of parent nodes ID
	    *  \param   conn   connection to the database
	    *  \param   error  error flag, will be set to true, if the method encounters an error
	    *  \param   graphID  graph ID
	    *  \param   layoutID  layout ID
	    *  \return	QList<qlonglong> list of parent nodes
	    */
	static QList<qlonglong> getParents( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID );

	/**
	    *  \fn public static  getNewMetaNodeId(QSqlDatabase* conn, qlonglong graphID, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes)
	    *  \brief	Return map of new nodes ID
	    *  \param   conn   connection to the database
	    *  \param   graphID  graph ID
	    *  \param	nodes nodes of graph
	    *  \return	QMap<qlonglong, qlonglong> new nodes ID
	    */
	static QMap<qlonglong, qlonglong> getNewMetaNodesId( QSqlDatabase* conn, qlonglong graphID, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes );

	/**
	    *  \fn public  static addSetings(QSqlDatabase* conn, qlonglong graphID, qlonglong layoutID, qlonglong nodeID, QString valName, double val)
	    *  \brief	Add node setting to DB
	    *  \param   conn   connection to the database
	    *  \param   graphID  ID of graph
	    *  \param   layoutID  ID of layout
	    *  \param   nodeID  ID of node
	    *  \param   valName  name of value
	    *  \param   val  value
	    *  \return	bool true, if node setting was successfully added to DB
	    */
	static bool addSettings( QSqlDatabase* conn, qlonglong graphID, qlonglong layoutID, qlonglong nodeID, QString valName, double val );

	/**
	    *  \fn public  static addSetings(QSqlDatabase* conn, qlonglong graphID, qlonglong layoutID, qlonglong nodeID, QString valName, float val)
	    *  \brief	Add node setting to DB
	    *  \param   conn   connection to the database
	    *  \param   graphID  ID of graph
	    *  \param   layoutID  ID of layout
	    *  \param   nodeID  ID of node
	    *  \param   valName  name of value
	    *  \param   val  value
	    *  \return	bool true, if node setting was successfully added to DB
	    */
	static bool addSettings( QSqlDatabase* conn, qlonglong graphID, qlonglong layoutID, qlonglong nodeID, QString valName, float val );

	/**
	    *  \fn public  static addSetings(QSqlDatabase* conn, qlonglong graphID, qlonglong layoutID, qlonglong nodeID, QString valName, bool val)
	    *  \brief	Add node setting to DB
	    *  \param   conn   connection to the database
	    *  \param   graphID  ID of graph
	    *  \param   layoutID  ID of layout
	    *  \param   nodeID  ID of node
	    *  \param   valName  name of value
	    *  \param   val  value
	    *  \return	bool true, if node setting was successfully added to DB
	    */
	static bool addSettings( QSqlDatabase* conn, qlonglong graphID, qlonglong layoutID, qlonglong nodeID, QString valName, bool val );

private:

	/**
	    *  \fn private constructor  NodeDAO
	    *  \brief Constructs NodeDAO object
	    *
	    * Only static members of the class should be used
	    */
	NodeDAO( void );

	/**
	    *  \fn private destructor  ~NodeDAO
	    *  \brief Destroys NodeDAO object
	    */
	~NodeDAO( void );
};
}
#endif
