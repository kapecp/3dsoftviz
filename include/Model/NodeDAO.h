/*!
 * NodeDAO.h
 * Projekt 3DVisual
 */
#ifndef MODEL_NODEDAO_DEF
#define MODEL_NODEDAO_DEF 1

#include "Data/GraphLayout.h"
#include "Data/Graph.h"
#include "Data/Node.h"

#include <QtSql>
#include <QDebug>

namespace Data {
	class Node;
}

namespace Model
{
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
		*  \param   isMeta  if nodes are meta or not
		*  \param   layout  layout from actual graph
		*  \return	bool true, if nodes were successfully added to DB
		*/
		static bool addNodesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, bool isMeta, Data::GraphLayout* layout);

		/**
		*  \fn public static  getNodesQuery(QSqlDatabase* conn, bool* error, qlonglong graphID)
		*  \brief	Return nodes query from DB
		*  \param   conn   connection to the database 
		*  \param   error  error flag, will be set to true, if the method encounters an error
		*  \param   graphID  graph ID
		*  \return	QSqlQuery* nodes query from DB
		*/
		static QSqlQuery* getNodesQuery(QSqlDatabase* conn, bool* error, qlonglong graphID);

		/**
		*  \fn public static  getNodesPositionsQuery(QSqlDatabase* conn, bool* error, qlonglong graphID)
		*  \brief	Return nodes positions query from DB
		*  \param   conn   connection to the database 
		*  \param   error  error flag, will be set to true, if the method encounters an error
		*  \param   graphID  graph ID
		*  \return	QSqlQuery* nodes positions query from DB
		*/
		static QSqlQuery* getNodesPositionsQuery(QSqlDatabase* conn, bool* error, qlonglong graphID);

		/**
		*  \fn public static  getNumberOfNodes(QSqlDatabase* conn, bool* error)
		*  \brief	Return list of nodes - graph ID
		*  \param   conn   connection to the database 
		*  \param   error  error flag, will be set to true, if the method encounters an error
		*  \return	qlonglong number of nodes in DB in graph
		*/
		static QList<qlonglong> getListOfNodes(QSqlDatabase* conn, bool* error);

		/**
		*  \fn public static  checkIfExists(Data::Node* node, QSqlDatabase* conn)
		*  \brief Checks if the Node exists in database
		*  \param   node   Node 
		*  \param   conn   connection to the database
		*  \return bool true, if the Node is in database
		*/
		static bool checkIfExists(Data::Node* node, QSqlDatabase* conn);
        

		/**
		*  \fn public static  removeNode(Data::Node* node, QSqlDatabase* conn)
		*  \brief Removes Node from the database
		*  \param  node   Node
		*  \param  conn   connection to the database
		*  \return bool true, if the Node was successfully removed from the database
		*/
		static bool removeNode(Data::Node* node, QSqlDatabase* conn);

		/**
        * \fn public static getSettings(Data::Node* node, QSqlDatabase* conn, bool* error)
        * \brief Returns setting of the Node
        * \param  graph	Node
        * \param  conn  connection to the database	
        * \param  error	error flag, will be set to true, if the method encounters an error
        * \return QMap<QString,QString> settings of the Node
        */
        static QMap<QString,QString> getSettings(Data::Node* node, QSqlDatabase* conn, bool* error);
    private:

		/**
		*  \fn private constructor  NodeDAO
		*  \brief Constructs NodeDAO object
		* 
		* Only static members of the class should be used
		*/
		NodeDAO(void);

		/**
		*  \fn private destructor  ~NodeDAO
		*  \brief Destroys NodeDAO object
		*/
		~NodeDAO(void);
    };
}
#endif
