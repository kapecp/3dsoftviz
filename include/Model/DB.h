/*!
 * DB.h
 * Projekt 3DVisual
 */
#ifndef Model_DB_DEF
#define Model_DB_DEF 1

#include <QtSql>



namespace Util
{
	class ApplicationConfig;
}

namespace Model
{
/**
	*  \class DB
	*  \brief Class represents connection to the database.
	*  \author Aurel Paulovic
	*  \date 29. 4. 2010
	*/
class DB
{

public:

	/**
		*  \fn public constructor  DB
		*  \brief Constructs DB object and opens database connection
		*/
	DB();

	/**
		*  \fn public destructor  ~DB
		*  \brief Destroys DB object
		*/
	~DB();

	/**
		* \fn public openConnection(QString host_name, QString db_name, QString user_name, QString pass)
		* \brief  Opens new database connection
		* \param  host_name host
		* \param  db_name 	database name
		* \param  user_name user name
		* \param  pass 		password
		* \param  requireSSL 		force ssl
		* \return bool true ak sa podarilo otvorit spojenie, false ak nie.
		*/
	bool openConnection(QString host_name, QString db_name, QString user_name, QString pass, bool requireSSL);

	/**
		* \fn public closeConnection
		* \brief Closes current database connection
		*/
	void closeConnection();

	/**
	 * @author Autor:Dávid Durčák
	 * \fn createConnection
	 * \brief Create connection only one time in first call.
	 */
	bool createConnection();

	/**
	 * @author Autor:Dávid Durčák
	 * \fn tmpGetConn
	 * \brief Create and return connection do database
	 */
	QSqlDatabase* tmpGetConn();

private:
	/**
		*	QSqlDatabase conn
		*	\brief Database connection
		*/
	QSqlDatabase conn;

	/**
		*	Util::ApplicationConfig * appConf
		*	\brief Application config
		*/
	Util::ApplicationConfig * appConf;


	bool mConneCreationTried;   // false if createConnection was not called yet

};
}

#endif
