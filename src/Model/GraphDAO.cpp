/*!
 * GraphDAO.cpp
 * Projekt 3DVisual
 */
#include "Model/GraphDAO.h"

Model::GraphDAO::GraphDAO(void)
{
}

Model::GraphDAO::~GraphDAO(void)
{
}

QMap<qlonglong, Data::Graph*> Model::GraphDAO::getGraphs(QSqlDatabase* conn, bool* error)
{
    QMap<qlonglong, Data::Graph*> qgraphs;
    *error = FALSE;

    if(conn==NULL || !conn->isOpen()) { //check if we have connection
        qDebug() << "[Model::GraphDAO::getGraphs] Connection to DB not opened.";
        *error = TRUE;
        return qgraphs;
    }
    
    //get all graphs with their max element id
    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("SELECT g.graph_id, g.graph_name, (CASE WHEN MAX(l.layout_id) IS NOT NULL THEN MAX(l.layout_id) ELSE 0 END) AS layout_id, (CASE WHEN MAX(ele_id) IS NOT NULL THEN MAX(ele_id) ELSE 0 END) AS ele_id FROM ("
        "("
        "SELECT MAX(node_id) AS ele_id, graph_id FROM nodes "
        "GROUP BY graph_id"
        ") UNION ALL ("
        "SELECT MAX(edge_id) AS ele_id, graph_id FROM edges "
        "GROUP BY graph_id"
        ")"
        ") AS foo "
        "RIGHT JOIN graphs AS g ON "
        "g.graph_id = foo.graph_id "
        "LEFT JOIN layouts AS l ON "
        "l.graph_id = g.graph_id "
        "GROUP BY g.graph_id, g.graph_name");

    if(!query->exec()) {
        qDebug() << "[Model::GraphDAO::getGraphs] Could not perform query on DB: " << query->lastError().databaseText();
        *error = TRUE;
        return qgraphs;
    }
    
    while(query->next()) {
        qgraphs.insert(query->value(0).toLongLong(), new Data::Graph(query->value(0).toLongLong(),query->value(1).toString(),query->value(2).toLongLong(),query->value(3).toLongLong(),conn));
    }
    return qgraphs;
}


void getNestedGraph(qlonglong parentID, Data::Graph** graph, QSqlDatabase* conn, bool* error2, qlonglong graphID, qlonglong layoutID, qlonglong* maxIdEleUsed, QMap<qlonglong, osg::Vec3f>* positions, QMap<qlonglong, Data::Node*>* nodes, Data::Type* typeNode, Data::Type* typeMetaNode, QList<qlonglong>* parentNodes)
{
	bool error;
	qlonglong nodeID;
	QString nodeName;
	Data::Type* type;
	osg::Vec3f position;
	QSqlQuery* queryNestedNodes;
	Data::Node* newNestedNode;

	queryNestedNodes = Model::NodeDAO::getNodesQuery(conn, &error, graphID, layoutID, parentID);

	//TODO pridat vnorenym nodom atributy - scale, farbu, ...
	while(queryNestedNodes->next()) 
	{
		nodeID = queryNestedNodes->value(0).toLongLong();
		nodeName = queryNestedNodes->value(1).toString();
		type = queryNestedNodes->value(4).toBool() ? typeMetaNode : typeNode;
		if(*maxIdEleUsed < nodeID)
			*maxIdEleUsed = nodeID + 1;
			
		if((*positions).contains(nodeID))
			position = (*positions).value(nodeID);

		newNestedNode = (*graph)->addNode(nodeID, nodeName, type, position);
		(*nodes).insert(nodeID, newNestedNode);
		(*graph)->addNestedNode(newNestedNode);

		if ((*parentNodes).contains(nodeID))
		{
			(*graph)->createNestedGraph(newNestedNode);

			getNestedGraph(nodeID, graph, conn, &error, graphID, layoutID, maxIdEleUsed, positions, nodes, typeNode, typeMetaNode, parentNodes);

			(*graph)->closeNestedGraph();
		}
	}
}

