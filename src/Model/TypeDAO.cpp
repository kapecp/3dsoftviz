/*!
 * TypeDAO.cpp
 * Projekt 3DVisual
 */
#include "Model/TypeDAO.h"

#include "Model/GraphDAO.h"
#include "Model/GraphLayoutDAO.h"

#include "Data/Graph.h"
#include "Data/Type.h"
#include "Data/GraphLayout.h"

#include <QDebug>

Model::TypeDAO::TypeDAO( void )
{
}

Model::TypeDAO::~TypeDAO( void )
{
}

bool Model::TypeDAO::checkIfExists( Data::Type* type, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::TypeDAO::checkIfExists] Connection to DB not opened.";
		return NULL;
	}
	else if ( type==NULL ) {
		qDebug() << "[Model::TypeDAO::checkIfExists] Invalid parameter - type is NULL.";
		return false;
	}
	else if ( type->getGraph()==NULL ) {
		qDebug() << "[Model::TypeDAO::checkIfExists] Type does not have graph assigned.";
		return false;
	}
	else if ( !type->isInDB() || ( type->getGraph()!=NULL && !type->getGraph()->isInDB() ) ) {
		return false;
	}

	//nacitame a porovname, ci v databaze ecistuje dany typ
	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "SELECT COUNT(1) FROM nodes "
					"WHERE node_id=:type_id AND graph_id=:graph_id" );
	query->bindValue( ":type_id", type->getId() );
	query->bindValue( ":graph_id", type->getGraph()->getId() );
	if ( !query->exec() ) {
		qDebug() << "[Model::TypeDAO::checkIfExists] Could not perform query on DB: " << query->lastError().databaseText();
		return NULL;
	}
	if ( query->next() && query->value( 0 )==1 ) {
		return true;
	}
	else {
		return false;
	}
}

bool Model::TypeDAO::removeType( Data::Type* type, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::TypeDAO::removeType] Connection to DB not opened.";
		return NULL;
	}
	else if ( type==NULL ) {
		qDebug() << "[Model::TypeDAO::removeType] Invalid parameter - type is NULL.";
		return false;
	}
	else if ( type->getGraph()==NULL ) {
		qDebug() << "[Model::TypeDAO::removeType] Type does not have graph.";
		return false;
	}
	else if ( !type->isInDB() || ( type->getGraph()!=NULL && !type->getGraph()->isInDB() ) ) {
		return true;
	}

	//odstranime z databazy dany typ
	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "DELETE FROM nodes WHERE graph_id = :graph_id AND node_id = :node_id" );
	query->bindValue( ":graph_id", type->getGraph()->getId() );
	query->bindValue( ":node_id", type->getId() );
	if ( !query->exec() ) {
		qDebug() << "[Model::TypeDAO::removeType] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

Data::Type* Model::TypeDAO::addType( QString type_name, Data::Graph* graph, QSqlDatabase* conn, QMap<QString, QString>* settings )
{
	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::TypeDAO::addType] Connection to DB not opened.";
		return NULL;
	}
	else if ( graph==NULL ) {
		qDebug() << "[Model::TypeDAO::addType] Invalid parameter - graph is NULL.";
		return NULL;
	}
	else if ( !graph->isInDB() ) {
		if ( !Model::GraphDAO::addGraph( graph, conn ) ) { //could not insert graph into DB
			qDebug() << "[Model::TypeDAO::addType] Could not insert type in DB. Graph is not in DB.";
			return NULL;
		}
	}

	//pridame do databazy typ
	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "INSERT INTO nodes (\"name\", graph_id) VALUES (:type_name,:graph_id) RETURNING node_id" );
	query->bindValue( ":type_name",type_name );
	query->bindValue( ":graph_id", graph->getId() );
	if ( !query->exec() ) {
		qDebug() << "[Model::TypeDAO::addType] Could not perform query on DB: " << query->lastError().databaseText();
		return NULL;
	}

	if ( query->next() ) {
		Data::Type* type = new Data::Type( query->value( 0 ).toLongLong(),type_name,graph,settings );
		type->setIsInDB();
		qDebug() << "[Model::TypeDAO::addType] Type was added to DB: " << type->toString();
		return type;
	}
	else {
		qDebug() << "[Model::TypeDAO::addType] Type was not added to DB: " << query->lastQuery();
		return NULL;
	}
}

