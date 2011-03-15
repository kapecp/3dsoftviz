/*!
 * GraphLayout.cpp
 * Projekt 3DVisual
 */

#include "Data/GraphLayout.h"

const QString Data::GraphLayout::META_NODE_TYPE = QString("META_NODE_TYPE");
const QString Data::GraphLayout::MULTI_NODE_TYPE = QString("MULTI_NODE_TYPE");
const QString Data::GraphLayout::META_EDGE_TYPE = QString("META_EDGE_TYPE");
const QString Data::GraphLayout::MULTI_EDGE_TYPE = QString("MULTI_EDGE_TYPE");
const QString Data::GraphLayout::NESTED_NODE_TYPE = QString("NESTED_NODE_TYPE");
const QString Data::GraphLayout::NESTED_EDGE_TYPE = QString("NESTED_EDGE_TYPE");
const QString Data::GraphLayout::NESTED_META_EDGE_TYPE= QString("NESTED_META_EDGE_TYPE");

Data::GraphLayout::GraphLayout(qlonglong layout_id, Data::Graph* graph, QString name, QSqlDatabase* conn)
{
    this->layout_id = layout_id;
    this->graph = graph;
    this->name = name;
    this->conn = conn;
    this->inDB = false;
    this->metaSettings = new QMap<QString,QString>();
}

Data::GraphLayout::~GraphLayout(void)
{
    this->metaSettings->clear();
    delete metaSettings;
    this->metaSettings = NULL;
}

QString Data::GraphLayout::setName(QString name)
{
    QString newName = Model::GraphLayoutDAO::setName(name,this,this->conn);

    if(newName!=NULL) {
        this->name = newName;
    }

    return this->name;
}

qlonglong Data::GraphLayout::getGraphId()
{
    return this->graph->getId();
}

QString Data::GraphLayout::toString() const
{
    QString str;
    QTextStream(&str) << "GraphLayout: graph_id:" << graph->getId() << " layout_id:" << layout_id << " name:" << name;
    return str;
}

bool Data::GraphLayout::loadMetaSettings()
{
    //loadnutie z DB
    return true;
}

void Data::GraphLayout::setMetaSetting( QString key, QString value )
{
    this->metaSettings->insert(key,value);
}

QString Data::GraphLayout::getMetaSetting( QString key )
{
    return this->metaSettings->value(key,NULL);
}

void Data::GraphLayout::removeMetaSetting( QString key )
{
	this->metaSettings->remove(key);
}