Data::Graph* Model::GraphDAO::getGraph(QSqlDatabase* conn, bool* error2, qlonglong graphID, qlonglong layoutID)
{
	Data::Graph* newGraph;
	Data::GraphLayout* newLayout;
	QSqlQuery* queryNodes;
	QSqlQuery* queryEdges;
	QString graphName, layoutName, nodeName, edgeName;
	bool error = false, isFixed, isOriented;
	qlonglong nodeID1, nodeID2, nodeID, edgeID, maxIdEleUsed = 0;
	Data::Type *type;
	QMap<qlonglong, Data::Node*> nodes;
	QMap<qlonglong, Data::Node*>::iterator iNodes1;
	QMap<qlonglong, Data::Node*>::iterator iNodes2;
	Data::Node* newNode; 
	osg::Vec3f position;
	QMap<qlonglong, osg::Vec3f> positions;
	QMap<qlonglong, osg::Vec4> nodeColors;
	QMap<qlonglong, osg::Vec4f> edgeColors;
	QMap<qlonglong, float> nodeScales;
	QMap<qlonglong, float> edgeScales;
	QMap<qlonglong, int> nodeMasks;
	QList<qlonglong> parentNodes;

	graphName = Model::GraphDAO::getName(graphID, &error, conn);
	layoutName = Model::GraphLayoutDAO::getName(conn, &error, graphID, layoutID);
	queryNodes = Model::NodeDAO::getNodesQuery(conn, &error, graphID, layoutID, -1);
	queryEdges = Model::EdgeDAO::getEdgesQuery(conn, &error, graphID, layoutID);
	positions = Model::NodeDAO::getNodesPositions(conn, &error, graphID, layoutID);
	nodeColors = Model::NodeDAO::getColors(conn, &error, graphID, layoutID);
	edgeColors = Model::EdgeDAO::getColors(conn, &error, graphID, layoutID);
	nodeScales = Model::NodeDAO::getScales(conn, &error, graphID, layoutID);
	edgeScales = Model::EdgeDAO::getScales(conn, &error, graphID, layoutID);
	nodeMasks = Model::NodeDAO::getMasks(conn, &error, graphID, layoutID);
	parentNodes = Model::NodeDAO::getParents(conn, &error, graphID, layoutID);
		
	if(!error)
	{
		qDebug() << "[Model::GraphDAO::getGraph] Data loaded from database successfully";

		newGraph = new Data::Graph(graphID, graphName, 0, 0, NULL);
		newLayout = new Data::GraphLayout(layoutID, newGraph, layoutName, conn);
		newGraph->selectLayout(newLayout);

 		Data::Type *typeNode = newGraph->addType("node");
		Data::Type *typeEdge = newGraph->addType("edge");
		Data::Type *typeMetaNode = newGraph->getNodeMetaType();
		Data::Type *typeMetaEdge = newGraph->getEdgeMetaType();

		while(queryNodes->next()) 
		{
			nodeID = queryNodes->value(0).toLongLong();
			nodeName = queryNodes->value(1).toString();
			type = queryNodes->value(4).toBool() ? typeMetaNode : typeNode;
			isFixed = queryNodes->value(5).toBool();

			if(maxIdEleUsed < nodeID)
				maxIdEleUsed = nodeID + 1;

			if(positions.contains(nodeID))
			{
				position = positions.value(nodeID);
			}

			newNode = newGraph->addNode(nodeID, nodeName, type, position);

			//vsetky uzly nastavime fixed, aby sme zachovali layout
			//hodnota, ktora je ulozena v DB - premenna isFixed 
			newNode->setFixed(true);
			
			if(nodeColors.contains(nodeID))
			{
				newNode->setColor(nodeColors.value(nodeID));
			}

			if(nodeScales.contains(nodeID))
			{
				newNode->setScale(nodeScales.value(nodeID));
			}

			if(nodeMasks.contains(nodeID))
			{
				newNode->setNodeMask(0);
			}

			nodes.insert(nodeID, newNode);

			if (parentNodes.contains(nodeID))
			{
				newGraph->createNestedGraph(newNode);

				getNestedGraph(nodeID, &newGraph, conn, &error, graphID, layoutID, &maxIdEleUsed, &positions, &nodes, typeNode, typeMetaNode, &parentNodes);

				newGraph->closeNestedGraph();
			}
		}
		
		while(queryEdges->next()) 
		{
			edgeID = queryEdges->value(0).toLongLong();
			edgeName = queryEdges->value(1).toString();
			nodeID1 = queryEdges->value(3).toLongLong();
			nodeID2 = queryEdges->value(4).toLongLong();
			type = queryEdges->value(6).toBool() ? typeMetaEdge : typeEdge;
			isOriented = queryEdges->value(5).toBool();

			if(maxIdEleUsed < edgeID)
				maxIdEleUsed = edgeID + 1;

			iNodes1 = nodes.find(nodeID1);
			iNodes2 = nodes.find(nodeID2);

			newGraph->addEdge(edgeID, edgeName, iNodes1.value(), iNodes2.value(), type, isOriented);

			if(edgeColors.contains(edgeID))
			{
				if(newGraph->getEdges()->contains(edgeID))
					newGraph->getEdges()->find(edgeID).value()->setEdgeColor(edgeColors.value(edgeID));
				else
					newGraph->getMetaEdges()->find(edgeID).value()->setEdgeColor(edgeColors.value(edgeID));
			}

			if(edgeScales.contains(edgeID))
			{
				if(newGraph->getEdges()->contains(edgeID))
					newGraph->getEdges()->find(edgeID).value()->setScale(edgeScales.value(edgeID));
				else
					newGraph->getMetaEdges()->find(edgeID).value()->setScale(edgeScales.value(edgeID));
			}
		}
	}
	else 
	{
		qDebug() << "[Model::GraphDAO::getGraph] Error while loading data from database";
	}

	newGraph->setEleIdCounter(maxIdEleUsed);
	 
	*error2 = error;
	return newGraph;
}