bool Model::TypeDAO::addType( Data::Type* type, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::TypeDAO::addType] Connection to DB not opened.";
		return false;
	}
	else if ( type==NULL ) {
		qDebug() << "[Model::TypeDAO::addType] Invalid parameter - type is NULL";
		return false;
	}

	if ( !type->getGraph()->isInDB() ) {
		if ( !Model::GraphDAO::addGraph( type->getGraph(), conn ) ) { //could not insert graph into DB
			qDebug() << "[Model::TypeDAO::addType] Could not insert type in DB. Graph is not in DB.";
			return NULL;
		}
	}

	if ( type->isInDB() ) {
		return true;
	}

	//pridame do databazy typ
	QSqlQuery* query = new QSqlQuery( *conn );
	if ( type->isMeta() ) {
		if ( !( static_cast<Data::MetaType*>( type ) )->getLayout()->isInDB() ) {
			if ( !Model::GraphLayoutDAO::addLayout( ( static_cast<Data::MetaType*>( type ) )->getLayout(),conn ) ) {
				qDebug() << "[Model::TypeDAO::addType] Could not insert metatype in DB. Layout is not in DB.";
				return NULL;
			}
		}
		query->prepare( "INSERT INTO nodes (\"name\", graph_id, meta, layout_id) VALUES (:type_name,:graph_id,:meta,:layout_id) RETURNING node_id" );
		query->bindValue( ":meta", true );
		query->bindValue( ":layout_id", ( static_cast<Data::MetaType*>( type ) )->getLayout()->getId() );
	}
	else {
		query->prepare( "INSERT INTO nodes (\"name\", graph_id) VALUES (:type_name,:graph_id) RETURNING node_id" );
	}
	query->bindValue( ":type_name",type->getId() );
	query->bindValue( ":graph_id", type->getGraph()->getId() );

	if ( !query->exec() ) {
		qDebug() << "[Model::TypeDAO::addType] Could not perform query on DB: " << query->lastError().databaseText();
		return NULL;
	}

	if ( query->next() ) {
		type->setIsInDB();
		qDebug() << "[Model::TypeDAO::addType] Type was added to DB";
		return true;
	}
	else {
		qDebug() << "[Model::TypeDAO::addType] Type was not added to DB: " << query->lastQuery();
		return false;
	}
}

QMap<QString,QString> Model::TypeDAO::getSettings( Data::Type* type, QSqlDatabase* conn, bool* error )
{
	QMap<QString,QString> settings;
	*error = FALSE;

	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::TypeDAO::getSettings] Connection to DB not opened.";
		*error = TRUE;
		return settings;
	}

	if ( type==NULL ) {
		qDebug() << "[Model::TypeDAO::getSettings] Invalid parameter - type is NULL";
		*error = TRUE;
		return settings;
	}

	if ( !type->isInDB() ) {
		qDebug() << "[Model::TypeDAO::getSettings] Type is not in DB";
		*error = TRUE;
		return settings;
	}

	//nacitame z databazy nastavenie podla ID typu
	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "SELECT val_name, val FROM type_settings WHERE graph_id = :graph_id AND type_id = :type_id" );
	query->bindValue( ":graph_id",type->getGraph()->getId() );
	query->bindValue( ":layout_id",type->getId() );
	if ( !query->exec() ) {
		qDebug() << "[Model::TypeDAO::getSettings] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return settings;
	}

	while ( query->next() ) {
		settings.insert( query->value( 0 ).toString(),query->value( 1 ).toString() );
	}

	return settings;
}
