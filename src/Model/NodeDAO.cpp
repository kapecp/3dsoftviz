/*!
 * NodeDAO.cpp
 * Projekt 3DVisual
 */
#include "Model/NodeDAO.h"

#include "Data/Graph.h"
#include "Data/GraphLayout.h"

#include <QDebug>

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

Model::NodeDAO::NodeDAO( void )
{
}

Model::NodeDAO::~NodeDAO( void )
{
}

bool Model::NodeDAO::addNodesToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes )
{
	//check if we have connection
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::NodeDAO::addNodesToDB] Connection to DB not opened.";
		return false;
	}

	QMap< qlonglong,osg::ref_ptr<Data::Node> >::const_iterator iNodes = nodes->constBegin();

	QSqlQuery* query = new QSqlQuery( *conn );

	//ukladame vsetky uzly do databazy
	while ( iNodes != nodes->constEnd() ) {
		qlonglong parentId = -1;
		if ( iNodes.value()->getParentNode() != NULL ) {
			parentId = iNodes.value()->getParentNode()->getId();
		}

		query->prepare( "INSERT INTO nodes (node_id, name, type_id, graph_id, meta, fixed, parent_id) VALUES (:node_id, :name, :type_id, :graph_id, :meta, :fixed, :parent_id)" );
		query->bindValue( ":node_id", iNodes.value()->getId() );
		query->bindValue( ":name", ( static_cast<Data::AbsNode*>( iNodes.value() ) )->getName() );
		query->bindValue( ":type_id", iNodes.value()->getType()->getId() );
		query->bindValue( ":graph_id", iNodes.value()->getGraph()->getId() );
		query->bindValue( ":meta", false );
		query->bindValue( ":fixed", iNodes.value()->isFixed() );
		query->bindValue( ":parent_id", parentId );

		if ( !query->exec() ) {
			qDebug() << "[Model::NodeDAO::addNodesToDB] Could not perform query on DB: " << query->lastError().databaseText();
			return false;
		}

		++iNodes;
	}

	qDebug() << "[Model::NodeDAO::addNodesToDB] " << nodes->count() << " nodes were saved to DB.";

	return true;
}

bool Model::NodeDAO::addMetaNodesToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID )
{
	//check if we have connection
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::NodeDAO::addMetaNodesToDB] Connection to DB not opened.";
		return false;
	}

	QMap< qlonglong,osg::ref_ptr<Data::Node> >::const_iterator iNodes = nodes->constBegin();

	QSqlQuery* query = new QSqlQuery( *conn );
	QMap<qlonglong, qlonglong>::iterator nodeIdIter;

	//ukladame vsetky meta-uzly do databazy
	while ( iNodes != nodes->constEnd() ) {
		qlonglong nodeID;
		if ( newMetaNodeID.contains( iNodes.value()->getId() ) ) {
			nodeIdIter = newMetaNodeID.find( iNodes.value()->getId() );
			nodeID = nodeIdIter.value();
		}
		else {
			qDebug() << "[Model::NodeDAO::addMetaNodesToDB] Node ID: " << iNodes.value()->getId() <<  " mismatch";
		}

		qlonglong parentId = -1;
		if ( iNodes.value()->getParentNode() != NULL ) {
			if ( newMetaNodeID.contains( iNodes.value()->getParentNode()->getId() ) ) {
				nodeIdIter = newMetaNodeID.find( iNodes.value()->getParentNode()->getId() );
				parentId = nodeIdIter.value();
			}
			else {
				qDebug() << "[Model::NodeDAO::addMetaNodesToDB] Node ID: " << iNodes.value()->getId() <<  " mismatch";
			}
		}

		query->prepare( "INSERT INTO nodes (node_id, name, type_id, graph_id, meta, fixed, layout_id, parent_id) VALUES (:node_id, :name, :type_id, :graph_id, :meta, :fixed, :layout_id, :parent_id)" );
		query->bindValue( ":node_id", nodeID );
		query->bindValue( ":name", ( static_cast<Data::AbsNode*>( iNodes.value() ) )->getName() );
		query->bindValue( ":type_id", iNodes.value()->getType()->getId() );
		query->bindValue( ":graph_id", iNodes.value()->getGraph()->getId() );
		query->bindValue( ":meta", true );
		query->bindValue( ":fixed", iNodes.value()->isFixed() );
		query->bindValue( ":layout_id", layout->getId() );
		query->bindValue( ":parent_id", parentId );

		if ( !query->exec() ) {
			qDebug() << "[Model::NodeDAO::addMetaNodesToDB] Could not perform query on DB: " << query->lastError().databaseText();
			return false;
		}

		++iNodes;
	}

	qDebug() << "[Model::NodeDAO::addMetaNodesToDB] " << nodes->count() << " meta nodes were saved to DB.";

	return true;
}