Data::Graph* Model::GraphDAO::addGraph(QString graph_name, QSqlDatabase* conn)
{
    if(conn==NULL || !conn->isOpen()) { //check if we have connection
        qDebug() << "[Model::GraphDAO::addGraph] Connection to DB not opened.";
        return NULL;
    }

	QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("INSERT INTO graphs (graph_name) VALUES (:graph_name) RETURNING graph_id");
    query->bindValue(":graph_name",graph_name);
    if(!query->exec()) {
        qDebug() << "[Model::GraphDAO::addGraph] Could not perform query on DB: " << query->lastError().databaseText();
        return NULL;
    }
	
    if(query->next()) {
        Data::Graph* graph = new Data::Graph(query->value(0).toLongLong(),graph_name,0,0,conn);
        graph->setIsInDB();
        qDebug() << "[Model::GraphDAO::addGraph] Graph was added to DB: " << graph->toString();
        return graph;
    } else {
        qDebug() << "[Model::GraphDAO::addGraph] Graph was not added to DB: " << query->lastQuery();
        return NULL;
    }
}

bool Model::GraphDAO::setGraphName(qlonglong graphID, QString graphName, QSqlDatabase* conn)
{
    if(conn==NULL || !conn->isOpen()) { //check if we have connection
        qDebug() << "[Model::GraphDAO::setGraphName] Connection to DB not opened.";
        return false;
    }

	QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("UPDATE graphs "
		"SET graph_name = :graph_name "
		"WHERE graph_id = :graph_id");
    query->bindValue(":graph_name", graphName);
	query->bindValue(":graph_id", graphID);

    if(!query->exec()) {
        qDebug() << "[Model::GraphDAO::setGraphName] Could not perform query on DB: " << query->lastError().databaseText();
        return false;
    }
	
	return true;
}

bool Model::GraphDAO::addGraph( Data::Graph* graph, QSqlDatabase* conn )
{
    if(conn==NULL || !conn->isOpen()) { //check if we have connection
        qDebug() << "[Model::GraphDAO::addGraph] Connection to DB not opened.";
        return false;
    }
    
    if(graph==NULL) {
        qDebug() << "[Model::GraphDAO::addGraph] Invalid parameter - graph is NULL";
        return false;
    }
    
    if(graph->isInDB()) return true; //graph already in DB
    
    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("INSERT INTO graphs (graph_name) VALUES (:graph_name) RETURNING graph_id");
    query->bindValue(":graph_name",graph->getName());
    if(!query->exec()) {
        qDebug() << "[Model::GraphDAO::addGraph] Could not perform query on DB: " << query->lastError().databaseText();
        return NULL;
    }

    if(query->next()) {
        graph->setId(query->value(0).toLongLong());
        graph->setIsInDB();
        qDebug() << "[Model::GraphDAO::addGraph2] Graph was added to DB and it's ID was set to: " << graph->getId();
        return true;
    } else {
        qDebug() << "[Model::GraphDAO::addGraph2] Graph was not added to DB: " << query->lastQuery();
        return false;
    }
}


bool Model::GraphDAO::removeGraph(Data::Graph* graph, QSqlDatabase* conn)
{
    if(conn==NULL || !conn->isOpen()) { //check if we have connection
        qDebug() << "[Model::GraphDAO::removeGraph] Connection to DB not opened.";
        return false;
    }
    
    if(graph==NULL) {
        qDebug() << "[Model::GraphDAO::removeGraph] Invalid parameter - graph is NULL";
        return false;
    }

    if(!graph->isInDB()) return true;

    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("DELETE FROM graphs WHERE graph_id = :graph_id");
    query->bindValue(":graph_id",graph->getId());
    if(!query->exec()) {
        qDebug() << "[Model::GraphDAO::removeGraph] Could not perform query on DB: " << query->lastError().databaseText();
        return false;
    }

	//TODO: mala by byt moznost nastavit isInDB priznak grafu

    return true;
}

