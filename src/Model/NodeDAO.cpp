/*!
 * NodeDAO.cpp
 * Projekt 3DVisual
 */
#include "Model/NodeDAO.h"

Model::NodeDAO::NodeDAO(void)
{
}

Model::NodeDAO::~NodeDAO(void)
{
}

bool Model::NodeDAO::addNodesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, bool isMeta, Data::GraphLayout* layout)
{
	//check if we have connection
	if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::NodeDAO::addNodesToDB] Connection to DB not opened.";
        return false;
    }

	QMap< qlonglong,osg::ref_ptr<Data::Node> >::const_iterator iNodes = nodes->constBegin();

	QSqlQuery* query = new QSqlQuery(*conn);
	
	while(iNodes != nodes->constEnd()) 
	{
		query->prepare("INSERT INTO nodes (node_id, name, type_id, graph_id, meta, fixed, layout_id) VALUES (:node_id, :name, :type_id, :graph_id, :meta, :fixed, :layout_id)");
		query->bindValue(":node_id", iNodes.value()->getId());
		query->bindValue(":name", iNodes.value()->getName());
		query->bindValue(":type_id", iNodes.value()->getType()->getId()); 
		query->bindValue(":graph_id", iNodes.value()->getGraph()->getId());
		query->bindValue(":meta", isMeta);
		query->bindValue(":fixed", iNodes.value()->isFixed());
		query->bindValue(":layout_id", layout->getId());

		if(!query->exec()) {
			qDebug() << "[Model::NodeDAO::addNodesToDB] Could not perform query on DB: " << query->lastError().databaseText();
			return false;
		}

		query->prepare("INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z, graph_id) VALUES (:layout_id, :node_id, :pos_x, :pos_y, :pos_z, :graph_id) RETURNING  node_id");
		query->bindValue(":layout_id", layout->getId()); 
		query->bindValue(":node_id", iNodes.value()->getId());
		query->bindValue(":pos_x", iNodes.value()->getCurrentPosition().x());
		query->bindValue(":pos_y", iNodes.value()->getCurrentPosition().y());
		query->bindValue(":pos_z", iNodes.value()->getCurrentPosition().z());
		query->bindValue(":graph_id", iNodes.value()->getGraph()->getId());

		if(!query->exec()) {
			qDebug() << "[Model::NodeDAO::addNodesToDB] Could not perform query on DB: " << query->lastError().databaseText();
			return false;
		}
		++iNodes;
	}
	if(isMeta)
		qDebug() << "[Model::NodeDAO::addNodesToDB] " << nodes->count() << " meta nodes were saved to DB.";
	else
		qDebug() << "[Model::NodeDAO::addNodesToDB] " << nodes->count() << " nodes were saved to DB.";

	return true;
}

QSqlQuery* Model::NodeDAO::getNodesQuery(QSqlDatabase* conn, bool* error, qlonglong graphID)
{
    *error = FALSE;
	QSqlQuery* query;

	//check if we have connection
    if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::NodeDAO::getNodes] Connection to DB not opened.";
        *error = TRUE;
        return query;
    }

    //get nodes from DB
    query = new QSqlQuery(*conn);
    query->prepare("SELECT * "
		"FROM nodes "
		"WHERE graph_id = :graph_id");
	query->bindValue(":graph_id", graphID);

    if(!query->exec()) {
        qDebug() << "[Model::NodeDAO::getNodes] Could not perform query on DB: " << query->lastError().databaseText();
        *error = TRUE;
        return query;
    }
    
    return query;
}

QSqlQuery* Model::NodeDAO::getNodesPositionsQuery(QSqlDatabase* conn, bool* error, qlonglong graphID)
{
    *error = FALSE;
	QSqlQuery* query;

	//check if we have connection
    if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::NodeDAO::getNodesPositions] Connection to DB not opened.";
        *error = TRUE;
        return query;
    }

    //get nodes positions from DB
    query = new QSqlQuery(*conn);
    query->prepare("SELECT * "
		"FROM positions "
		"WHERE graph_id = :graph_id");
	query->bindValue(":graph_id", graphID);

    if(!query->exec()) {
        qDebug() << "[Model::NodeDAO::getNodesPositions] Could not perform query on DB: " << query->lastError().databaseText();
        *error = TRUE;
        return query;
    }
    
    return query;
}

