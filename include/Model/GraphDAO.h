/*!
 * GraphDAO.h
 * Projekt 3DVisual
 */
#ifndef MODEL_GRAPHDAO_DEF
#define MODEL_GRAPHDAO_DEF 1

#include <QtSql>
#include <QString>
#include <QMap>


namespace Data
{
class Graph;
}

namespace Model
{
/**
	*  \class GraphDAO
	*  \brief Class represents a database layer for Data::Graph objects
	*  \author Aurel Paulovic
	*  \date 29. 4. 2010
	*/
class GraphDAO
{
public:


	/**
		*  \fn public static  getGraphs(QSqlDatabase* conn, bool* error )
		*  \brief Returns QMap of graphs in the database
		*  \param   conn QSqlDatabase *    connection to the database
		*  \param  error    error flag, will be set to true, if the method encounters an error
		*  \return QMap<qlonglong,Data::Graph*> graphs in database
		*/
	static QMap<qlonglong, Data::Graph*> getGraphs(QSqlDatabase* conn, bool* error );

	/**
		*  \fn public static  getGraph(QSqlDatabase* conn, bool* error, qlonglong graphID)
		*  \brief	Return graph from DB
		*  \param   conn   connection to the database
		*  \param   error  error flag, will be set to true, if the method encounters an error
		*  \param   graphID  graph ID
		*  \param   layoutID  layout ID
		*  \return	Data::Graph* graph from DB
		*/
	static Data::Graph* getGraph(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID);

	/**
		*  \fn public static overloaded  addGraph(QString graph_name, QSqlDatabase* conn)
		*  \brief Adds new Graph to the database and returns it
		*  \param   graph_name     name of the Graph
		*  \param   conn     connection to the database
		*  \return Data::Graph * new Graph
		*/
	static Data::Graph* addGraph(QString graph_name, QSqlDatabase* conn);


	/**
		*  \fn public static overloaded  addGraph(Data::Graph* graph, QSqlDatabase* conn)
		*  \brief Adds Graph to the database
		*  \param  graph     Graph to be added to database
		*  \param  conn    connection to the database
		*  \return bool true, if the Graph was successfully added to database
		*/
	static bool addGraph(Data::Graph* graph, QSqlDatabase* conn);

	/**
		*  \fn public static setGraphName(qlonglong graphID, QString graphName, QSqlDatabase* conn)
		*  \brief Rename Graph in the database
		*  \param  graphID     Graph ID
		*  \param  graphName     Graph name
		*  \param  conn    connection to the database
		*  \return bool true, if the Graph was successfully renamed in the database
		*/
	static bool setGraphName(qlonglong graphID, QString graphName, QSqlDatabase* conn);

	/**
		*  \fn public static  removeGraph(Data::Graph* graph, QSqlDatabase* conn)
		*  \brief Removes graph from the database
		*  \param   graph     Graph to be removed from database
		*  \param   conn   connection to the database
		*  \return bool true, if the Graph was successfully removed
		*/
	static bool removeGraph(Data::Graph* graph, QSqlDatabase* conn);

	/**
		*  \fn public static  removeGraph(qlonglong graphID, QSqlDatabase* conn)
		*  \brief Removes graph from the database
		*  \param   graphID     ID of graph to be removed from database
		*  \param   conn   connection to the database
		*  \return bool true, if the Graph was successfully removed
		*/
	static bool removeGraph(qlonglong graphID, QSqlDatabase* conn);

	/**
		*  \fn public static  getName(qlonglong graphID, bool* error, QSqlDatabase* conn)
		*  \brief Get name of graph from database
		*  \param   graphID     graph ID
		*  \param   error    error
		*  \param   conn     connection to the database
		*  \return QString name of the graph
		*/
	static QString getName(qlonglong graphID, bool* error, QSqlDatabase* conn);

	/**
		*  \fn public static  setName(QString name, Data::Graph* graph, QSqlDatabase* conn)
		*  \brief Sets new name to the Graph in database
		*  \param   name     new name
		*  \param   graph    Graph
		*  \param   conn     connection to the database
		*  \return QString resultant name of the Graph
		*/
	static QString setName(QString name, Data::Graph* graph, QSqlDatabase* conn);

	/**
		* \fn public static getSettings(Data::Graph* graph, QSqlDatabase* conn, bool* error)
		* \brief Returns setting of the Graph
		* \param  graph 		Graph
		* \param  conn  		connection to the database
		* \param  error		error flag, will be set to true, if the method encounters an error
		* \return QMap<QString,QString> settings of the Graph
		*/
	static QMap<QString,QString> getSettings(Data::Graph* graph, QSqlDatabase* conn, bool* error);

	/**
	  *
	  *
	  */

	static void getNestedGraph(qlonglong parentID, Data::Graph** graph, QSqlDatabase* conn, bool* error2, qlonglong graphID, qlonglong layoutID, qlonglong* maxIdEleUsed, QMap<qlonglong, osg::Vec3f>* positions, QMap<qlonglong, Data::Node*>* nodes, Data::Type* typeNode, Data::Type* typeMetaNode, QList<qlonglong>* parentNodes);


private:

	/**
		*  \fn private constructor  GraphDAO
		*  \brief Constructs GraphDAO object
		*
		* Only static members of the class should be used
		*/
	GraphDAO(void);

	/**
		*  \fn private destructor  ~GraphDAO
		*  \brief Destroys GraphDAO object
		*/
	~GraphDAO(void);
};
}
#endif