bool Model::NodeDAO::addNodesPositionsToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta )
{
	//check if we have connection
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::NodeDAO::addNodesPositionsToDB] Connection to DB not opened.";
		return false;
	}

	QMap< qlonglong,osg::ref_ptr<Data::Node> >::const_iterator iNodes = nodes->constBegin();

	QSqlQuery* query = new QSqlQuery( *conn );
	qlonglong nodeID;
	QMap<qlonglong, qlonglong>::iterator nodeIdIter;

	//ukladame pozicie uzlov pre vsetky uzly
	while ( iNodes != nodes->constEnd() ) {
		if ( meta ) {
			if ( newMetaNodeID.contains( iNodes.value()->getId() ) ) {
				nodeIdIter = newMetaNodeID.find( iNodes.value()->getId() );
				nodeID = nodeIdIter.value();
			}
			else {
				qDebug() << "[Model::NodeDAO::addNodesPositionsToDB] Node ID: " << iNodes.value()->getId() <<  " mismatch";
			}
		}
		else {
			nodeID = iNodes.value()->getId();
		}

		query->prepare( "INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z, graph_id) VALUES (:layout_id, :node_id, :pos_x, :pos_y, :pos_z, :graph_id) RETURNING  node_id" );
		query->bindValue( ":layout_id", layout->getId() );
		query->bindValue( ":node_id", nodeID );
		query->bindValue( ":pos_x", iNodes.value()->getCurrentPosition().x() );
		query->bindValue( ":pos_y", iNodes.value()->getCurrentPosition().y() );
		query->bindValue( ":pos_z", iNodes.value()->getCurrentPosition().z() );
		query->bindValue( ":graph_id", iNodes.value()->getGraph()->getId() );

		if ( !query->exec() ) {
			qDebug() << "[Model::NodeDAO::addNodesPositionsToDB] Could not perform query on DB: " << query->lastError().databaseText();
			return false;
		}

		++iNodes;
	}

	return true;
}

bool Model::NodeDAO::addNodesColorToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta )
{
	QMap< qlonglong,osg::ref_ptr<Data::Node> >::const_iterator iNodes = nodes->constBegin();
	qlonglong nodeID;
	QMap<qlonglong, qlonglong>::iterator nodeIdIter;

	//ukladame vsetky farby uzlov do databazy
	while ( iNodes != nodes->constEnd() ) {
		//ulozime farbu len nodom, ktore maju farbu inu nez default

		if ( iNodes.value()->getColor().r() != 1 || iNodes.value()->getColor().g() != 1 ||iNodes.value()->getColor().b() != 1 ||iNodes.value()->getColor().a() != 1 ) {
			if ( meta ) {
				if ( newMetaNodeID.contains( iNodes.value()->getId() ) ) {
					nodeIdIter = newMetaNodeID.find( iNodes.value()->getId() );
					nodeID = nodeIdIter.value();
				}
				else {
					qDebug() << "[Model::NodeDAO::addNodesColorToDB] Node ID: " << iNodes.value()->getId() <<  " mismatch";
				}
			}
			else {
				nodeID = iNodes.value()->getId();
			}

			addSettings( conn, iNodes.value()->getGraph()->getId(), layout->getId(), nodeID, "color_r", iNodes.value()->getColor().r() );
			addSettings( conn, iNodes.value()->getGraph()->getId(), layout->getId(), nodeID, "color_g", iNodes.value()->getColor().g() );
			addSettings( conn, iNodes.value()->getGraph()->getId(), layout->getId(), nodeID, "color_b", iNodes.value()->getColor().b() );
			addSettings( conn, iNodes.value()->getGraph()->getId(), layout->getId(), nodeID, "color_a", iNodes.value()->getColor().a() );
		}

		++iNodes;
	}

	return true;
}

