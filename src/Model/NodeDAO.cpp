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

bool Model::NodeDAO::addNodesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes)
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
		query->prepare("INSERT INTO nodes (node_id, name, type_id, graph_id, meta, fixed) VALUES (:node_id, :name, :type_id, :graph_id, :meta, :fixed)");
		query->bindValue(":node_id", iNodes.value()->getId());
		query->bindValue(":name", iNodes.value()->getName());
		query->bindValue(":type_id", iNodes.value()->getType()->getId()); 
		query->bindValue(":graph_id", iNodes.value()->getGraph()->getId());
		query->bindValue(":meta", false);
		query->bindValue(":fixed", iNodes.value()->isFixed());

		if(!query->exec()) {
			qDebug() << "[Model::NodeDAO::addNodesToDB] Could not perform query on DB: " << query->lastError().databaseText();
			return false;
		}

		++iNodes;
	}

	qDebug() << "[Model::NodeDAO::addNodesToDB] " << nodes->count() << " nodes were saved to DB.";

	return true;
}

bool Model::NodeDAO::addMetaNodesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout)
{
	//check if we have connection
	if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::NodeDAO::addMetaNodesToDB] Connection to DB not opened.";
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
		query->bindValue(":meta", true);
		query->bindValue(":fixed", iNodes.value()->isFixed());
		query->bindValue(":layout_id", layout->getId());

		if(!query->exec()) {
			qDebug() << "[Model::NodeDAO::addMetaNodesToDB] Could not perform query on DB: " << query->lastError().databaseText();
			return false;
		}

		++iNodes;
	}

	qDebug() << "[Model::NodeDAO::addMetaNodesToDB] " << nodes->count() << " meta nodes were saved to DB.";

	return true;
}

bool Model::NodeDAO::addNodesPositionsToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout)
{
	//check if we have connection
	if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::NodeDAO::addNodesPositionsToDB] Connection to DB not opened.";
        return false;
    }

	QMap< qlonglong,osg::ref_ptr<Data::Node> >::const_iterator iNodes = nodes->constBegin();

	QSqlQuery* query = new QSqlQuery(*conn);
	
	while(iNodes != nodes->constEnd()) 
	{
		query->prepare("INSERT INTO positions (layout_id, node_id, pos_x, pos_y, pos_z, graph_id) VALUES (:layout_id, :node_id, :pos_x, :pos_y, :pos_z, :graph_id) RETURNING  node_id");
		query->bindValue(":layout_id", layout->getId()); 
		query->bindValue(":node_id", iNodes.value()->getId());
		query->bindValue(":pos_x", iNodes.value()->getCurrentPosition().x());
		query->bindValue(":pos_y", iNodes.value()->getCurrentPosition().y());
		query->bindValue(":pos_z", iNodes.value()->getCurrentPosition().z());
		query->bindValue(":graph_id", iNodes.value()->getGraph()->getId());

		if(!query->exec()) {
			qDebug() << "[Model::NodeDAO::addNodesPositionsToDB] Could not perform query on DB: " << query->lastError().databaseText();
			return false;
		}

		++iNodes;
	}

	return true;
}

bool Model::NodeDAO::addNodesColorToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, Data::GraphLayout* layout)
{
	QMap< qlonglong,osg::ref_ptr<Data::Node> >::const_iterator iNodes = nodes->constBegin();
	
	while(iNodes != nodes->constEnd()) 
	{
		//ulozime farbu len nodom, ktore maju farbu inu nez default
		if(iNodes.value()->getColor().r() != 1 || iNodes.value()->getColor().g() != 1 ||iNodes.value()->getColor().b() != 1 ||iNodes.value()->getColor().a() != 1)
		{
			addColorToDB(conn, iNodes.value()->getGraph()->getId(), layout->getId(), iNodes.value()->getId(), "color_r", iNodes.value()->getColor().r());
			addColorToDB(conn, iNodes.value()->getGraph()->getId(), layout->getId(), iNodes.value()->getId(), "color_g", iNodes.value()->getColor().g());
			addColorToDB(conn, iNodes.value()->getGraph()->getId(), layout->getId(), iNodes.value()->getId(), "color_b", iNodes.value()->getColor().b());
			addColorToDB(conn, iNodes.value()->getGraph()->getId(), layout->getId(), iNodes.value()->getId(), "color_a", iNodes.value()->getColor().a());
		}

		++iNodes;
	}
	
	return true;
}