bool Model::GraphDAO::removeGraph(qlonglong graphID, QSqlDatabase* conn)
{
    if(conn==NULL || !conn->isOpen()) { 
        qDebug() << "[Model::GraphDAO::removeGraph] Connection to DB not opened.";
        return false;
    }
    
	Model::EdgeDAO::removeEdges(graphID, conn);
	Model::NodeDAO::removeNodes(graphID, conn);
	Model::GraphLayoutDAO::removeLayouts(graphID, conn);

    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("DELETE FROM graphs WHERE graph_id = :graph_id");
    query->bindValue(":graph_id", graphID);
    if(!query->exec()) {
        qDebug() << "[Model::GraphDAO::removeGraph] Could not perform query on DB: " << query->lastError().databaseText();
        return false;
    }

	qDebug() << "[Model::GraphDAO::removeGraph] Graph no. " << graphID << " was removed from database";

    return true;
}

QString Model::GraphDAO::getName(qlonglong graphID, bool* error, QSqlDatabase* conn)
{
    *error = FALSE;
	QSqlQuery* query;
	QString name;

	//check if we have connection
    if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::GraphDAO::getName] Connection to DB not opened.";
        *error = TRUE;
        return name;
    }

    //get name of graph from DB
    query = new QSqlQuery(*conn);
    query->prepare("SELECT graph_name "
		"FROM graphs "
		"WHERE graph_id = :graph_id");
	query->bindValue(":graph_id", graphID);

    if(!query->exec()) 
	{
        qDebug() << "[Model::GraphDAO::getName] Could not perform query on DB: " << query->lastError().databaseText();
        *error = TRUE;
        return name;
    }
    
	if(query->next()) 
	{
		name = query->value(0).toString();
    }

    return name;
}

QString Model::GraphDAO::setName(QString name,Data::Graph* graph, QSqlDatabase* conn)
{
    //bolo by lepsie prerobit na vracanie error flagu aby sa vedelo ci problem nastal kvoli nedostatocnym parametrom alebo kvoli chybe insertu
    if(conn==NULL || !conn->isOpen()) { //check if we have connection
        qDebug() << "[Model::GraphDAO::setName] Connection to DB not opened.";
        return NULL;
    }
    if(graph==NULL) {
        qDebug() << "[Model::GraphDAO::setName] Invalid parameter - graph is NULL";
        return NULL;
    }
    
    /*if(!graph->isInDB()) {
        if(!Model::GraphDAO::addGraph(graph, conn)) { //could not insert graph into DB
            qDebug() << "[Model::GraphDAO::setName] Could not update graph name in DB. Graph is not in DB.";
            return NULL;
        }
    }*/

    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("UPDATE graphs SET graph_name = :graph_name WHERE graph_id = :graph_id");
    query->bindValue(":graph_id",graph->getId());
    query->bindValue(":graph_name",name);
    if(!query->exec()) {
        qDebug() << "[Model::GraphDAO::setName] Could not perform query on DB: " << query->lastError().databaseText();
        return NULL;
    }

    return name;
}

QMap<QString,QString> Model::GraphDAO::getSettings( Data::Graph* graph, QSqlDatabase* conn, bool* error)
{
    QMap<QString,QString> settings;
    *error = FALSE;
    
    if(conn==NULL || !conn->isOpen()) { //check if we have connection
        qDebug() << "[Model::GraphDAO::getSettings] Connection to DB not opened.";
        *error = TRUE;
        return settings;
    }

    if(graph==NULL) {
        qDebug() << "[Model::GraphDAO::getSettings] Invalid parameter - graph is NULL";
        *error = TRUE;
        return settings;
    }

    if(!graph->isInDB()) {
        qDebug() << "[Model::GraphDAO::getSettings] Graph is not in DB";
        *error = TRUE;
        return settings;
    }
    
    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("SELECT val_name, val FROM graph_settings WHERE graph_id = :graph_id");
    query->bindValue(":graph_id",graph->getId());
    if(!query->exec()) {
        qDebug() << "[Model::GraphDAO::getSettings] Could not perform query on DB: " << query->lastError().databaseText();
        *error = TRUE;
        return settings;
    }

    while(query->next()) {
        settings.insert(query->value(0).toString(),query->value(1).toString());
    }
    
    return settings;
}

