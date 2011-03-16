/*!
 * EdgeDAO.h
 * Projekt 3DVisual
 */
#ifndef MODEL_EDGEDAO_DEF
#define MODEL_EDGEDAO_DEF 1

#include "Data/Edge.h"

#include <QtSql>
#include <QDebug>

namespace Data {
	class Edge;
}

namespace Model 
{
	/**
	*  \class EdgeDAO
	*  \brief Class represents a database layer for Data::Edge objects
	*  \author Aurel Paulovic
	*  \date 29. 4. 2010
	*/
	class EdgeDAO
    {
    public:

		/**
		*  \fn public static  addEdgesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edgess)
		*  \brief	Add edges to DB
		*  \param   conn   connection to the database 
		*  \param   edges  edges from actual graph
		*  \param   isMeta  if edges from actual graph are meta
		*  \return	bool true, if edges were successfully added to DB
		*/
		//static bool addEdgesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges, bool isMeta);

		/**
		*  \fn public static  addEdgesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edgess)
		*  \brief	Add edges to DB
		*  \param   conn   connection to the database 
		*  \param   edges  edges from actual graph
		*  \return	bool true, if edges were successfully added to DB
		*/
		static bool addEdgesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges);

		/**
		*  \fn public static  addEdgesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edgess)
		*  \brief	Add edges to DB
		*  \param   conn   connection to the database 
		*  \param   edges  edges from actual graph
		*  \param   layout  layout from actual graph
		*  \return	bool true, if edges were successfully added to DB
		*/
		static bool addMetaEdgesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges, Data::GraphLayout* layout);

		/**
		*  \fn public static  getEdgesQuery(QSqlDatabase* conn, bool* error, qlonglong graphID)
		*  \brief	Return edges query from DB
		*  \param   conn   connection to the database 
		*  \param   error  error flag, will be set to true, if the method encounters an error
		*  \param   graphID  graph ID
		*  \return	QSqlQuery* edges query from DB
		*/
		static QSqlQuery* getEdgesQuery(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID);

		/**
		*  \fn public static  getNumberOfEdges(QSqlDatabase* conn, bool* error)
		*  \brief	Return list of edges - graph ID
		*  \param   conn   connection to the database 
		*  \param   error  error flag, will be set to true, if the method encounters an error
		*  \return	qlonglong number of edges in DB in graph
		*/
		static QList<qlonglong> getListOfEdges(QSqlDatabase* conn, bool* error);

		/**
		*  \fn public static  checkIfExists(Data::Edge* edge, QSqlDatabase* conn)
		*  \brief Checks if the Edge exists in database
		*  \param   edge     Edge
		*  \param   conn     connection to the database
		*  \return bool true, if the Edge is in database
		*/
		static bool checkIfExists(Data::Edge* edge, QSqlDatabase* conn);
        

		/**
		*  \fn public static  removeEdge(Data::Edge* edge, QSqlDatabase* conn)
		*  \brief Removes the Edge from database
		*  \param   edge    Edge
		*  \param   conn     connection to the database
		*  \return bool true, if the Edge was removed from database
		*/
		static bool removeEdge(Data::Edge* edge, QSqlDatabase* conn);
        
		/**
        * \fn public static getSettings(Data::Edge* edge, QSqlDatabase* conn, bool* error)
        * \brief Returns setting of the Edge
        * \param  graph 		edge
        * \param  conn  		connection to the database	
        * \param  error 		error flag, will be set to true, if the method encounters an error
        * \return QMap<QString,QString> settings of the Edge
        */
        static QMap<QString,QString> getSettings(Data::Edge* edge, QSqlDatabase* conn, bool* error);
    private:

		/**
		*  \fn private constructor  EdgeDAO
		*  \brief Constructs EdgeDAO object
		* 
		* Only static members of the class should be used
		*/
		EdgeDAO(void);

		/**
		*  \fn private destructor  ~EdgeDAO
		*  \brief Destroys EdgeDAO object
		*/
		~EdgeDAO(void);
    };
}
#endif
