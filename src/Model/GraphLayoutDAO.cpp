/*!
 * GraphLayoutDAO.cpp
 * Projekt 3DVisual
 */
#include "Model/GraphLayoutDAO.h"

#include "Data/GraphLayout.h"
#include "Data/Graph.h"

#include "Model/GraphDAO.h"
#include "Model/NodeDAO.h"
#include "Model/EdgeDAO.h"

#include <QDebug>

Model::GraphLayoutDAO::GraphLayoutDAO( void )
{
}

Model::GraphLayoutDAO::~GraphLayoutDAO( void )
{
}

QMap<qlonglong, QString> Model::GraphLayoutDAO::getLayoutsNames( qlonglong graph_id, QSqlDatabase* conn, bool* error )
{
	QMap<qlonglong, QString> layoutNames;
	*error = FALSE;

	if ( conn==NULL || !conn->open() ) { //check if we have connection
		qDebug() << "[Model::GraphLayoutDAO::getLayoutsNames] Connection to DB not opened.";
		*error = TRUE;
		return layoutNames;
	}

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "SELECT layout_id, layout_name FROM layouts WHERE graph_id = :graph_id" );
	query->bindValue( ":graph_id", graph_id );
	if ( !query->exec() ) {
		qDebug() << "[Model::GraphLayoutDAO::getLayoutsNames] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return layoutNames;
	}

	//nacitavame nazvy rozlozeni z databazy
	while ( query->next() ) {
		layoutNames.insert( query->value( 0 ).toLongLong(), query->value( 1 ).toString() );
	}

	return layoutNames;
}

QList<qlonglong> Model::GraphLayoutDAO::getListOfLayouts( QSqlDatabase* conn, bool* error )
{
	QList<qlonglong> layouts;
	*error = FALSE;

	//check if we have connection
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::GraphLayoutDAO::getListOfLayouts] Connection to DB not opened.";
		*error = TRUE;
		return layouts;
	}

	//get layouts from DB
	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "SELECT graph_id "
					"FROM layouts " );

	//nacitame zoznam layoutou z databazy
	if ( !query->exec() ) {
		qDebug() << "[Model::GraphLayoutDAO::getListOfLayouts] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return layouts;
	}

	while ( query->next() ) {
		layouts << query->value( 0 ).toLongLong();
	}

	return layouts;
}

QMap<qlonglong, Data::GraphLayout*> Model::GraphLayoutDAO::getLayouts( Data::Graph* graph, QSqlDatabase* conn, bool* error )
{
	QMap<qlonglong, Data::GraphLayout*> qgraphslayouts;
	*error = FALSE;

	if ( conn==NULL || !conn->open() ) { //check if we have connection
		qDebug() << "[Model::GraphLayoutDAO::getLayouts] Connection to DB not opened.";
		*error = TRUE;
		return qgraphslayouts;
	}

	if ( graph==NULL ) {
		qDebug() << "[Model::GraphLayoutDAO::getLayouts] Invalid parameter - graph is NULL";
		*error = TRUE;
		return qgraphslayouts;
	}

	if ( !graph->isInDB() ) {
		return qgraphslayouts;
	}

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "SELECT layout_id, layout_name FROM layouts WHERE graph_id = :graph_id" );
	query->bindValue( ":graph_id", graph->getId() );

	//nacitame jednotlive layouty z databazy do QMapy
	if ( !query->exec() ) {
		qDebug() << "[Model::GraphLayoutDAO::getLayouts] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return qgraphslayouts;
	}

	while ( query->next() ) {
		qgraphslayouts.insert( query->value( 0 ).toLongLong(), new Data::GraphLayout( query->value( 0 ).toLongLong(),graph,query->value( 1 ).toString(),conn ) );
	}

	return qgraphslayouts;
}

Data::GraphLayout* Model::GraphLayoutDAO::addLayout( QString layout_name, Data::Graph* graph, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::GraphLayoutDAO::addLayout] Connection to DB not opened.";
		return NULL;
	}
	else if ( graph==NULL ) {
		qDebug() << "[Model::GraphLayoutDAO::addLayout] Invalid parameter - graph is NULL";
		return NULL;
	}

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "INSERT INTO layouts (layout_name, graph_id) VALUES (:layout_name,:graph_id) RETURNING layout_id" );
	query->bindValue( ":layout_name",layout_name );
	query->bindValue( ":graph_id", graph->getId() );

	//ulozime layout do databazy
	if ( !query->exec() ) {
		qDebug() << "[Model::GraphLayoutDAO::addLayout] Could not perform query on DB: " << query->lastError().databaseText();
		return NULL;
	}

	if ( query->next() ) {
		Data::GraphLayout* layout = new Data::GraphLayout( query->value( 0 ).toLongLong(),graph,layout_name,conn );
		layout->setIsInDB();
		qDebug() << "[Model::GraphLayoutDAO::addLayout] GraphLayout was added to DB: " << layout->toString();
		return layout;
	}
	else {
		qDebug() << "[Model::GraphLayoutDAO::addLayout] GraphLayout was not added to DB: " << query->lastQuery();
		return NULL;
	}
}

