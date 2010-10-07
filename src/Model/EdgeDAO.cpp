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