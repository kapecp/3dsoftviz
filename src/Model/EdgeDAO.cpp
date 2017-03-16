/*!
 * EdgeDAO.cpp
 * Projekt 3DVisual
 */
#include "Model/EdgeDAO.h"
#include "Data/Edge.h"
#include "Data/Graph.h"
#include "Data/GraphLayout.h"

#include <QDebug>

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

Model::EdgeDAO::EdgeDAO( void )
{
}

Model::EdgeDAO::~EdgeDAO( void )
{
}

bool Model::EdgeDAO::addEdgesToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges )
{
	//overime ci mame pripojenie
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::EdgeDAO::addEdgesToDB] Connection to DB not opened.";
		return false;
	}

	QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator iEdges =	edges->constBegin();

	QSqlQuery* query = new QSqlQuery( *conn );

	//pridavame hrany do query
	while ( iEdges != edges->constEnd() ) {
		bool isNested = false;
		if ( iEdges.value()->getType() == iEdges.value()->getGraph()->getNestedEdgeType() ) {
			isNested = true;
		}

		query->prepare( "INSERT INTO edges (edge_id, name, type_id, n1, n2, oriented, meta, graph_id, nested) VALUES (:edge_id, :name, :type_id, :n1, :n2, :oriented, :meta, :graph_id, :nested) RETURNING  edge_id" );
		query->bindValue( ":edge_id", iEdges.value()->getId() );
		query->bindValue( ":name", ( static_cast<Data::AbsEdge*>( iEdges.value() ) )->getName() );
		query->bindValue( ":type_id", iEdges.value()->getType()->getId() );
		query->bindValue( ":n1", iEdges.value()->getSrcNode()->getId() );
		query->bindValue( ":n2", iEdges.value()->getDstNode()->getId() );
		query->bindValue( ":oriented", iEdges.value()->isOriented() );
		query->bindValue( ":meta", false );
		query->bindValue( ":graph_id", iEdges.value()->getGraph()->getId() );
		query->bindValue( ":nested", isNested );

		if ( !query->exec() ) {
			qDebug() << "[Model::EdgeDAO::addEdgesToDB] Could not perform query on DB: " << query->lastError().databaseText();
			return false;
		}
		++iEdges;
	}

	qDebug() << "[Model::EdgeDAO::addEdgesToDB] " << edges->count() << " edges were saved to DB.";

	return true;
}

bool Model::EdgeDAO::addMetaEdgesToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, QMap<qlonglong, qlonglong> newMetaEdgeID )
{
	//check if we have connection
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::EdgeDAO::addMetaEdgesToDB] Connection to DB not opened.";
		return false;
	}

	QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator iEdges =	edges->constBegin();

	QSqlQuery* query = new QSqlQuery( *conn );
	qlonglong nodeID1, nodeID2, edgeID;
	QMap<qlonglong, qlonglong>::iterator nodeIdIter;
	QMap<qlonglong, qlonglong>::iterator edgeIdIter;

	//pridavame meta-uzly do query DB
	while ( iEdges != edges->constEnd() ) {
		if ( newMetaNodeID.contains( iEdges.value()->getSrcNode()->getId() ) ) {
			nodeIdIter = newMetaNodeID.find( iEdges.value()->getSrcNode()->getId() );
			nodeID1 = nodeIdIter.value();
		}
		else {
			nodeID1 = iEdges.value()->getSrcNode()->getId();
		}

		if ( newMetaNodeID.contains( iEdges.value()->getDstNode()->getId() ) ) {
			nodeIdIter = newMetaNodeID.find( iEdges.value()->getDstNode()->getId() );
			nodeID2 = nodeIdIter.value();
		}
		else {
			nodeID2 = iEdges.value()->getDstNode()->getId();
		}

		if ( newMetaEdgeID.contains( iEdges.value()->getId() ) ) {
			edgeIdIter = newMetaEdgeID.find( iEdges.value()->getId() );
			edgeID = edgeIdIter.value();
		}
		else {
			qDebug() << "[Model::NodeDAO::addMetaEdgesToDB] Edge ID: " << iEdges.value()->getId() <<  " mismatch";
		}

		bool isNested = false;
		if ( iEdges.value()->getType() == iEdges.value()->getGraph()->getNestedEdgeType() ) {
			isNested = true;
		}

		query->prepare( "INSERT INTO edges (edge_id, name, type_id, n1, n2, oriented, meta, graph_id, layout_id, nested) VALUES (:edge_id, :name, :type_id, :n1, :n2, :oriented, :meta, :graph_id, :layout_id, :nested) RETURNING  edge_id" );
		query->bindValue( ":edge_id", edgeID );
		query->bindValue( ":name", ( static_cast<Data::AbsEdge*>( iEdges.value() ) )->getName() );
		query->bindValue( ":type_id", iEdges.value()->getType()->getId() );
		query->bindValue( ":n1", nodeID1 );
		query->bindValue( ":n2", nodeID2 );
		query->bindValue( ":oriented", iEdges.value()->isOriented() );
		query->bindValue( ":meta", true );
		query->bindValue( ":graph_id", iEdges.value()->getGraph()->getId() );
		query->bindValue( ":layout_id", layout->getId() );
		query->bindValue( ":nested", isNested );

		if ( !query->exec() ) {
			qDebug() << "[Model::EdgeDAO::addMetaEdgesToDB] Could not perform query on DB: " << query->lastError().databaseText();
			return false;
		}
		++iEdges;
	}

	qDebug() << "[Model::EdgeDAO::addMetaEdgesToDB] " << edges->count() << " meta edges were saved to DB.";

	return true;
}

