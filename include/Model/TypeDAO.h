/*!
 * TypeDAO.h
 * Projekt 3DVisual
 */
#ifndef MODEL_TYPEDAO_DEF
#define MODEL_TYPEDAO_DEF 1

#include "Data/Type.h"
#include "Data/Graph.h"
#include "Model/GraphDAO.h"

#include <QtSql>
#include <QDebug>

namespace Model {
/**
	*  \class TypeDAO
	*  \brief Class represents a database layer for Data::Type objects
	*  \author Aurel Paulovic
	*  \date 29. 4. 2010
	*/
class TypeDAO
{
public:

	/**
		*  \fn public static  checkIfExists(Data::Type* type, QSqlDatabase* conn)
		*  \brief Checks if the Type exists in database
		*  \param type    Type
		*  \param conn     connection to the database
		*  \return bool true, if the Type is in database
		*/
	static bool checkIfExists(Data::Type* type, QSqlDatabase* conn);


	/**
		*  \fn public static overloaded  addType(QString type_name, Data::Graph* graph, QSqlDatabase* conn, QMap<QString, QString> * settings = 0)
		*  \brief Creates new Type, assigns it to the Graph and adds it to the database
		*  \param  type_name     name of the Type
		*  \param  graph   Graph
		*  \param  conn     connection to the database
		*  \param  settings     settings of the Type
		*  \return Data::Type * new Type
		*/
	static Data::Type* addType(QString type_name, Data::Graph* graph, QSqlDatabase* conn, QMap<QString, QString> * settings = 0);


	/**
		*  \fn public static overloaded  addType(Data::Type* type, QSqlDatabase* conn)
		*  \brief Adds Type to the database
		*  \param  type     Type
		*  \param conn     connection to the database
		*  \return bool true, if the Type was successfully added to the database
		*/
	static bool addType(Data::Type* type, QSqlDatabase* conn);


	/**
		*  \fn public static  removeType(Data::Type* type, QSqlDatabase* conn)
		*  \brief Removes Type from the database
		*  \param   type    Type
		*  \param   conn     connection to the database
		*  \return bool true, if the Type was successfully removed from the database
		*/
	static bool removeType(Data::Type* type, QSqlDatabase* conn);

	/**
		* \fn public static getSettings(Data::Type* type, QSqlDatabase* conn, bool* error)
		* \brief Returns setting of the Type
		* \param  graph 	Type
		* \param  conn 		connection to the database
		* \param  error 	error flag, will be set to true, if the method encounters an error
		* \return QMap<QString,QString> settings of the Type
		*/
	static QMap<QString,QString> getSettings(Data::Type* type, QSqlDatabase* conn, bool* error);

private:

	/**
		*  \fn private constructor  TypeDAO
		*  \brief Constructs TypeDAO object
		*
		* Only static members of the class should be used
		*/
	TypeDAO(void);

	/**
		*  \fn private destructor  ~TypeDAO
		*  \brief Destroys TypeDAO object
		*/
	~TypeDAO(void);
};
}
#endif
