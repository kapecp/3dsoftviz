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