bool Model::GraphLayoutDAO::addLayout( Data::GraphLayout* layout, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::GraphLayoutDAO::addLayout] Connection to DB not opened.";
		return false;
	}
	else if ( layout==NULL ) {
		qDebug() << "[Model::GraphLayoutDAO::addLayout] Invalid parameter - layout is NULL";
		return false;
	}
	else if ( layout->getGraph()==NULL ) {
		qDebug() << "[Model::GraphLayoutDAO::addLayout] Layout does not have a graph assigned.";
		return false;
	}

	if ( layout->isInDB() ) {
		return true;
	}

	if ( !layout->getGraph()->isInDB() ) {
		if ( !Model::GraphDAO::addGraph( layout->getGraph(), conn ) ) { //could not insert graph into DB
			qDebug() << "[Model::GraphLayoutDAO::addType] Could not insert GraphLayout in DB. Graph is not in DB.";
			return reinterpret_cast<bool*>( NULL );
		}
	}

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "INSERT INTO layouts (layout_id, layout_name, graph_id) VALUES (:layout_id,:layout_name,:graph_id) RETURNING layout_id" );
	query->bindValue( ":layout_name",layout->getName() );
	query->bindValue( ":graph_id", layout->getGraphId() );

	//ulozime layout do databazy
	if ( !query->exec() ) {
		qDebug() << "[Model::GraphLayoutDAO::addLayout] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	if ( query->next() ) {
		layout->setIsInDB();
		qDebug() << "[Model::GraphLayoutDAO::addLayout] GraphLayout was added to DB";
		return true;
	}
	else {
		qDebug() << "[Model::GraphLayoutDAO::addLayout] GraphLayout was not added to DB: " << query->lastQuery();
		return false;
	}
}

bool Model::GraphLayoutDAO::removeLayout( Data::GraphLayout* graphLayout, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::GraphLayoutDAO::removeLayout] Connection to DB not opened.";
		return false;
	}
	else if ( graphLayout==NULL ) {
		qDebug() << "[Model::GraphLayoutDAO::removeLayout] Invalid parameter - graphLayout is NULL";
		return false;
	}

	if ( !graphLayout->isInDB() || ( graphLayout->getGraph()!=NULL && !graphLayout->getGraph()->isInDB() ) ) {
		return true;
	}

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "DELETE FROM layouts WHERE graph_id = :graph_id AND layout_id = :layout_id" );
	query->bindValue( ":graph_id", graphLayout->getGraph()->getId() );
	query->bindValue( ":layout_id", graphLayout->getId() );

	//odstranime layout z databazy
	if ( !query->exec() ) {
		qDebug() << "[Model::GraphLayoutDAO::removeLayout] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

bool Model::GraphLayoutDAO::removeLayouts( qlonglong graphID, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::GraphLayoutDAO::removeLayouts] Connection to DB not opened.";
		return false;
	}

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "DELETE FROM layouts WHERE graph_id = :graph_id" );
	query->bindValue( ":graph_id", graphID );

	//odstranime vsetky layouty z databazy
	if ( !query->exec() ) {
		qDebug() << "[Model::GraphLayoutDAO::removeLayouts] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

bool Model::GraphLayoutDAO::removeLayout( qlonglong graphID, qlonglong layoutID, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::GraphLayoutDAO::removeLayout] Connection to DB not opened.";
		return false;
	}

	Model::EdgeDAO::removeEdges( graphID, layoutID, conn );
	Model::NodeDAO::removeNodes( graphID, layoutID, conn );

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "DELETE FROM layouts WHERE graph_id = :graph_id AND layout_id = :layout_id" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":layout_id", layoutID );

	//odstranime jeden layout z databazy
	if ( !query->exec() ) {
		qDebug() << "[Model::GraphLayoutDAO::removeLayout] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

QString Model::GraphLayoutDAO::getName( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID )
{
	QSqlQuery* query;
	*error = FALSE;

	//check if we have connection
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::GraphLayoutDAO::getName] Connection to DB not opened.";
		*error = TRUE;
		return "";
	}

	//get name of layout from DB
	query = new QSqlQuery( *conn );
	query->prepare( "SELECT layout_name "
					"FROM layouts "
					"WHERE graph_id = :graph_id AND layout_id = :layout_id" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":layout_id", layoutID );

	//nacitame nazov layoutu z databazy
	if ( !query->exec() ) {
		qDebug() << "[Model::GraphLayoutDAO::getName] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return "";
	}

	if ( query->next() ) {
		return query->value( 0 ).toString();
	}
	else {
		return "";
	}
}