bool Model::NodeDAO::addNodesScaleToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta, float defaultScale )
{
	QMap< qlonglong,osg::ref_ptr<Data::Node> >::const_iterator iNodes = nodes->constBegin();
	qlonglong nodeID;
	QMap<qlonglong, qlonglong>::iterator nodeIdIter;

	//ukladame velkosti jednotlivym uzlom, ktore ju nemaju Default
	while ( iNodes != nodes->constEnd() ) {
		//ulozime scale len nodom, ktore maju velkost inu nez default
		if ( !qFuzzyCompare( iNodes.value()->getScale(),defaultScale ) )
			//if(iNodes.value()->getScale() != defaultScale)
		{
			if ( meta ) {
				if ( newMetaNodeID.contains( iNodes.value()->getId() ) ) {
					nodeIdIter = newMetaNodeID.find( iNodes.value()->getId() );
					nodeID = nodeIdIter.value();
				}
				else {
					qDebug() << "[Model::NodeDAO::addNodesScaleToDB] Node ID: " << iNodes.value()->getId() <<  " mismatch";
				}
			}
			else {
				nodeID = iNodes.value()->getId();
			}

			addSettings( conn, iNodes.value()->getGraph()->getId(), layout->getId(), nodeID, "scale", iNodes.value()->getScale() );
		}

		++iNodes;
	}

	return true;
}

bool Model::NodeDAO::addNodesMaskToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta )
{
	QMap< qlonglong,osg::ref_ptr<Data::Node> >::const_iterator iNodes = nodes->constBegin();
	qlonglong nodeID;
	QMap<qlonglong, qlonglong>::iterator nodeIdIter;
	float scale = 0;

	//ukladame zobrazovanu masku pre uzly, ktore ju nemaju Default
	while ( iNodes != nodes->constEnd() ) {
		//ulozime masku len nodom, ktore ju maju nastavenu, t. j. je rovna 0
		if ( iNodes.value()->getNodeMask() == 0 ) {
			if ( meta ) {
				if ( newMetaNodeID.contains( iNodes.value()->getId() ) ) {
					nodeIdIter = newMetaNodeID.find( iNodes.value()->getId() );
					nodeID = nodeIdIter.value();
				}
				else {
					qDebug() << "[Model::NodeDAO::addNodesMaskToDB] Node ID: " << iNodes.value()->getId() <<  " mismatch";
				}
			}
			else {
				nodeID = iNodes.value()->getId();
			}

			addSettings( conn, iNodes.value()->getGraph()->getId(), layout->getId(), nodeID, "mask", scale );
		}

		++iNodes;
	}

	return true;
}

bool Model::NodeDAO::addNodesParentToDB( QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout, QMap<qlonglong, qlonglong> newMetaNodeID, bool meta )
{
	QMap< qlonglong,osg::ref_ptr<Data::Node> >::const_iterator iNodes = nodes->constBegin();
	qlonglong nodeID;
	QMap<qlonglong, qlonglong>::iterator nodeIdIter;

	//ukladame nadradene uzly k danym uzlom do databazy
	while ( iNodes != nodes->constEnd() ) {
		if ( iNodes.value()->isParentNode() ) {
			if ( meta ) {
				if ( newMetaNodeID.contains( iNodes.value()->getId() ) ) {
					nodeIdIter = newMetaNodeID.find( iNodes.value()->getId() );
					nodeID = nodeIdIter.value();
				}
				else {
					qDebug() << "[Model::NodeDAO::addNodesScaleToDB] Node ID: " << iNodes.value()->getId() <<  " mismatch";
				}
			}
			else {
				nodeID = iNodes.value()->getId();
			}

			addSettings( conn, iNodes.value()->getGraph()->getId(), layout->getId(), nodeID, "is_parent", true );
		}

		++iNodes;
	}

	return true;
}