QList<qlonglong> Model::NodeDAO::getListOfNodes(QSqlDatabase* conn, bool* error)
{
	QList<qlonglong> nodes;
    *error = FALSE;

	//check if we have connection
    if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::NodeDAO::getNumberOfNodes] Connection to DB not opened.";
        *error = TRUE;
        return nodes;
    }

    //get nodes from DB
    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("SELECT graph_id " 
		"FROM nodes ");

    if(!query->exec()) 
	{
        qDebug() << "[Model::NodeDAO::getNumberOfNodes] Could not perform query on DB: " << query->lastError().databaseText();
        *error = TRUE;
        return nodes;
    }
    
    while(query->next()) {
		nodes << query->value(0).toLongLong();
    }

    return nodes;
}

bool Model::NodeDAO::checkIfExists(Data::Node* node, QSqlDatabase* conn)
{
    if(conn==NULL || !conn->isOpen()) { //check if we have connection
        qDebug() << "[Model::NodeDAO::checkIfExists] Connection to DB not opened.";
        return NULL;
    } else if(node==NULL) {
        qDebug() << "[Model::NodeDAO::checkIfExists] Invalid parameter - node is NULL.";
        return false;
    } else if(node->getGraph()==NULL) {
        qDebug() << "[Model::NodeDAO::checkIfExists] Node does not have graph assigned.";
        return false;
    } else if(!node->isInDB() || (node->getGraph()!=NULL && !node->getGraph()->isInDB())) {
        return false;
    }

    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("SELECT COUNT(1) FROM nodes "
        "WHERE node_id=:node_id AND graph_id=:graph_id");
    query->bindValue(":node_id", node->getId());
    query->bindValue(":graph_id", node->getGraph()->getId());
    if(!query->exec()) {
        qDebug() << "[Model::NodeDAO::checkIfExists] Could not perform query on DB: " << query->lastError().databaseText();
        return NULL;
    }
    if(query->next() && query->value(0)==1) {
        return true;
    } else return false;
}

bool Model::NodeDAO::removeNode( Data::Node* node, QSqlDatabase* conn )
{
    if(conn==NULL || !conn->isOpen()) { //check if we have connection
        qDebug() << "[Model::NodeDAO::removeNode] Connection to DB not opened.";
        return NULL;
    } else if(node==NULL) {
        qDebug() << "[Model::NodeDAO::removeNode] Invalid parameter - node is NULL.";
        return false;
    } else if(node->getGraph()==NULL) {
        qDebug() << "[Model::NodeDAO::removeNode] Node does not have graph.";
        return false;
    } else if(!node->isInDB() || (node->getGraph()!=NULL && !node->getGraph()->isInDB())) {
        return false;
    }
    
    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("DELETE FROM nodes WHERE graph_id = :graph_id AND node_id = :node_id");
    query->bindValue(":graph_id", node->getGraph()->getId());
    query->bindValue(":node_id", node->getId());
    if(!query->exec()) {
        qDebug() << "[Model::NodeDAO::removeNode] Could not perform query on DB: " << query->lastError().databaseText();
        return false;
    }

    return true;
}

QMap<QString,QString> Model::NodeDAO::getSettings( Data::Node* node, QSqlDatabase* conn, bool* error )
{
    QMap<QString,QString> settings;
    *error = FALSE;

    if(conn==NULL || !conn->isOpen()) { //check if we have connection
        qDebug() << "[Model::NodeDAO::getSettings] Connection to DB not opened.";
        *error = TRUE;
        return settings;
    }

    if(node==NULL) {
        qDebug() << "[Model::NodeDAO::getSettings] Invalid parameter - Node is NULL";
        *error = TRUE;
        return settings;
    }

    if(!node->isInDB()) {
        qDebug() << "[Model::NodeDAO::getSettings] Node is not in DB";
        *error = TRUE;
        return settings;
    }

    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("SELECT val_name, val FROM node_settings WHERE graph_id = :graph_id AND node_id = :node_id");
    query->bindValue(":graph_id",node->getGraph()->getId());
    query->bindValue(":node_id",node->getId());
    if(!query->exec()) {
        qDebug() << "[Model::NodeDAO::getSettings] Could not perform query on DB: " << query->lastError().databaseText();
        *error = TRUE;
        return settings;
    }

    while(query->next()) {
        settings.insert(query->value(0).toString(),query->value(1).toString());
    }

    return settings;
}