bool Model::EdgeDAO::addEdgesColorToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaEdgeID, bool meta )
{
	QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator iEdges = edges->constBegin();
	qlonglong edgeID;
	QMap<qlonglong, qlonglong>::iterator edgeIdIter;

	while ( iEdges != edges->constEnd() ) {
		//ulozime farbu len hranam, ktore maju farbu inu nez default
		if ( iEdges.value()->getEdgeColor()[0] != 1.f || iEdges.value()->getEdgeColor()[1] != 1.f ||iEdges.value()->getEdgeColor()[2] != 1.f ||iEdges.value()->getEdgeColor()[3] != 1.f ) {
			if ( meta ) {
				if ( newMetaEdgeID.contains( iEdges.value()->getId() ) ) {
					edgeIdIter = newMetaEdgeID.find( iEdges.value()->getId() );
					edgeID = edgeIdIter.value();
				}
				else {
					qDebug() << "[Model::NodeDAO::addEdgesColorToDB] Edge ID: " << iEdges.value()->getId() <<  " mismatch";
				}
			}
			else {
				edgeID = iEdges.value()->getId();
			}

			addSetings( conn, iEdges.value()->getGraph()->getId(), layout->getId(), edgeID, "color_r", iEdges.value()->getEdgeColor()[0] );
			addSetings( conn, iEdges.value()->getGraph()->getId(), layout->getId(), edgeID, "color_g", iEdges.value()->getEdgeColor()[1] );
			addSetings( conn, iEdges.value()->getGraph()->getId(), layout->getId(), edgeID, "color_b", iEdges.value()->getEdgeColor()[2] );
			addSetings( conn, iEdges.value()->getGraph()->getId(), layout->getId(), edgeID, "color_a", iEdges.value()->getEdgeColor()[3] );
		}

		++iEdges;
	}

	return true;
}

bool Model::EdgeDAO::addEdgesScaleToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges, Data::GraphLayout* layout,  QMap<qlonglong, qlonglong> newMetaEdgeID, bool meta, double defaultScale )
{
	QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator iEdges = edges->constBegin();
	qlonglong edgeID;
	QMap<qlonglong, qlonglong>::iterator edgeIdIter;

	while ( iEdges != edges->constEnd() ) {
		//ulozime scale len hranam, ktore maju scale ine nez default
		bool isNotDefault = !qFuzzyCompare( iEdges.value()->getScale(),defaultScale );
		if ( isNotDefault ) {
			if ( meta ) {
				if ( newMetaEdgeID.contains( iEdges.value()->getId() ) ) {
					edgeIdIter = newMetaEdgeID.find( iEdges.value()->getId() );
					edgeID = edgeIdIter.value();
				}
				else {
					qDebug() << "[Model::NodeDAO::addEdgesScaleToDB] Edge ID: " << iEdges.value()->getId() <<  " mismatch";
				}
			}
			else {
				edgeID = iEdges.value()->getId();
			}

			addSetings( conn, iEdges.value()->getGraph()->getId(), layout->getId(), edgeID, "scale", iEdges.value()->getScale() );
		}

		++iEdges;
	}

	return true;
}