QSqlQuery* Model::NodeDAO::getNodesQuery( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID, qlonglong parentID )
{
	*error = FALSE;
	QSqlQuery* query;

	//check if we have connection
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::NodeDAO::getNodes] Connection to DB not opened.";
		*error = TRUE;
		return NULL;
	}

	query = new QSqlQuery( *conn );

	//vyberame z databazy vsetky uzly podla ID grafu, layoutu a nadradeneho uzla
	query->prepare( "SELECT * "
					"FROM nodes "
					"WHERE graph_id = :graph_id "
					"AND (layout_id IS NULL OR layout_id = :layout_id) "
					"AND parent_id = :parent_id" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":layout_id", layoutID );
	query->bindValue( ":parent_id", parentID );

	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::getNodes] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return query;
	}

	return query;
}

QMap<qlonglong, osg::Vec3f> Model::NodeDAO::getNodesPositions( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID )
{
	QMap<qlonglong, osg::Vec3f> positions;
	*error = FALSE;
	QSqlQuery* query;
	osg::Vec3f position;

	//check if we have connection
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::NodeDAO::getNodesPositions] Connection to DB not opened.";
		*error = TRUE;
		return positions;
	}

	query = new QSqlQuery( *conn );

	//vyberame z databazy pozicie uzlov podla ID grafu a layoutu
	query->prepare( "SELECT * "
					"FROM positions "
					"WHERE graph_id = :graph_id "
					"AND layout_id = :layout_id" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":layout_id", layoutID );

	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::getNodesPositions] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return positions;
	}

	while ( query->next() ) {
		qlonglong nodeId = query->value( 1 ).toLongLong();
		position = osg::Vec3f( query->value( 2 ).toFloat(), query->value( 3 ).toFloat(), query->value( 4 ).toFloat() );

		positions.insert( nodeId, position );
	}

	return positions;
}

QList<qlonglong> Model::NodeDAO::getListOfNodes( QSqlDatabase* conn, bool* error )
{
	QList<qlonglong> nodes;
	*error = FALSE;

	//check if we have connection
	if ( conn==NULL || !conn->isOpen() )

	{
		qDebug() << "[Model::NodeDAO::getListOfNodes] Connection to DB not opened.";
		*error = TRUE;
		return nodes;
	}

	//vyberame zoznam uzlov z databazy podla ID grafu
	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "SELECT graph_id "
					"FROM nodes " );

	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::getListOfNodes] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return nodes;
	}

	while ( query->next() ) {
		nodes << query->value( 0 ).toLongLong();
	}

	return nodes;
}

bool Model::NodeDAO::checkIfExists( Data::Node* node, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::NodeDAO::checkIfExists] Connection to DB not opened.";
		return ( bool* )NULL;
	}
	else if ( node==NULL ) {
		qDebug() << "[Model::NodeDAO::checkIfExists] Invalid parameter - node is NULL.";
		return false;
	}
	else if ( node->getGraph()==NULL ) {
		qDebug() << "[Model::NodeDAO::checkIfExists] Node does not have graph assigned.";
		return false;
	}
	else if ( !node->isInDB() || ( node->getGraph()!=NULL && !node->getGraph()->isInDB() ) ) {
		return false;
	}

	//overujeme ci su v databaze uzly podla ID
	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "SELECT COUNT(1) FROM nodes "
					"WHERE node_id=:node_id AND graph_id=:graph_id" );
	query->bindValue( ":node_id", node->getId() );
	query->bindValue( ":graph_id", node->getGraph()->getId() );
	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::checkIfExists] Could not perform query on DB: " << query->lastError().databaseText();
		return ( bool* )NULL;
	}
	if ( query->next() && query->value( 0 )==1 ) {
		return true;
	}
	else {
		return false;
	}
}

