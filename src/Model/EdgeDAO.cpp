/*!
 * EdgeDAO.cpp
 * Projekt 3DVisual
 */
#include "Model/EdgeDAO.h"

Model::EdgeDAO::EdgeDAO(void)
{
}

Model::EdgeDAO::~EdgeDAO(void)
{
}

bool Model::EdgeDAO::addEdgesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges)
{
	//check if we have connection
	if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::EdgeDAO::addEdgesToDB] Connection to DB not opened.";
        return false;
    }

	QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator iEdges =	edges->constBegin();

	QSqlQuery* query = new QSqlQuery(*conn);
 
	while(iEdges != edges->constEnd()) 
	{
		query->prepare("INSERT INTO edges (edge_id, name, type_id, n1, n2, oriented, meta, graph_id) VALUES (:edge_id, :name, :type_id, :n1, :n2, :oriented, :meta, :graph_id) RETURNING  edge_id");
		query->bindValue(":edge_id", iEdges.value()->getId());
		query->bindValue(":name", iEdges.value()->getName());
		query->bindValue(":type_id", iEdges.value()->getType()->getId());
		query->bindValue(":n1", iEdges.value()->getSrcNode()->getId()); 
		query->bindValue(":n2", iEdges.value()->getDstNode()->getId());
		query->bindValue(":oriented", iEdges.value()->isOriented());
		query->bindValue(":meta", false);
		query->bindValue(":graph_id", iEdges.value()->getGraph()->getId());

		if(!query->exec()) {
			qDebug() << "[Model::EdgeDAO::addEdgesToDB] Could not perform query on DB: " << query->lastError().databaseText();
			return false;
		}
		++iEdges;
	}

	qDebug() << "[Model::EdgeDAO::addEdgesToDB] " << edges->count() << " edges were saved to DB.";

	return true;
}

bool Model::EdgeDAO::addMetaEdgesToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges, Data::GraphLayout* layout)
{
	//check if we have connection
	if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::EdgeDAO::addMetaEdgesToDB] Connection to DB not opened.";
        return false;
    }

	QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator iEdges =	edges->constBegin();

	QSqlQuery* query = new QSqlQuery(*conn);
 
	while(iEdges != edges->constEnd()) 
	{
		query->prepare("INSERT INTO edges (edge_id, name, type_id, n1, n2, oriented, meta, graph_id, layout_id) VALUES (:edge_id, :name, :type_id, :n1, :n2, :oriented, :meta, :graph_id, :layout_id) RETURNING  edge_id");
		query->bindValue(":edge_id", iEdges.value()->getId());
		query->bindValue(":name", iEdges.value()->getName());
		query->bindValue(":type_id", iEdges.value()->getType()->getId());
		query->bindValue(":n1", iEdges.value()->getSrcNode()->getId()); 
		query->bindValue(":n2", iEdges.value()->getDstNode()->getId());
		query->bindValue(":oriented", iEdges.value()->isOriented());
		query->bindValue(":meta", true);
		query->bindValue(":graph_id", iEdges.value()->getGraph()->getId());
		query->bindValue(":layout_id", layout->getId());

		if(!query->exec()) {
			qDebug() << "[Model::EdgeDAO::addMetaEdgesToDB] Could not perform query on DB: " << query->lastError().databaseText();
			return false;
		}
		++iEdges;
	}

	qDebug() << "[Model::EdgeDAO::addMetaEdgesToDB] " << edges->count() << " meta edges were saved to DB.";

	return true;
}

bool Model::EdgeDAO::addEdgesColorToDB(QSqlDatabase* conn, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges, Data::GraphLayout* layout)
{
	QMap< qlonglong,osg::ref_ptr<Data::Edge> >::const_iterator iEdges = edges->constBegin();
	
	while(iEdges != edges->constEnd()) 
	{
		//ulozime farbu len hranam, ktore maju farbu inu nez default
		if(iEdges.value()->getEdgeColor().r() != 1 || iEdges.value()->getEdgeColor().g() != 1 ||iEdges.value()->getEdgeColor().b() != 1 ||iEdges.value()->getEdgeColor().a() != 1)
		{
			addColorToDB(conn, iEdges.value()->getGraph()->getId(), layout->getId(), iEdges.value()->getId(), "color_r", iEdges.value()->getEdgeColor().r());
			addColorToDB(conn, iEdges.value()->getGraph()->getId(), layout->getId(), iEdges.value()->getId(), "color_g", iEdges.value()->getEdgeColor().g());
			addColorToDB(conn, iEdges.value()->getGraph()->getId(), layout->getId(), iEdges.value()->getId(), "color_b", iEdges.value()->getEdgeColor().b());
			addColorToDB(conn, iEdges.value()->getGraph()->getId(), layout->getId(), iEdges.value()->getId(), "color_a", iEdges.value()->getEdgeColor().a());
		}

		++iEdges;
	}
	
	return true;
}