QSqlQuery* Model::EdgeDAO::getEdgesQuery( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID )
{
	QSqlQuery* query;
	*error = FALSE;

	//check if we have connection
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::EdgeDAO::getEdgesQuery] Connection to DB not opened.";
		*error = TRUE;

		return NULL;
	}

	//nacitame SELECTom hrany z DB
	query = new QSqlQuery( *conn );
	query->prepare( "SELECT * "
					"FROM edges "
					"WHERE graph_id = :graph_id "
					"AND (layout_id IS NULL OR layout_id = :layout_id)" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":layout_id", layoutID );

	if ( !query->exec() ) {
		qDebug() << "[Model::EdgeDAO::getEdgesQuery] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return query;
	}

	return query;
}

QList<qlonglong> Model::EdgeDAO::getListOfEdges( QSqlDatabase* conn, bool* error )
{
	QList<qlonglong> edges;
	*error = FALSE;

	//check if we have connection
	if ( conn==NULL || !conn->isOpen() )

	{
		qDebug() << "[Model::EdgeDAO::getListOfEdges] Connection to DB not opened.";
		*error = TRUE;
		return edges;
	}

	//nacitame uzly z DB
	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "SELECT graph_id "
					"FROM edges " );

	if ( !query->exec() ) {
		qDebug() << "[Model::EdgeDAO::getListOfEdges] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return edges;
	}

	while ( query->next() ) {
		edges << query->value( 0 ).toLongLong();
	}

	return edges;
}

bool Model::EdgeDAO::checkIfExists( Data::Edge* edge, QSqlDatabase* conn )
{
	//overime ci je dana hrana ulozena v databaze
	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::EdgeDAO::checkIfExists] Connection to DB not opened.";
		return ( bool* )NULL;
	}
	else if ( edge==NULL ) {
		qDebug() << "[Model::EdgeDAO::checkIfExists] Invalid parameter - edge is NULL.";
		return ( bool* )NULL;
	}
	else if ( edge->getGraph()==NULL ) {
		qDebug() << "[Model::EdgeDAO::checkIfExists] Edge does not have graph assigned.";
		return false;
	}
	else if ( !edge->isInDB() || ( edge->getGraph()!=NULL && !edge->getGraph()->isInDB() ) ) {
		return false;
	}

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "SELECT COUNT(1) FROM edges "
					"WHERE edge_id=:edge_id AND graph_id=:graph_id" );
	query->bindValue( ":edge_id", edge->getId() );
	query->bindValue( ":graph_id", edge->getGraph()->getId() );
	if ( !query->exec() ) {
		qDebug() << "[Model::EdgeDAO::checkIfExists] Could not perform query on DB: " << query->lastError().databaseText();
		return ( bool* )NULL;
	}
	if ( query->next() && query->value( 0 )==1 ) {
		return true;
	}
	else {
		return false;
	}
}