bool Model::NodeDAO::removeNode( Data::Node* node, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::NodeDAO::removeNode] Connection to DB not opened.";
		return ( bool* )NULL;
	}
	else if ( node==NULL ) {
		qDebug() << "[Model::NodeDAO::removeNode] Invalid parameter - node is NULL.";
		return false;
	}
	else if ( node->getGraph()==NULL ) {
		qDebug() << "[Model::NodeDAO::removeNode] Node does not have graph.";
		return false;
	}
	else if ( !node->isInDB() || ( node->getGraph()!=NULL && !node->getGraph()->isInDB() ) ) {
		return false;
	}

	//odstranujeme uzol z databazy podla jeho ID
	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "DELETE FROM nodes WHERE graph_id = :graph_id AND node_id = :node_id" );
	query->bindValue( ":graph_id", node->getGraph()->getId() );
	query->bindValue( ":node_id", node->getId() );
	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::removeNode] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

bool Model::NodeDAO::removeNodes( qlonglong graphID, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::NodeDAO::removeNodes] Connection to DB not opened.";
		return ( bool* )NULL;
	}

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "DELETE FROM node_settings WHERE graph_id = :graph_id" );
	query->bindValue( ":graph_id", graphID );
	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::removeNodes] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	//odstranujeme uzly z databazy podla ID grafu
	query->prepare( "DELETE FROM nodes WHERE graph_id = :graph_id" );
	query->bindValue( ":graph_id", graphID );
	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::removeNodes] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

bool Model::NodeDAO::removeNodes( qlonglong graphID, qlonglong layoutID, QSqlDatabase* conn )
{
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::NodeDAO::removeNodes] Connection to DB not opened.";
		return ( bool* )NULL;
	}

	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "DELETE FROM node_settings WHERE graph_id = :graph_id AND layout_id = :layout_id" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":layout_id", layoutID );
	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::removeNodes] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	//odstranujeme uzly z databazy podla ID grafu a layoutu
	query->prepare( "DELETE FROM nodes WHERE graph_id = :graph_id AND layout_id = :layout_id" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":layout_id", layoutID );
	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::removeNodes] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

QMap<QString,QString> Model::NodeDAO::getSettings( Data::Node* node, QSqlDatabase* conn, bool* error )
{
	QMap<QString,QString> settings;
	*error = FALSE;

	if ( conn==NULL || !conn->isOpen() ) { //check if we have connection
		qDebug() << "[Model::NodeDAO::getSettings] Connection to DB not opened.";
		*error = TRUE;
		return settings;
	}

	if ( node==NULL ) {
		qDebug() << "[Model::NodeDAO::getSettings] Invalid parameter - Node is NULL";
		*error = TRUE;
		return settings;
	}

	if ( !node->isInDB() ) {
		qDebug() << "[Model::NodeDAO::getSettings] Node is not in DB";
		*error = TRUE;
		return settings;
	}

	//vyberame nastavenia z databazy podla ID grafu a uzla
	QSqlQuery* query = new QSqlQuery( *conn );
	query->prepare( "SELECT val_name, val FROM node_settings WHERE graph_id = :graph_id AND node_id = :node_id" );
	query->bindValue( ":graph_id",node->getGraph()->getId() );
	query->bindValue( ":node_id",node->getId() );
	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::getSettings] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return settings;
	}

	while ( query->next() ) {
		settings.insert( query->value( 0 ).toString(),query->value( 1 ).toString() );
	}

	return settings;
}

QMap<qlonglong, QString> Model::NodeDAO::getSettings( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID, QString attributeName )
{
	*error = FALSE;
	QSqlQuery* query;
	QMap<qlonglong, QString> settings;

	//check if we have connection
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::NodeDAO::getSettings] Connection to DB not opened.";
		*error = TRUE;
		return settings;
	}

	//vyberame nastavenia z databazy podla ID grafu, uzla a nazvu nastavenia
	query = new QSqlQuery( *conn );
	query->prepare( "SELECT node_id, val "
					"FROM node_settings "
					"WHERE graph_id = :graph_id "
					"AND val_name = :attribute_name "
					"AND layout_id = :layout_id" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":attribute_name", attributeName );
	query->bindValue( ":layout_id", layoutID );

	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::getSettings] Could not perform query on DB: " << query->lastError().databaseText();
		*error = TRUE;
		return settings;
	}

	while ( query->next() ) {
		settings.insert( query->value( 0 ).toLongLong(), query->value( 1 ).toString() );
	}

	return settings;
}