QString Model::GraphLayoutDAO::setName( QString name, Data::GraphLayout* graphLayout, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->open() ) { //check if we have connection
		qDebug() << "[Model::GraphLayoutDAO::setName] Connection to DB not opened.";
		return NULL;
	}
	else if ( graphLayout==NULL ) {
		qDebug() << "[Model::GraphLayoutDAO::setName] Invalid parameter - graphLayout is NULL";
		return NULL;
	}
	else if ( graphLayout->getGraph()==NULL ) {
		qDebug() << "[Model::GraphLayoutDAO::setName] GraphLayout does not have a graph assigned.";
		return NULL;
	}
	else if ( graphLayout->getGraph()!=NULL && !graphLayout->getGraph()->isInDB() ) {
		if ( !Model::GraphDAO::addGraph( graphLayout->getGraph(), conn ) ) { //could not insert graph into DB
			qDebug() << "[Model::GraphLayoutDAO::setName]] Could not update graphLayout name in DB. Graph is not in DB.";
			return NULL;
		}
	}

	if ( !graphLayout->isInDB() ) { //TODO: ked sa uz pokusame ulozit s novym menom, mozno by bolo efektivnejsie zmenit docasne meno, pokusit sa vlozit, potom zmenit naspat
		if ( !Model::GraphLayoutDAO::addLayout( graphLayout, conn ) ) { //could not insert graphLayout into DB
			qDebug() << "[Model::GraphDAO::setName] Could not update graphLayout name in DB. GraphLayout is not in DB.";
			return NULL;
		}
	}

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "UPDATE layouts SET layout_name = :layout_name WHERE graph_id = :graph_id AND layout_id = :layout_id" );
	query->bindValue( ":layout_id", graphLayout->getId() );
	query->bindValue( ":graph_id", graphLayout->getGraph()->getId() );
	query->bindValue( ":layout_name",name );

	//nastavime nazov layoutu do databazy
	if ( !query->exec() ) {
		qDebug() << "[Model::GraphLayoutDAO::setName] Could not perform query on DB: " << query->lastError().databaseText();
		return name;
	}

	return name;
}

bool Model::GraphLayoutDAO::checkIfExists( Data::GraphLayout* graphLayout, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::GraphLayoutDAO::checkIfExists] Connection to DB not opened.";
		return reinterpret_cast<bool*>( NULL );
	}
	else if ( graphLayout==NULL ) {
		qDebug() << "[Model::GraphLayoutDAO::checkIfExists] Invalid parameter - graphLayout is NULL.";
		return false;
	}
	else if ( graphLayout->getGraph()==NULL ) {
		qDebug() << "[Model::GraphLayoutDAO::checkIfExists] GraphLayout does not have graph assigned.";
		return false;
	}
	else if ( !graphLayout->isInDB() || ( graphLayout->getGraph()!=NULL && !graphLayout->getGraph()->isInDB() ) ) {
		return false;
	}

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "SELECT COUNT(1) FROM layouts "
					"WHERE layout_id=:layout_id AND graph_id=:graph_id" );
	query->bindValue( ":layout_id", graphLayout->getId() );
	query->bindValue( ":graph_id", graphLayout->getGraph()->getId() );

	//overujeme ci existuje layout v databaze podla ID
	if ( !query->exec() ) {
		qDebug() << "[Model::GraphLayoutDAO::checkIfExists] Could not perform query on DB: " << query->lastError().databaseText();
		return reinterpret_cast<bool*>( NULL );
	}
	if ( query->next() && query->value( 0 )==1 ) {
		return true;
	}
	else {
		return false;
	}
}

QMap<QString,QString> Model::GraphLayoutDAO::getSettings( Data::GraphLayout* graphLayout, QSqlDatabase* conn, bool* error )
{
	QMap<QString,QString> settings;
	*error = FALSE;

	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::GraphLayoutDAO::getSettings] Connection to DB not opened.";
		*error = TRUE;
		return settings;
	}

	if ( graphLayout==NULL ) {
		qDebug() << "[Model::GraphLayoutDAO::getSettings] Invalid parameter - graphLayout is NULL";
		*error = TRUE;
		return settings;
	}

	if ( !graphLayout->isInDB() ) {
		qDebug() << "[Model::GraphLayoutDAO::getSettings] GraphLayout is not in DB";
		*error = TRUE;
		return settings;
	}

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "SELECT val_name, val FROM layout_settings WHERE graph_id = :graph_id AND layout_id = :layout_id" );
	query->bindValue( ":graph_id",graphLayout->getGraph()->getId() );
	query->bindValue( ":layout_id",graphLayout->getId() );
	if ( !query->exec() ) {
		qDebug() << "[Model::GraphLayoutDAO::getSettings] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return settings;
	}

	//nacitame settings pre layout z databazy
	while ( query->next() ) {
		settings.insert( query->value( 0 ).toString(),query->value( 1 ).toString() );
	}

	return settings;
}