bool Model::EdgeDAO::removeEdge( Data::Edge* edge, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::EdgeDAO::removeEdge] Connection to DB not opened.";
		return ( bool* )NULL;
	}
	else if ( edge==NULL ) {
		qDebug() << "[Model::EdgeDAO::removeEdge] Invalid parameter - edge is NULL.";
		return false;
	}
	else if ( edge->getGraph()==NULL ) {
		qDebug() << "[Model::EdgeDAO::removeEdge] Edge does not have graph assigned.";
		return false;
	}
	else if ( !edge->isInDB() ) {
		return false;
	}

	//odoberieme hranu z databazy
	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "DELETE FROM edges WHERE graph_id = :graph_id AND edge_id = :edge_id" );
	query->bindValue( ":graph_id", edge->getGraph()->getId() );
	query->bindValue( ":edge_id", edge->getId() );
	if ( !query->exec() ) {
		qDebug() << "[Model::EdgeDAO::removeEdge] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

bool Model::EdgeDAO::removeEdges( qlonglong graphID, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::EdgeDAO::removeEdges] Connection to DB not opened.";
		return ( bool* )NULL;
	}

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "DELETE FROM edge_settings WHERE graph_id = :graph_id" );
	query->bindValue( ":graph_id", graphID );
	if ( !query->exec() ) {
		qDebug() << "[Model::EdgeDAO::removeEdges] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	//vymazeme vsetky hrany daneho grafu z databazy
	query->prepare( "DELETE FROM edges WHERE graph_id = :graph_id" );
	query->bindValue( ":graph_id", graphID );
	if ( !query->exec() ) {
		qDebug() << "[Model::EdgeDAO::removeEdges] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

bool Model::EdgeDAO::removeEdges( qlonglong graphID, qlonglong layoutID, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::EdgeDAO::removeEdges] Connection to DB not opened.";
		return ( bool* )NULL;
	}

	//vymazeme layout z databazy
	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "DELETE FROM edge_settings WHERE graph_id = :graph_id AND layout_id = :layout_id" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":layout_id", layoutID );
	if ( !query->exec() ) {
		qDebug() << "[Model::EdgeDAO::removeEdges] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	//vymazeme vsetky uzly z databazy
	query->prepare( "DELETE FROM edges WHERE graph_id = :graph_id AND layout_id = :layout_id" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":layout_id", layoutID );
	if ( !query->exec() ) {
		qDebug() << "[Model::EdgeDAO::removeEdges] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

QMap<QString,QString> Model::EdgeDAO::getSettings( Data::Edge* edge, QSqlDatabase* conn, bool* error )
{
	QMap<QString,QString> settings;
	*error = FALSE;

	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::EdgeDAO::getSettings] Connection to DB not opened.";
		*error = TRUE;
		return settings;
	}

	if ( edge==NULL ) {
		qDebug() << "[Model::EdgeDAO::getSettings] Invalid parameter - Edge is NULL";
		*error = TRUE;
		return settings;
	}

	if ( !edge->isInDB() ) {
		qDebug() << "[Model::EdgeDAO::getSettings] Edge is not in DB";
		*error = TRUE;
		return settings;
	}

	//nacitame zoznam nastaveni pre hrany
	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "SELECT val_name, val FROM edge_settings WHERE graph_id = :graph_id AND edge_id = :edge_id" );
	query->bindValue( ":graph_id",edge->getGraph()->getId() );
	query->bindValue( ":edge_id",edge->getId() );
	if ( !query->exec() ) {
		qDebug() << "[Model::EdgeDAO::getSettings] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return settings;
	}

	while ( query->next() ) {
		settings.insert( query->value( 0 ).toString(),query->value( 1 ).toString() );
	}

	return settings;
}

QMap<qlonglong, QString> Model::EdgeDAO::getSettings( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID, QString attributeName )
{
	*error = FALSE;
	QSqlQuery* query;
	QMap<qlonglong, QString> settings;

	//check if we have connection
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::EdgeDAO::getSettings] Connection to DB not opened.";
		*error = TRUE;
		return settings;
	}

	query = new QSqlQuery( *conn );
	query->prepare( "SELECT edge_id, val "
					"FROM edge_settings "
					"WHERE graph_id = :graph_id "
					"AND val_name = :attribute_name "
					"AND layout_id = :layout_id" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":attribute_name", attributeName );
	query->bindValue( ":layout_id", layoutID );

	//nacitame z databazy zoznam nastaveni
	if ( !query->exec() ) {
		qDebug() << "[Model::EdgeDAO::getSettings] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return settings;
	}

	while ( query->next() ) {
		settings.insert( query->value( 0 ).toLongLong(), query->value( 1 ).toString() );
	}

	return settings;
}

QMap<qlonglong, osg::Vec4f> Model::EdgeDAO::getColors( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID )
{
	*error = FALSE;
	bool error2 = false;
	osg::Vec4f color;
	QMap<qlonglong, osg::Vec4f> colors;

	QMap<qlonglong, QString> edgeColorR;
	QMap<qlonglong, QString> edgeColorG;
	QMap<qlonglong, QString> edgeColorB;
	QMap<qlonglong, QString> edgeColorA;
	QMap<qlonglong, QString>::iterator iter_r;
	QMap<qlonglong, QString>::iterator iter_g;
	QMap<qlonglong, QString>::iterator iter_b;
	QMap<qlonglong, QString>::iterator iter_a;

	edgeColorR = getSettings( conn, &error2, graphID, layoutID, "color_r" );
	if ( error2 ) {
		qDebug() << "[Model::EdgeDAO::getColors] Could not get color_r setting";
		*error = error2;
		return colors;
	}
	edgeColorG = getSettings( conn, &error2, graphID, layoutID, "color_g" );
	if ( error2 ) {
		qDebug() << "[Model::EdgeDAO::getColors] Could not get color_g setting";
		*error = error2;
		return colors;
	}
	edgeColorB = getSettings( conn, &error2, graphID, layoutID, "color_b" );
	if ( error2 ) {
		qDebug() << "[Model::EdgeDAO::getColors] Could not get color_b setting";
		*error = error2;
		return colors;
	}
	edgeColorA = getSettings( conn, &error2, graphID, layoutID, "color_a" );
	if ( error2 ) {
		qDebug() << "[Model::NodeDAO::getColors] Could not get color_a setting";
		*error = error2;
		return colors;
	}

	//nacitavame ulozene farby v databaze
	for ( iter_r = edgeColorR.begin(); iter_r != edgeColorR.end(); ++iter_r ) {

		qlonglong id = iter_r.key();
		iter_g = edgeColorG.find( id );
		iter_b = edgeColorB.find( id );
		iter_a = edgeColorA.find( id );

		color = osg::Vec4f( iter_r.value().toFloat(), iter_g.value().toFloat(), iter_b.value().toFloat(), iter_a.value().toFloat() );
		colors.insert( id, color );
	}

	return colors;
}

