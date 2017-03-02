#include "LuaGraph/LuaGraph.h"
#include "LuaTypes/LuaValue.h"
#include "LuaTypes/LuaValueList.h"
#include "LuaTypes/LuaValueMap.h"

#include "LuaGraph/LuaGraphObserver.h"
#include "LuaInterface/LuaInterface.h"

#include <sstream>
#include <iostream>

#include <QStringList>
#include <QDebug>

Lua::LuaGraph* Lua::LuaGraph::instance;

Lua::LuaValueList luaCallback( const Lua::LuaValueList& params )
{
	std::cout << "C callback" << std::endl;
	if ( !Lua::LuaGraph::hasObserver() ) {
		return Lua::LuaValueList().getValue();
	}
	Lua::LuaGraph::getInstance()->getObserver()->onUpdate();
	return Lua::LuaValueList().getValue();
}

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#endif

LUA_WRAP_FUNCTION( luaCallback )

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif

Lua::LuaGraph::LuaGraph()
{
	nodes = new QMap<qlonglong, Lua::LuaNode*>();
	edges = new QMap<qlonglong, Lua::LuaEdge*>();
	incidences = new QMap<qlonglong, Lua::LuaIncidence*>();
	observer = NULL;
	Lua::LuaInterface* lua = Lua::LuaInterface::getInstance();
	( *( lua->getLuaState().getValue() ) )["graphChangedCallback"] = LUA_WRAPPER_FUNCTION( luaCallback );
}
Lua::LuaGraphObserver* Lua::LuaGraph::getObserver() const
{
	return observer;
}

void Lua::LuaGraph::setObserver( Lua::LuaGraphObserver* value )
{
	observer = value;
}

void Lua::LuaGraph::clearGraph()
{
	nodes->clear();
	edges->clear();
	incidences->clear();
}

void Lua::LuaGraph::printGraph()
{
	std::cout << "Incidences " << incidences->count() << std::endl;
	for ( QMap<qlonglong, LuaIncidence*>::iterator i = incidences->begin(); i != incidences->end(); ++i ) {
		std::cout << i.key() << " ";
	}
	std::cout << std::endl;
	std::cout << "Nodes " << nodes->count() << std::endl;
	for ( QMap<qlonglong, LuaNode*>::iterator i = nodes->begin(); i != nodes->end(); ++i ) {
		std::cout << i.key() << " ";
	}
	std::cout << std::endl;
	std::cout << "Edges " << edges->count() << std::endl;
	for ( QMap<qlonglong, LuaEdge*>::iterator i = edges->begin(); i != edges->end(); ++i ) {
		std::cout << i.key() << " ";
	}
	std::cout << std::endl;
}

Lua::LuaGraph* Lua::LuaGraph::getInstance()
{
	if ( instance == NULL ) {
		instance = new Lua::LuaGraph();
	}
	return instance;
}

Lua::LuaGraph* Lua::LuaGraph::loadGraph()
{
	std::cout << "loading graph" << std::endl;
	Lua::LuaInterface* lua = Lua::LuaInterface::getInstance();

	Lua::LuaGraph* result = Lua::LuaGraph::getInstance();
	result->clearGraph();

	Lua::LuaValueMap edges = ( *( lua->getLuaState().getValue() ) )["getGraph"]()[0].asTable();

	for ( auto iterator = edges.begin(); iterator != edges.end(); ++iterator ) {
		qlonglong id = iterator->first.asTable()["id"].asInteger();

		Lua::LuaEdge* edge = new Lua::LuaEdge();
		edge->setId( id );
		edge->setParams( iterator->first.asTable()["params"] );
		if ( iterator->first.asTable()["label"].type() != 0 ) {
			edge->setLabel( QString::fromStdString( iterator->first.asTable()["label"].asString() ) );
		}
		else {
			std::stringstream sstm1;
			sstm1 << "Edge " << id;
			edge->setLabel( QString::fromStdString( sstm1.str() ) );
		}
		result->edges->insert( id, edge );


		Lua::LuaValueMap incidences = iterator->second.asTable();
		for ( auto iterator2 = incidences.begin(); iterator2 != incidences.end(); ++iterator2 ) {
			qlonglong id2 = iterator2->first.asTable()["id"].asInteger();
			edge->addIncidence( id2 );
			Lua::LuaIncidence* incidence = new Lua::LuaIncidence();
			incidence->setId( id2 );
			incidence->setParams( iterator2->first.asTable()["params"] );
			if ( iterator2->first.asTable()["label"].type() != 0 ) {
				incidence->setLabel( QString::fromStdString( iterator2->first.asTable()["label"].asString() ) );
			}
			else {
				std::stringstream sstm2;
				sstm2 << "Incid " << id2;
				incidence->setLabel( QString::fromStdString( sstm2.str() ) );
			}
			incidence->setOriented( iterator2->first.asTable()["direction"].type() != 0 );
			if ( incidence->getOriented() ) {
				incidence->setOutGoing( iterator2->first.asTable()["direction"].asString() == "out" );
			}

			qlonglong id3 = iterator2->second.asTable()["id"].asInteger();
			if ( result->nodes->contains( id3 ) ) {
				result->nodes->value( id3 )->addIncidence( id2 );
			}
			else {
				Lua::LuaNode* node = new Lua::LuaNode();
				node->setId( id3 );
				node->setParams( iterator2->second.asTable()["params"] );
				if ( iterator2->second.asTable()["label"].type() != 0 ) {
					node->setLabel( QString::fromStdString( iterator2->second.asTable()["label"].asString() ) );
				}
				else {
					std::stringstream sstm;
					sstm << "Node " << id3;
					node->setLabel( QString::fromStdString( sstm.str() ) );
				}
				node->addIncidence( id2 );
				result->nodes->insert( id3, node );
			}

			incidence->setEdgeNode( id, id3 );
			result->incidences->insert( id2, incidence );
		}
	}

	std::cout << "Node count: " << result->nodes->count() << std::endl;
	return result;
}