bool Model::EdgeDAO::addColorToDB(QSqlDatabase* conn, qlonglong graphID, qlonglong layoutID, qlonglong edgeID, QString valName, double val)
{
	if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::EdgeDAO::addColorToDB] Connection to DB not opened.";
        return false;
    }

	QSqlQuery* query = new QSqlQuery(*conn);
	
	query->prepare("INSERT INTO edge_settings (graph_id, edge_id, val_name, val, layout_id) VALUES (:graph_id, :edge_id, :val_name, :val, :layout_id)");
	query->bindValue(":graph_id", graphID); 
	query->bindValue(":edge_id", edgeID);
	query->bindValue(":val_name", valName);
	query->bindValue(":val", val);
	query->bindValue(":layout_id", layoutID);

	if(!query->exec()) {
		qDebug() << "[Model::EdgeDAO::addColorToDB] Could not perform query on DB: " << query->lastError().databaseText();
		return false;
	}

	return true;
}

QSqlQuery* Model::EdgeDAO::getEdgesQuery(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID)
{
	QSqlQuery* query;
    *error = FALSE;

	//check if we have connection
    if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::EdgeDAO::getEdgesQuery] Connection to DB not opened.";
        *error = TRUE;
        return query;
    }

    //get nodes from DB
    query = new QSqlQuery(*conn);
    query->prepare("SELECT * "
		"FROM edges "
		"WHERE graph_id = :graph_id "
		"AND (layout_id IS NULL OR layout_id = :layout_id)");
	query->bindValue(":graph_id", graphID);
	query->bindValue(":layout_id", layoutID);

    if(!query->exec()) {
        qDebug() << "[Model::EdgeDAO::getEdgesQuery] Could not perform query on DB: " << query->lastError().databaseText();
        *error = TRUE;
        return query;
    }
    
    return query;
}

QList<qlonglong> Model::EdgeDAO::getListOfEdges(QSqlDatabase* conn, bool* error)
{
	QList<qlonglong> edges;
    *error = FALSE;

	//check if we have connection
    if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::EdgeDAO::getListOfEdges] Connection to DB not opened.";
        *error = TRUE;
        return edges;
    }

    //get edges from DB
    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("SELECT graph_id " 
		"FROM edges ");

    if(!query->exec()) 
	{
        qDebug() << "[Model::EdgeDAO::getListOfEdges] Could not perform query on DB: " << query->lastError().databaseText();
        *error = TRUE;
        return edges;
    }
    
    while(query->next()) {
		edges << query->value(0).toLongLong();
    }

    return edges;
}

bool Model::EdgeDAO::checkIfExists(Data::Edge* edge, QSqlDatabase* conn)
{
    if(conn==NULL || !conn->isOpen()) { //check if we have connection
        qDebug() << "[Model::EdgeDAO::checkIfExists] Connection to DB not opened.";
        return NULL;
    } else if(edge==NULL) {
        qDebug() << "[Model::EdgeDAO::checkIfExists] Invalid parameter - edge is NULL.";
        return NULL;
    } else if(edge->getGraph()==NULL) {
        qDebug() << "[Model::EdgeDAO::checkIfExists] Edge does not have graph assigned.";
        return false;
    } else if(!edge->isInDB() || (edge->getGraph()!=NULL && !edge->getGraph()->isInDB())) {
        return false;
    }

    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("SELECT COUNT(1) FROM edges "
        "WHERE edge_id=:edge_id AND graph_id=:graph_id");
    query->bindValue(":edge_id", edge->getId());
    query->bindValue(":graph_id", edge->getGraph()->getId());
    if(!query->exec()) {
        qDebug() << "[Model::EdgeDAO::checkIfExists] Could not perform query on DB: " << query->lastError().databaseText();
        return NULL;
    }
    if(query->next() && query->value(0)==1) {
        return true;
    } else return false;
}

bool Model::EdgeDAO::removeEdge( Data::Edge* edge, QSqlDatabase* conn )
{
    if(conn==NULL || !conn->isOpen()) { //check if we have connection
        qDebug() << "[Model::EdgeDAO::removeEdge] Connection to DB not opened.";
        return NULL;
    } else if(edge==NULL) {
        qDebug() << "[Model::EdgeDAO::removeEdge] Invalid parameter - edge is NULL.";
        return false;
    } else if(edge->getGraph()==NULL) {
        qDebug() << "[Model::EdgeDAO::removeEdge] Edge does not have graph assigned.";
        return false;
    } else if(!edge->isInDB()) {
        return false;
    }

    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("DELETE FROM edges WHERE graph_id = :graph_id AND edge_id = :edge_id");
    query->bindValue(":graph_id", edge->getGraph()->getId());
    query->bindValue(":edge_id", edge->getId());
    if(!query->exec()) {
        qDebug() << "[Model::EdgeDAO::removeEdge] Could not perform query on DB: " << query->lastError().databaseText();
        return false;
    }

    return true;
}