QMap<qlonglong, osg::Vec4f> Model::NodeDAO::getColors( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID )
{
	*error = FALSE;
	bool error2 = false;
	osg::Vec4f color;

	QMap<qlonglong, osg::Vec4f> colors;

	QMap<qlonglong, QString> nodeColorR;
	QMap<qlonglong, QString> nodeColorG;
	QMap<qlonglong, QString> nodeColorB;
	QMap<qlonglong, QString> nodeColorA;
	QMap<qlonglong, QString>::iterator iter_r;
	QMap<qlonglong, QString>::iterator iter_g;
	QMap<qlonglong, QString>::iterator iter_b;
	QMap<qlonglong, QString>::iterator iter_a;

	nodeColorR = getSettings( conn, &error2, graphID, layoutID, "color_r" );
	if ( error2 ) {
		qDebug() << "[Model::NodeDAO::getColors] Could not get color_r setting";
		*error = error2;
		return colors;
	}
	nodeColorG = getSettings( conn, &error2, graphID, layoutID, "color_g" );
	if ( error2 ) {
		qDebug() << "[Model::NodeDAO::getColors] Could not get color_g setting";
		*error = error2;
		return colors;
	}
	nodeColorB = getSettings( conn, &error2, graphID, layoutID, "color_b" );
	if ( error2 ) {
		qDebug() << "[Model::NodeDAO::getColors] Could not get color_b setting";
		*error = error2;
		return colors;
	}
	nodeColorA = getSettings( conn, &error2, graphID, layoutID, "color_a" );
	if ( error2 ) {
		qDebug() << "[Model::NodeDAO::getColors] Could not get color_a setting";
		*error = error2;
		return colors;
	}

	//nacitavame z databazy farby podla ID grafu a layoutu
	for ( iter_r = nodeColorR.begin(); iter_r != nodeColorR.end(); ++iter_r ) {
		qlonglong id = iter_r.key();
		iter_g = nodeColorG.find( id );
		iter_b = nodeColorB.find( id );
		iter_a = nodeColorA.find( id );

		color = osg::Vec4f( iter_r.value().toFloat(), iter_g.value().toFloat(), iter_b.value().toFloat(), iter_a.value().toFloat() );
		colors.insert( id, color );
	}

	*error = error2;

	return colors;
}

QMap<qlonglong, float> Model::NodeDAO::getScales( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID )
{
	*error = FALSE;
	bool error2 = false;
	QMap<qlonglong, float> scales;

	QMap<qlonglong, QString> nodeScale;
	QMap<qlonglong, QString>::iterator iter;

	nodeScale = getSettings( conn, &error2, graphID, layoutID, "scale" );
	if ( !error2 ) {
		//nacitavame z databazy velkost layoutu a rozlozenia grafu
		for ( iter = nodeScale.begin(); iter != nodeScale.end(); ++iter ) {
			qlonglong id = iter.key();

			float scale = iter.value().toFloat();
			scales.insert( id, scale );
		}
	}
	else {
		qDebug() << "[Model::NodeDAO::getScales] Could not get scale setting";
	}
	*error = error2;

	return scales;
}

QMap<qlonglong, int> Model::NodeDAO::getMasks( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID )
{
	*error = FALSE;
	bool error2 = false;
	QMap<qlonglong, int> masks;

	QMap<qlonglong, QString> nodeMask;
	QMap<qlonglong, QString>::iterator iter;

	nodeMask = getSettings( conn, &error2, graphID, layoutID, "mask" );
	if ( !error2 ) {
		//nacitavame z databazy masky uzlov
		for ( iter = nodeMask.begin(); iter != nodeMask.end(); ++iter ) {
			qlonglong id = iter.key();

			int mask = iter.value().toInt();
			masks.insert( id, mask );
		}
	}
	else {
		qDebug() << "[Model::NodeDAO::getMasks] Could not get mask setting";
	}

	*error = error2;

	return masks;
}