Lua::LuaGraph* Lua::LuaGraph::loadEvoGraph( QString repoFilepath )
{
//    qDebug() << "loading evo graph";
	Lua::LuaInterface* lua = Lua::LuaInterface::getInstance();

	Lua::LuaGraph* result = Lua::LuaGraph::getInstance();
	result->clearGraph();

	Lua::LuaValueMap edges = ( *( lua->getLuaState().getValue() ) )["getGraph"]()[0].asTable();

	QList<qlonglong> unusedNodes = QList<qlonglong>();

	for ( auto iterator = edges.begin(); iterator != edges.end(); ++iterator ) {
		// cast nastavenia Edge
		qlonglong edgeId = iterator->first.asTable()["id"].asInteger();
		Lua::LuaEdge* edge = new Lua::LuaEdge();
		edge->setId( edgeId );
		edge->setParams( iterator->first.asTable()["params"] );

		if ( iterator->first.asTable()["label"].type() != 0 ) {
			edge->setLabel( QString::fromStdString( iterator->first.asTable()["label"].asString() ) );
		}
		else {
//            qDebug() << "Edge" << edgeId << "neobsahuje LABEL";
		}

		result->edges->insert( edgeId, edge );

		Lua::LuaValueMap incidences = iterator->second.asTable();
		for ( auto iterator2 = incidences.begin(); iterator2 != incidences.end(); ++iterator2 ) {

			// cast nastavenia Incidence
			qlonglong incidenceId = iterator2->first.asTable()["id"].asInteger();
			Lua::LuaIncidence* incidence = new Lua::LuaIncidence();
			incidence->setId( incidenceId );
			incidence->setParams( iterator2->first.asTable()["params"] );
			if ( iterator2->first.asTable()["label"].type() != 0 ) {
				incidence->setLabel( QString::fromStdString( iterator2->first.asTable()["label"].asString() ) );
			}
			else {
//                qDebug() << "Incidence" << incidenceId << "neobsahuje LABEL";
			}

			incidence->setOriented( iterator2->first.asTable()["direction"].type() != 0 );
			if ( incidence->getOriented() ) {
				incidence->setOutGoing( iterator2->first.asTable()["direction"].asString() == "out" );
			}

			edge->addIncidence( incidenceId );

			// cast nastavenia Node
			qlonglong nodeId = iterator2->second.asTable()["id"].asInteger();

			QString type = QString::fromStdString( iterator2->second.asTable()["params"].asTable()["type"].asString() );

			QString identifier = "";

			if ( !QString::compare( type, "directory" ) || !QString::compare( type, "file" ) ) {
				identifier = type + ";" + QString::fromStdString( iterator2->second.asTable()["params"].asTable()["path"].asString() ).replace( repoFilepath + "/", "" );
//                qDebug() << "directory/file =" << identifier;
			}

			if ( !QString::compare( type, "globalModule" ) ) {
				if ( iterator2->second.asTable()["label"].type() != 0 ) {
					identifier = type + ";" + QString::fromStdString( iterator2->second.asTable()["label"].asString() );
//                    qDebug() << "globalModule =" << identifier;
				}
				else {
					qDebug() << "Uzol" << nodeId << "neobsahuje LABEL";
					delete incidence;
					return NULL;
				}
			}

			if ( !QString::compare( type, "function" ) ) {
				QString modulePath = QString::fromStdString( iterator2->second.asTable()["params"].asTable()["modulePath"].asString() ).replace( repoFilepath + "/", "" );
				if ( iterator2->second.asTable()["label"].type() != 0 ) {
					identifier = type + ";" + modulePath + ";" + QString::fromStdString( iterator2->second.asTable()["label"].asString() );
//                    qDebug() << "function =" << identifier;
				}
				else {
					qDebug() << "Uzol" << nodeId << "neobsahuje LABEL";
					delete incidence;
					return NULL;
				}
			}
			Lua::LuaNode* node = new Lua::LuaNode();
			node->setId( nodeId );
			node->setParams( iterator2->second.asTable()["params"] );
			node->setLabel( QString::fromStdString( iterator2->second.asTable()["label"].asString() ) );


			if ( result->nodes->contains( nodeId ) ) {
				result->nodes->value( nodeId )->addIncidence( incidenceId );
			}
			else {
				node->setIdentifier( identifier );

//                qDebug() << identifier;
				result->nodes->insert( nodeId, node );
				node->addIncidence( incidenceId );

				if ( identifier == "" ) {
					unusedNodes.append( nodeId );
				}
			}

			incidence->setEdgeNode( edgeId, nodeId );
			result->incidences->insert( incidenceId, incidence );
		}
	}

	foreach ( qlonglong nodeId, unusedNodes ) {
		Lua::LuaNode* node = result->nodes->find( nodeId ).value();
		QString nodeType = QString::fromStdString( node->getParams().getValue()["type"].asString() );
		bool isPartOfModule = false;
		foreach ( qlonglong incidenceId, node->getIncidences() ) {
			Lua::LuaEdge* edge = result->edges->find( result->incidences->value( incidenceId )->getEdgeNodePair().first ).value();

			Lua::LuaIncidence* incidence = nullptr;
			if ( edge->getIncidences().at( 0 ) != incidenceId ) {
				incidence = result->incidences->find( edge->getIncidences().at( 0 ) ).value();
			}
			else {
				incidence = result->incidences->find( edge->getIncidences().at( 1 ) ).value();
			}

			Lua::LuaNode* otherNode = result->nodes->find( incidence->getEdgeNodePair().second ).value();
			QString type = QString::fromStdString( otherNode->getParams().getValue()["type"].asString() );
			if ( !QString::compare( type, "globalModule" ) ) {
				QString identifier = nodeType + ";" + otherNode->getLabel() + ";" + node->getLabel();
				node->setIdentifier( identifier );
//                qDebug() << node->getIdentifier() << "found";
				isPartOfModule = true;
				break;
			}
		}

		if ( !isPartOfModule ) {
			QString identifier = nodeType + ";" + node->getLabel();
			node->setIdentifier( identifier );
//            qDebug() << node->getIdentifier() << "not found";
		}
	}

	for ( QMap<qlonglong, Lua::LuaEdge*>::iterator iterator =  result->getEdges()->begin(); iterator != result->getEdges()->end(); ++iterator ) {
		Lua::LuaEdge* edge = iterator.value();
		Lua::LuaNode* node1 =  result->getNodes()->value( result->getIncidences()->value( edge->getIncidences().at( 0 ) )->getEdgeNodePair().second );
		Lua::LuaNode* node2 =  result->getNodes()->value( result->getIncidences()->value( edge->getIncidences().at( 1 ) )->getEdgeNodePair().second );
		QString identifier = node1->getIdentifier() + "+" + node2->getIdentifier();
		edge->setIdentifier( identifier );
//        qDebug() << identifier;
	}

//    qDebug() << "EvoNode count: " << result->nodes->count();
	return result;
}

Lua::LuaGraph::~LuaGraph()
{
	for ( QMap<qlonglong, Lua::LuaNode*>::iterator i = nodes->begin(); i != nodes->end(); ++i ) {
		delete( *i );
	}

	for ( QMap<qlonglong, Lua::LuaEdge*>::iterator i = edges->begin(); i != edges->end(); ++i ) {
		delete( *i );
	}

	for ( QMap<qlonglong, Lua::LuaIncidence*>::iterator i = incidences->begin(); i != incidences->end(); ++i ) {
		delete( *i );
	}
	delete nodes;
	delete incidences;
	delete edges;
}
QMap<qlonglong, Lua::LuaIncidence*>* Lua::LuaGraph::getIncidences() const
{
	return incidences;
}

bool Lua::LuaGraph::hasObserver()
{
	if ( instance == NULL ) {
		return false;
	}
	return instance->observer != NULL;
}

QMap<qlonglong, Lua::LuaEdge*>* Lua::LuaGraph::getEdges() const
{
	return edges;
}

QMap<qlonglong, Lua::LuaNode*>* Lua::LuaGraph::getNodes() const
{
	return nodes;
}

Lua::LuaNode* Lua::LuaGraph::findNodeByLuaIdentifier( QString identifier )
{
	for ( QMap<qlonglong, Lua::LuaNode*>::iterator iterator = this->getNodes()->begin(); iterator != this->getNodes()->end(); ++iterator ) {
		if ( !QString::compare( identifier, iterator.value()->getIdentifier() ) ) {
			return iterator.value();
		}
	}

	return nullptr;
}

Lua::LuaEdge* Lua::LuaGraph::findEdgeByLuaIdentifier( QString identifier )
{

	QStringList nodes = identifier.split( "+" );
	QString newIdentifier = nodes.at( 1 ) + "+" + nodes.at( 0 );

	for ( QMap<qlonglong, Lua::LuaEdge*>::iterator iterator = this->getEdges()->begin(); iterator != this->getEdges()->end(); ++iterator ) {
		if ( !QString::compare( identifier, iterator.value()->getIdentifier() ) ) {
			return iterator.value();
		}

		if ( !QString::compare( newIdentifier, iterator.value()->getIdentifier() ) ) {
			return iterator.value();
		}
	}

	return nullptr;
}

