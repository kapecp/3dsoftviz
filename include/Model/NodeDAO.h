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