QList<qlonglong> Model::NodeDAO::getParents( QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID )
{
	*error = FALSE;
	bool error2 = false;
	//bool isParent;
	QList<qlonglong> parents;

	QMap<qlonglong, QString> nodeParents;
	QMap<qlonglong, QString>::iterator iter;

	nodeParents = getSettings( conn, &error2, graphID, layoutID, "is_parent" );
	if ( !error2 ) {
		//nacitavame z databazy rodicovske/nadradene uzly
		for ( iter = nodeParents.begin(); iter != nodeParents.end(); ++iter ) {
			qlonglong id = iter.key();
			parents << id;
		}
	}
	else {
		qDebug() << "[Model::NodeDAO::getParents] Could not get is_parent setting";
	}

	*error = error2;

	return parents;
}

QMap<qlonglong, qlonglong> Model::NodeDAO::getNewMetaNodesId( QSqlDatabase* conn, qlonglong graphID, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes )
{
	QMap<qlonglong, qlonglong> newId;
	qlonglong maxId = 0;
	QSqlQuery* query;

	QMap< qlonglong, osg::ref_ptr<Data::Node> >::const_iterator iNodes = nodes->constBegin();

	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::NodeDAO::getNewMetaNodesId] Connection to DB not opened.";
		return newId;
	}

	query = new QSqlQuery( *conn );
	query->prepare( "SELECT MAX(node_id) "
					"FROM nodes "
					"WHERE graph_id = :graph_id " );
	query->bindValue( ":graph_id", graphID );

	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::getNewMetaNodesId] Could not perform query on DB: " << query->lastError().databaseText();
		return newId;
	}

	while ( query->next() ) {
		maxId = query->value( 0 ).toLongLong();
	}

	//vytvarame zoznam novych meta uzlov
	while ( iNodes != nodes->constEnd() ) {
		maxId++;
		newId.insert( iNodes.value()->getId(), maxId );
		++iNodes;
	}

	return newId;
}

bool Model::NodeDAO::addSettings( QSqlDatabase* conn, qlonglong graphID, qlonglong layoutID, qlonglong nodeID, QString valName, double val )
{
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::NodeDAO::addSettings] Connection to DB not opened.";
		return false;
	}

	QSqlQuery* query = new QSqlQuery( *conn );

	//ukladame do databazy nazvy nastaveni
	query->prepare( "INSERT INTO node_settings (graph_id, node_id, val_name, val, layout_id) VALUES (:graph_id, :node_id, :val_name, :val, :layout_id)" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":node_id", nodeID );
	query->bindValue( ":val_name", valName );
	query->bindValue( ":val", val );
	query->bindValue( ":layout_id", layoutID );

	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::addSettings] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

bool Model::NodeDAO::addSettings( QSqlDatabase* conn, qlonglong graphID, qlonglong layoutID, qlonglong nodeID, QString valName, float val )
{
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::NodeDAO::addSettings] Connection to DB not opened.";
		return false;
	}

	QSqlQuery* query = new QSqlQuery( *conn );

	//ukladame do databazy jednotlive nastavenia
	query->prepare( "INSERT INTO node_settings (graph_id, node_id, val_name, val, layout_id) VALUES (:graph_id, :node_id, :val_name, :val, :layout_id)" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":node_id", nodeID );
	query->bindValue( ":val_name", valName );
	query->bindValue( ":val", val );
	query->bindValue( ":layout_id", layoutID );

	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::addSettings] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

bool Model::NodeDAO::addSettings( QSqlDatabase* conn, qlonglong graphID, qlonglong layoutID, qlonglong nodeID, QString valName, bool val )
{
	if ( conn==NULL || !conn->isOpen() ) {
		qDebug() << "[Model::NodeDAO::addSettings] Connection to DB not opened.";
		return false;
	}

	QSqlQuery* query = new QSqlQuery( *conn );

	//pridavame nastavenie do databazy
	query->prepare( "INSERT INTO node_settings (graph_id, node_id, val_name, val, layout_id) VALUES (:graph_id, :node_id, :val_name, :val, :layout_id)" );
	query->bindValue( ":graph_id", graphID );
	query->bindValue( ":node_id", nodeID );
	query->bindValue( ":val_name", valName );
	query->bindValue( ":val", val );
	query->bindValue( ":layout_id", layoutID );

	if ( !query->exec() ) {
		qDebug() << "[Model::NodeDAO::addSettings] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif
