/*!
 * GraphLayout.cpp
 * Projekt 3DVisual
 */

#include "Data/GraphLayout.h"

#include "Data/Graph.h"
#include "Model/GraphLayoutDAO.h"

#include <leathers/push>
#include <leathers/exit-time-destructors>
#include <leathers/global-constructors>

//nastavenie konstantnych typov jednotlivych prvkov grafu
const QString Data::GraphLayout::META_NODE_TYPE = QString( "META_NODE_TYPE" );
const QString Data::GraphLayout::MULTI_NODE_TYPE = QString( "MULTI_NODE_TYPE" );
const QString Data::GraphLayout::SPLITTER_NODE_TYPE = QString( "SPLITTER_NODE_TYPE" );
const QString Data::GraphLayout::META_EDGE_TYPE = QString( "META_EDGE_TYPE" );
const QString Data::GraphLayout::EDGE_PIECE_TYPE = QString( "EDGE_PIECE_TYPE" );
const QString Data::GraphLayout::EDGE_TYPE = QString( "EDGE_TYPE" );
const QString Data::GraphLayout::MULTI_EDGE_TYPE = QString( "MULTI_EDGE_TYPE" );
const QString Data::GraphLayout::HYPER_NODE_TYPE = QString( "HYPER_NODE_TYPE" );
const QString Data::GraphLayout::NESTED_NODE_TYPE = QString( "NESTED_NODE_TYPE" );
const QString Data::GraphLayout::NESTED_EDGE_TYPE = QString( "NESTED_EDGE_TYPE" );
const QString Data::GraphLayout::NESTED_META_EDGE_TYPE= QString( "NESTED_META_EDGE_TYPE" );
const QString Data::GraphLayout::RESTRICTION_NODE_TYPE = QString( "RESTRICTION_NODE_TYPE" );
const QString Data::GraphLayout::MERGED_NODE_TYPE = QString( "MERGED_NODE_TYPE" );
const QString Data::GraphLayout::MERGED_EDGE_TYPE = QString( "MERGED_EDGE_TYPE" );
const QString Data::GraphLayout::HIERARCHY_EDGE_TYPE = QString( "HIERARCHY_EDGE_TYPE" );
const QString Data::GraphLayout::ARC_EDGE_TYPE = QString( "ARC_EDGE_TYPE" );

#include <leathers/pop>

Data::GraphLayout::GraphLayout( qlonglong layout_id, Data::Graph* graph, QString name, QSqlDatabase* conn )
{
	this->layout_id = layout_id;
	this->graph = graph;
	this->name = name;
	this->conn = conn;
	this->inDB = false;
	this->metaSettings = new QMap<QString,QString>();
}

Data::GraphLayout::~GraphLayout( void )
{
	this->metaSettings->clear();
	delete metaSettings;
	this->metaSettings = NULL;
}

QString Data::GraphLayout::setName( QString name )
{
	//nastavenie nazvu layoutu
	QString newName = Model::GraphLayoutDAO::setName( name,this,this->conn );

	if ( newName!=NULL ) {
		this->name = newName;
	}

	return this->name;
}

qlonglong Data::GraphLayout::getGraphId()
{
	//vraciame ID prisluchajuceho grafu
	return this->graph->getId();
}

QString Data::GraphLayout::toString() const
{
	QString str;
	QTextStream( &str ) << "GraphLayout: graph_id:" << graph->getId() << " layout_id:" << layout_id << " name:" << name;
	return str;
}

bool Data::GraphLayout::loadMetaSettings()
{
	//nacitanie z Databazy
	return true;
}

void Data::GraphLayout::setMetaSetting( QString key, QString value )
{
	this->metaSettings->insert( key,value );
}

QString Data::GraphLayout::getMetaSetting( QString key )
{
	return this->metaSettings->value( key,NULL );
}

void Data::GraphLayout::removeMetaSetting( QString key )
{
	this->metaSettings->remove( key );
}