bool Model::NodeDAO::addColorToDB(QSqlDatabase* conn, qlonglong graphID, qlonglong layoutID, qlonglong nodeID, QString valName, double val)
{
	if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::NodeDAO::addColorToDB] Connection to DB not opened.";
        return false;
    }

	QSqlQuery* query = new QSqlQuery(*conn);
	
	query->prepare("INSERT INTO node_settings (graph_id, node_id, val_name, val, layout_id) VALUES (:graph_id, :node_id, :val_name, :val, :layout_id)");
	query->bindValue(":graph_id", graphID); 
	query->bindValue(":node_id", nodeID);
	query->bindValue(":val_name", valName);
	query->bindValue(":val", val);
	query->bindValue(":layout_id", layoutID);

	if(!query->exec()) {
		qDebug() << "[Model::NodeDAO::addColorToDB] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

QSqlQuery* Model::NodeDAO::getNodesQuery(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID)
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

    query = new QSqlQuery(*conn);
    query->prepare("SELECT * "
		"FROM nodes "
		"WHERE graph_id = :graph_id "
		"AND (layout_id IS NULL OR layout_id = :layout_id)");
	query->bindValue(":graph_id", graphID);
	query->bindValue(":layout_id", layoutID);

    if(!query->exec()) {
        qDebug() << "[Model::NodeDAO::getNodes] Could not perform query on DB: " << query->lastError().databaseText();
        *error = TRUE;
        return query;
    }
    
    return query;
}

QSqlQuery* Model::NodeDAO::getNodesPositionsQuery(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID)
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
		"WHERE graph_id = :graph_id "
		"AND layout_id = :layout_id"); 
	query->bindValue(":graph_id", graphID);
	query->bindValue(":layout_id", layoutID);

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
        qDebug() << "[Model::NodeDAO::getListOfNodes] Connection to DB not opened.";
        *error = TRUE;
        return nodes;
    }

    //get nodes from DB
    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("SELECT graph_id " 
		"FROM nodes ");

    if(!query->exec()) 
	{
        qDebug() << "[Model::NodeDAO::getListOfNodes] Could not perform query on DB: " << query->lastError().databaseText();
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

QMap<qlonglong, QString> Model::NodeDAO::getSettings(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID, QString attributeName)
{
    *error = FALSE;
	QSqlQuery* query;
	QMap<qlonglong, QString> settings;

	//check if we have connection
    if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::NodeDAO::getSettings] Connection to DB not opened.";
        *error = TRUE;
        return settings;
    }

    query = new QSqlQuery(*conn);
    query->prepare("SELECT node_id, val "
		"FROM node_settings "
		"WHERE graph_id = :graph_id "
		"AND val_name = :attribute_name "
		"AND layout_id = :layout_id"); 
	query->bindValue(":graph_id", graphID);
	query->bindValue(":attribute_name", attributeName);
	query->bindValue(":layout_id", layoutID);

	if(!query->exec()) {
        qDebug() << "[Model::NodeDAO::getSettings] Could not perform query on DB: " << query->lastError().databaseText();
        *error = TRUE;
        return settings;
    }

    while(query->next()) {
		settings.insert(query->value(0).toLongLong(), query->value(1).toString());
    }
    
    return settings;
}

QMap<qlonglong, osg::Vec4f> Model::NodeDAO::getColors(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID)
{
    *error = FALSE;
	bool error2 = false;
	osg::Vec4f color;
	qlonglong id;
	QMap<qlonglong, osg::Vec4f> colors;

	QMap<qlonglong, QString> nodeColorR;
	QMap<qlonglong, QString> nodeColorG;
	QMap<qlonglong, QString> nodeColorB;
	QMap<qlonglong, QString> nodeColorA;
	QMap<qlonglong, QString>::iterator iter_r;
	QMap<qlonglong, QString>::iterator iter_g;
	QMap<qlonglong, QString>::iterator iter_b;
	QMap<qlonglong, QString>::iterator iter_a;

	nodeColorR = getSettings(conn, &error2, graphID, layoutID, "color_r");
	nodeColorG = getSettings(conn, &error2, graphID, layoutID, "color_g");
	nodeColorB = getSettings(conn, &error2, graphID, layoutID, "color_b");
	nodeColorA = getSettings(conn, &error2, graphID, layoutID, "color_a");

	for(iter_r = nodeColorR.begin(); iter_r != nodeColorR.end(); iter_r++) 
	{
		id = iter_r.key();
		iter_g = nodeColorG.find(id);
		iter_b = nodeColorB.find(id);
		iter_a = nodeColorA.find(id);

		color = osg::Vec4f(iter_r.value().toFloat(), iter_g.value().toFloat(), iter_b.value().toFloat(), iter_a.value().toFloat());
		colors.insert(id, color);
	}

	*error = error2;

	return colors;
}