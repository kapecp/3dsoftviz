#include "LuaGraph/GitGraphVisualizer.h"
#include "LuaGraph/HyperGraphVisualizer.h"
#include "LuaGraph/LuaGraphVisualizer.h"
#include "LuaGraph/LuaGraph.h"
#include "Importer/GraphOperations.h"

#include "Data/Graph.h"

#include "Manager/Manager.h"
#include "GitLib/GitEvolutionGraph.h"

#include <QMap>

#include "limits"
#include <iostream>

#include <QDebug>

Lua::GitGraphVisualizer::GitGraphVisualizer( Data::Graph* graph, osg::ref_ptr<osg::Camera> camera )
	: Lua::LuaGraphVisualizer( graph, camera ) {}

void Lua::GitGraphVisualizer::visualize()
{
    qDebug() << "GitGraphVisualizer visualization";
	Lua::LuaGraph* g = Lua::LuaGraph::loadGraph();
//	g->printGraph();

    QString filepath =  Manager::GraphManager::getInstance()->getActiveEvolutionGraph()->getFilePath();
    int countNode = 0;
    int luaCount = 0;
    int globalCount = 0;

	for ( QMap<qlonglong, Lua::LuaNode*>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i ) {
        luaCount++;
        QString type = QString::fromStdString( i.value()->getParams()["type"].asString() );

        if( QString::compare( type, "globalFunction" ) && QString::compare( type, "globalModule" ) ) {
            QString nodeName = "";

            if( !QString::compare( type, "directory" ) || !QString::compare( type, "file" ) ) {
                nodeName = QString::fromStdString( i.value()->getParams()["path"].asString() ).replace( filepath + "/", "" );
            } else {
                nodeName = i.value()->getLabel();
            }

            osg::ref_ptr<Data::Node> n = this->currentGraph->findNodeByName( nodeName );


            if( !n ) {
                qDebug() << nodeName << "not found";
                countNode++;
//                if( !QString::compare( type, "function") ) {
//                    qDebug() << "Pridavam funkciu" << nodeName;
//                    n = this->currentGraph->addNode( i.key() , nodeName, nodeType );
//                }

            } else {
                qDebug() << nodeName << "found";
                n = currentGraph->replaceNodeId( n->getId(), i.key() );

                if( n ) {
                    n->setId( i.key() );
                    setNodeParams( n, i.value(), osg::Vec4f( 1,1,1,1 ), 8 );
                    n->Data::AbsNode::setName( nodeName );
                    n->setLabelText( nodeName );
                    n->showLabel( true );
                    n->reloadConfig();
                } else {
                    return;
                }
            }
            /*
            setNodeParams( n, i.value(), osg::Vec4f( 1,1,1,1 ), 8 );
            n->Data::AbsNode::setName( nodeName );
            n->setLabelText( nodeName );
            n->reloadConfig();*/
        } else {
            globalCount++;
        }

	}

    qDebug() << "Pocet pridanych uzlov do grafu" << countNode << " a pocet z Lua " << luaCount << " a global " << globalCount;

	for ( QMap<qlonglong, Lua::LuaEdge*>::iterator i = g->getEdges()->begin(); i != g->getEdges()->end(); ++i ) {
		if ( i.value()->getIncidences().size() != 2 ) {
			throw new std::runtime_error( "Not a simple graph" );
		}
		LuaIncidence* const incid1 = g->getIncidences()->value( i.value()->getIncidences()[0] );
		LuaIncidence* const incid2 = g->getIncidences()->value( i.value()->getIncidences()[1] );
		osg::ref_ptr<Data::Node> srcNode = currentGraph->getNodes()->value( incid1->getEdgeNodePair().second );
		osg::ref_ptr<Data::Node> dstNode = currentGraph->getNodes()->value( incid2->getEdgeNodePair().second );
		osg::ref_ptr<Data::Edge> newEdge;

        if( srcNode.get() && dstNode.get() ) {

        } else {
//            qDebug() << "Prazdny uzol";
            continue;
        }

		if ( incid1->getOriented() ) {
			if ( incid1->getOutGoing() ) {
				newEdge = currentGraph->addEdge( i.key(), i.value()->getLabel(), dstNode, srcNode, edgeType, true );
			}
			else {
				newEdge = currentGraph->addEdge( i.key(), i.value()->getLabel(), srcNode, dstNode, edgeType, true );
			}
		}
		else {
			newEdge = currentGraph->addEdge( i.key(), i.value()->getLabel(), srcNode, dstNode, edgeType, false );
		}
		newEdge->setCamera( camera );
		setEdgeParams( newEdge, i.value(), osg::Vec4f( 1,1,1,1 ) );
    }
}

void Lua::GitGraphVisualizer::onUpdate()
{
	std::cout << "SimpleGraph update called" << std::endl;
	Lua::LuaGraph* g = Lua::LuaGraph::loadGraph();
	for ( QMap<qlonglong, Lua::LuaNode*>::iterator i = g->getNodes()->begin(); i != g->getNodes()->end(); ++i ) {
		osg::ref_ptr<Data::Node> n = currentGraph->getNodes()->value( i.key() );
		setNodeParams( n, i.value(), osg::Vec4f( 1,1,1,1 ), 8 );
	}
	for ( QMap<qlonglong, Lua::LuaEdge*>::iterator i = g->getEdges()->begin(); i != g->getEdges()->end(); ++i ) {
		osg::ref_ptr<Data::Edge> e = currentGraph->getEdges()->value( i.key() );
		setEdgeParams( e, i.value(), osg::Vec4f( 1,1,1,1 ) );
		e->reloadLabel();
	}
}



