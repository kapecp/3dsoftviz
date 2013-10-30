/*!
 * DB.cpp
 * Projekt 3DVisual
 */
#include "Model/DB.h"

Model::DB::DB()
{
    this->appConf = Util::ApplicationConfig::get();
    
    DB::openConnection(
    	appConf->getValue("Model.DB.HostName"),
        appConf->getValue("Model.DB.DbName"),
        appConf->getValue("Model.DB.UserName"),
        appConf->getValue("Model.DB.Pass"),
        appConf->getBoolValue("Model.DB.RequireSSL", true)
    );
}

Model::DB::~DB()
{
    DB::closeConnection();
}

void Model::DB::closeConnection()
{
    if(conn.open()) {
        conn.close();
        qDebug() << "[Model::DB::closeConnection] Database connection closed.";
    }
}

bool Model::DB::openConnection(QString host_name, QString db_name, QString user_name, QString pass, bool requireSSL)
{
    if(conn.isOpen()) {
        qDebug() << "[Model::DB::openConnection] Database connection already open.";
        return false;
    }
    if(conn.database().isValid()) {
        conn.removeDatabase(conn.connectionName());
    }

    conn = QSqlDatabase::addDatabase("QPSQL");

    conn.setHostName(host_name);
    conn.setDatabaseName(db_name);
    conn.setUserName(user_name);
    conn.setPassword(pass);

    if (requireSSL) {
    	//conn.setConnectOptions("requiressl=1");
    }

	QString timeout;
	timeout="connect_timeout=5";
	conn.setConnectOptions(timeout);

    if(!conn.open()) {
        qDebug() << "[Model::DB::openConnection] Could not establish database connection: " << conn.lastError().databaseText();
        return false;
    } else {
        qDebug() << "[Model::DB::openConnection] Opened DB connection to " << db_name << "@" << host_name << " as " << user_name;
        return true;
    }
}
