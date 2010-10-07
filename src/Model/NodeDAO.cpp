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