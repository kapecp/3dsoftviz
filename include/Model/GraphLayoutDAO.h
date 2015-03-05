/*!
 * GraphLayoutDAO.h
 * Projekt 3DVisual
 */
#ifndef MODEL_GRAPHLAYOUTDAO_DEF
#define MODEL_GRAPHLAYOUTDAO_DEF 1

#include <QtSql>
#include <QString>
#include <QMap>

namespace Data {

class Graph;
class GraphLayout;
}

namespace Model {
/**
		*  \class GraphLayoutDAO
		*  \brief Class represents a database layer for Data::GraphLayout objects
		*  \author Aurel Paulovic
		*  \date 29. 4. 2010
		*/
class GraphLayoutDAO
{
public:

	/**
		*  \fn public static  getLayoutsNames(qlonglong graph_id, QSqlDatabase* conn, bool* error)
		*  \brief Returns QMap of layouts names assigned to the Graph in the database
		*  \param  graph_id    Graph ID
		*  \param  conn    connection to the database
		*  \param  error  error flag, will be set to true, if the method encounters an error
		*  \return QMap<qlonglong,QString> layouts names of the Graph
		*/
	static QMap<qlonglong, QString> getLayoutsNames( qlonglong graph_id, QSqlDatabase* conn, bool* error );

	/**
		*  \fn public static  getListOfLayouts(QSqlDatabase* conn, bool* error)
		*  \brief	Return list of layouts - graph ID
		*  \param   conn   connection to the database
		*  \param   error  error flag, will be set to true, if the method encounters an error
		*  \return	qlonglong number of layouts in DB for graph
		*/
	static QList<qlonglong> getListOfLayouts( QSqlDatabase* conn, bool* error );

	/**
		*  \fn public static  getLayouts(Data::Graph* graph, QSqlDatabase* conn, bool* error)
		*  \brief Returns QMap of GraphLayotus assigned to the Graph in the database
		*  \param  graph    Graph
		*  \param  conn    connection to the database
		*  \param  error  error flag, will be set to true, if the method encounters an error
		*  \return QMap<qlonglong,Data::GraphLayout*> GraphLayouts of the Graph
		*/
	static QMap<qlonglong, Data::GraphLayout*> getLayouts( Data::Graph* graph, QSqlDatabase* conn, bool* error );

	/**
		*  \fn public static overloaded  addLayout(QString layout_name, Data::Graph* graph, QSqlDatabase* conn)
		*  \brief Creates new GraphLayout, assigns it to the Graph and adds it into the database
		*  \param  layout_name  name of the GraphLayout
		*  \param  graph     Graph
		*  \param  conn     connection to the database
		*  \return Data::GraphLayout * new GraphLayout
		*/
	static Data::GraphLayout* addLayout( QString layout_name, Data::Graph* graph, QSqlDatabase* conn );


	/**
		*  \fn public static overloaded  addLayout(Data::GraphLayout* layout, QSqlDatabase* conn)
		*  \brief Adds GraphLayout to the database
		*  \param  layout Data::GraphLayout *    GraphLayout
		*  \param  conn QSqlDatabase *    connection to the database
		*  \return bool true, if the GraphLayaout was added to the database
		*/
	static bool addLayout( Data::GraphLayout* layout, QSqlDatabase* conn );


	/**
		*  \fn public static  removeLayout(Data::GraphLayout* graphLayout, QSqlDatabase* conn)
		*  \brief Removes Graphlayout from the databse
		*  \param  graphLayout    GraphLayout
		*  \param  conn   connection to the database
		*  \return bool true, if the GraphLayout was successfully removed from the database
		*/
	static bool removeLayout( Data::GraphLayout* graphLayout, QSqlDatabase* conn );

	/**
		*  \fn public static  removeLayout(qlonglong graphID, QSqlDatabase* conn)
		*  \brief Removes Graphlayout from the databse
		*  \param  graphID    ID of graph
		*  \param  conn   connection to the database
		*  \return bool true, if the GraphLayout was successfully removed from the database
		*/
	static bool removeLayouts( qlonglong graphID, QSqlDatabase* conn );

	/**
		*  \fn public static  removeLayout(qlonglong graphID, qlonglong layoutID, QSqlDatabase* conn)
		*  \brief Removes Graphlayout from the databse
		*  \param  graphID    ID of graph
		*  \param  layoutID    ID of layout
		*  \param  conn   connection to the database
		*  \return bool true, if the GraphLayout was successfully removed from the database
		*/
	static bool removeLayout( qlonglong graphID, qlonglong layoutID, QSqlDatabase* conn );

	/**
		*  \fn public static  getName(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID)
		*  \brief Gets name of GraphLayout from database
		*  \param  conn    connection to the database
		*  \param  error  error flag, will be set to true, if the method encounters an error
		*  \param  graphID     graph ID
		*  \param  layoutID    layout ID
		*  \return QString name of the GraphLayout
		*/
	static QString getName( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID );


	/**
		*  \fn public static  setName(QString name, Data::GraphLayout* graphLayout, QSqlDatabase* conn)
		*  \brief Sets new name to the GraphLayout in database
		*  \param    name     new name
		*  \param   graphLayout    GraphLayout
		*  \param   conn    connection to the database
		*  \return QString resultant name of the GraphLayout
		*/
	static QString setName( QString name, Data::GraphLayout* graphLayout, QSqlDatabase* conn );


	/**
		*  \fn public static  checkIfExists(Data::GraphLayout* graphLayout, QSqlDatabase* conn)
		*  \brief Checks if the GraphLayout is in database
		*  \param   graphLayout     GraphLayout
		*  \param   conn     connection to the database
		*  \return bool true, if the GraphLayout is in database
		*/
	static bool checkIfExists( Data::GraphLayout* graphLayout, QSqlDatabase* conn );

	/**
		* \fn public static getSettings(Data::GraphLayout* graphLayout, QSqlDatabase* conn, bool* error)
		* \brief Returns setting of the GraphLayout
		* \param  graph 	GraphLayout
		* \param  conn  	connection to the database
		* \param  error  	error flag, will be set to true, if the method encounters an error
		* \return QMap<QString,QString> settings of the GraphLayout
		*/
	static QMap<QString,QString> getSettings( Data::GraphLayout* graphLayout, QSqlDatabase* conn, bool* error );
private:

	/**
		*  \fn private constructor  GraphLayoutDAO
		*  \brief Constructs GraphLayoutDAO object
		*
		* Only static members of the class should be used
		*/
	GraphLayoutDAO( void );

	/**
		*  \fn private destructor  ~GraphLayoutDAO
		*  \brief Destroys GraphLayoutDAO object
		*/
	~GraphLayoutDAO( void );
};
}
#endif