QMap<QString,QString> Model::EdgeDAO::getSettings( Data::Edge* edge, QSqlDatabase* conn, bool* error )
{
    QMap<QString,QString> settings;
    *error = FALSE;

    if(conn==NULL || !conn->isOpen()) { //check if we have connection
        qDebug() << "[Model::EdgeDAO::getSettings] Connection to DB not opened.";
        *error = TRUE;
        return settings;
    }

    if(edge==NULL) {
        qDebug() << "[Model::EdgeDAO::getSettings] Invalid parameter - Edge is NULL";
        *error = TRUE;
        return settings;
    }

    if(!edge->isInDB()) {
        qDebug() << "[Model::EdgeDAO::getSettings] Edge is not in DB";
        *error = TRUE;
        return settings;
    }

    QSqlQuery* query = new QSqlQuery(*conn);
    query->prepare("SELECT val_name, val FROM edge_settings WHERE graph_id = :graph_id AND edge_id = :edge_id");
    query->bindValue(":graph_id",edge->getGraph()->getId());
    query->bindValue(":edge_id",edge->getId());
    if(!query->exec()) {
        qDebug() << "[Model::EdgeDAO::getSettings] Could not perform query on DB: " << query->lastError().databaseText();
        *error = TRUE;
        return settings;
    }

    while(query->next()) {
        settings.insert(query->value(0).toString(),query->value(1).toString());
    }

    return settings;
}

QMap<qlonglong, QString> Model::EdgeDAO::getSettings(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID, QString attributeName)
{
    *error = FALSE;
	QSqlQuery* query;
	QMap<qlonglong, QString> settings;

	//check if we have connection
    if(conn==NULL || !conn->isOpen()) 
	{ 
        qDebug() << "[Model::EdgeDAO::getSettings] Connection to DB not opened.";
        *error = TRUE;
        return settings;
    }

    query = new QSqlQuery(*conn);
    query->prepare("SELECT edge_id, val "
		"FROM edge_settings "
		"WHERE graph_id = :graph_id "
		"AND val_name = :attribute_name "
		"AND layout_id = :layout_id"); 
	query->bindValue(":graph_id", graphID);
	query->bindValue(":attribute_name", attributeName);
	query->bindValue(":layout_id", layoutID);

    if(!query->exec()) {
        qDebug() << "[Model::EdgeDAO::getSettings] Could not perform query on DB: " << query->lastError().databaseText();
        *error = TRUE;
        return settings;
    }

	while(query->next()) {
		settings.insert(query->value(0).toLongLong(), query->value(1).toString());
    }
    
    return settings;
}

QMap<qlonglong, osg::Vec4f> Model::EdgeDAO::getColors(QSqlDatabase* conn, bool* error, qlonglong graphID, qlonglong layoutID)
{
    *error = FALSE;
	bool error2 = false;
	osg::Vec4f color;
	qlonglong id;
	QMap<qlonglong, osg::Vec4f> colors;

	QMap<qlonglong, QString> edgeColorR;
	QMap<qlonglong, QString> edgeColorG;
	QMap<qlonglong, QString> edgeColorB;
	QMap<qlonglong, QString> edgeColorA;
	QMap<qlonglong, QString>::iterator iter_r;
	QMap<qlonglong, QString>::iterator iter_g;
	QMap<qlonglong, QString>::iterator iter_b;
	QMap<qlonglong, QString>::iterator iter_a;

	edgeColorR = getSettings(conn, &error2, graphID, layoutID, "color_r");
	edgeColorG = getSettings(conn, &error2, graphID, layoutID, "color_g");
	edgeColorB = getSettings(conn, &error2, graphID, layoutID, "color_b");
	edgeColorA = getSettings(conn, &error2, graphID, layoutID, "color_a");

	for(iter_r = edgeColorR.begin(); iter_r != edgeColorR.end(); iter_r++) 
	{
		id = iter_r.key();
		iter_g = edgeColorG.find(id);
		iter_b = edgeColorB.find(id);
		iter_a = edgeColorA.find(id);

		color = osg::Vec4f(iter_r.value().toFloat(), iter_g.value().toFloat(), iter_b.value().toFloat(), iter_a.value().toFloat());
		colors.insert(id, color);
	}

	return colors;
}