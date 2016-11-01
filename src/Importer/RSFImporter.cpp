#include "Importer/RSFImporter.h"
#include "Importer/GraphOperations.h"
#include "Importer/ReadNodesStore.h"

#include "Data/Graph.h"
#include "Data/Edge.h"

#include <QTextStream>

#include <string>
#include <memory>

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif

namespace Importer {

osg::ref_ptr<Data::Node> RSFImporter::getHyperEdge(
	QString srcNodeName,
	QString edgeName,QMap<qlonglong,
	osg::ref_ptr<Data::Edge> >* mapa )
{
	osg::ref_ptr<Data::Node> hyperEdgeNode1;
	//zaciatocny bod hyperhrany
	for ( QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator it = mapa->begin(); it != mapa->end(); ++it ) {
		osg::ref_ptr<Data::Edge> existingEdge = it.value();
		if (
			( ( Data::AbsNode* )existingEdge->getSrcNode() )->getName() == srcNodeName &&
			( ( Data::AbsNode* )existingEdge->getDstNode() )->getName() == edgeName
		) {
			hyperEdgeNode1 = existingEdge->getDstNode();
			break;
		}
	}
	return hyperEdgeNode1;
}

bool RSFImporter::import(
	ImporterContext& context
)
{
	QTextStream stream( &( context.getStream() ) );
	QString line;

	GraphOperations graphOp( context.getGraph() );
	ReadNodesStore readNodes;
	Data::Type* edgeType = NULL;
	Data::Type* nodeType = NULL;

	( void )graphOp.addDefaultTypes( edgeType, nodeType );


	//citanie vstupneho suboru
	while ( !stream.atEnd() ) {
		line = stream.readLine();
		QStringList words;
		words = line.split( QRegExp( QString( "[ \t]+" ) ) );
		int size = words.size();
		osg::ref_ptr<Data::Node> node1, node2;
		osg::ref_ptr<Data::Edge> edge;
		if ( size!=3 ) {
			printf( "%d",size );
			context.getInfoHandler().reportError( "Zvoleny subor nie je validny RSF subor." );
			return false;
		}
		else {
			if ( words[0]=="tagged" ) {

			}
			else {
				QString edgeName = words[0];
				QString srcNodeName = words[1];
				QString dstNodeName = words[2];

				// vytvorenie pociatocneho uzla
				if ( !readNodes.contains( srcNodeName ) ) {
					node1 = context.getGraph().addNode( srcNodeName, nodeType );
					readNodes.addNode( srcNodeName, node1 );
				}
				else {
					node1=readNodes.get( srcNodeName );
				}

				//vytvorenie koncoveho uzla
				if ( !readNodes.contains( dstNodeName ) ) {
					node2 = context.getGraph().addNode( dstNodeName, nodeType );
					readNodes.addNode( dstNodeName, node2 );
				}
				else {
					node2=readNodes.get( dstNodeName );
				}
				//vytvorenie celej hyperhrany
				osg::ref_ptr<Data::Node> hyperEdgeNode;
				QMap<qlonglong, osg::ref_ptr<Data::Edge> >* mapa = context.getGraph().getEdges();

				hyperEdgeNode=RSFImporter().getHyperEdge( srcNodeName,edgeName,mapa );
				if ( !hyperEdgeNode.valid() ) {
					hyperEdgeNode = context.getGraph().addHyperEdge( edgeName );
					context.getGraph().addEdge( QString( "" ), node1, hyperEdgeNode, edgeType, true );
				}

				//pridanie hyperhrany do grafu
				context.getGraph().addEdge( QString( "" ), hyperEdgeNode, node2, edgeType, true );
			}
		}

	}

	return true;
}

} // namespace

#if defined(__linux) || defined(__linux__) || defined(linux)
#pragma GCC diagnostic pop
#endif