QMap<qlonglong, float> Model::EdgeDAO::getScales( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID )
{
	*error = FALSE;
	bool error2 = false;
	QMap<qlonglong, float> scales;

	QMap<qlonglong, QString> edgeScale;
	QMap<qlonglong, QString>::iterator iter;

	edgeScale = getSettings( conn, &error2, graphID, layoutID, "scale" );
	if ( error2 ) {
		qDebug() << "[Model::EdgeDAO::getScales] Could not get scale setting";
		*error = error2;
		return scales;
	}

	//nacitavame z databazy velkosti jednotlivych prvkov
	for ( iter = edgeScale.begin(); iter != edgeScale.end(); ++iter ) {
		qlonglong id = iter.key();

		float scale = iter.value().toFloat();
		scales.insert( id, scale );
	}

	return scales;
}

QMap<qlonglong, qlonglong> Model::EdgeDAO::getNewMetaEdgesId( QSqlDatabase* conn, qlonglong graphID, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges )
{
	QMap<qlonglong, qlonglong> newId;
	qlonglong maxId = 0;
	QSqlQuery* query;

	QMap< qlonglong, osg::ref_ptr<Data::Edge> >::const_iterator iEdges = edges->constBegin();

	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::EdgeDAO::getNewMetaEdgesId] Connection to DB not opened.";
		return newId;
	}

	query = new QSqlQuery( *conn );
	query->prepare( "SELECT MAX(edge_id) "
					"FROM edges "
					"WHERE graph_id = :graph_id " );
	query->bindValue( ":graph_id", graphID );

	if ( !query->exec() ) {
		qDebug() << "[Model::EdgeDAO::getNewMetaEdgesId] Could not perform query on DB: " << query->lastError().databaseText();
		return newId;
	}

	while ( query->next() ) {
		maxId = query->value( 0 ).toLongLong();
	}

	//vytvarame mapu novych hran
	while ( iEdges != edges->constEnd() ) {
		maxId++;
		newId.insert( iEdges.value()->getId(), maxId );
		++iEdges;
	}

	return newId;
}

bool Model::EdgeDAO::addSetings( QSqlDatabase* conn, qlonglong graphID, qlonglong layoutID, qlonglong edgeID, QString valName, double val )
{
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::EdgeDAO::addSettings] Connection to DB not opened.";
		return false;
	}

	QSqlQuery* query = new QSqlQuery( *conn );

	query->prepare( "INSERT INTO edge_settings (graph_id, edge_id, val_name, val, layout_id) VALUES (:graph_id, :edge_id, :val_name, :val, :layout_id)" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":edge_id", edgeID );
	query->bindValue( ":val_name", valName );
	query->bindValue( ":val", val );
	query->bindValue( ":layout_id", layoutID );

	//ukladame jednotlive atributy do databazy
	if ( !query->exec() ) {
		qDebug() << "[Model::EdgeDAO::addSettings] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

bool Model::EdgeDAO::addSetings( QSqlDatabase* conn, qlonglong graphID, qlonglong layoutID, qlonglong edgeID, QString valName, float val )
{
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::EdgeDAO::addSettings] Connection to DB not opened.";
		return false;
	}

	QSqlQuery* query = new QSqlQuery( *conn );

	query->prepare( "INSERT INTO edge_settings (graph_id, edge_id, val_name, val, layout_id) VALUES (:graph_id, :edge_id, :val_name, :val, :layout_id)" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":edge_id", edgeID );
	query->bindValue( ":val_name", valName );
	query->bindValue( ":val", val );
	query->bindValue( ":layout_id", layoutID );

	//ukladame jednotlive atributy do databazy
	if ( !query->exec() ) {
		qDebug() << "[Model::EdgeDAO::addSettings] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
