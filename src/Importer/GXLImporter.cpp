#include "Importer/GXLImporter.h"
#include "Data/Graph.h"
//-----------------------------------------------------------------------------

namespace Importer {

bool GXLImporter::import(
	ImporterContext& context
)
{
	// context
	context_ = &context;

	// helpers
	xml_.reset( new QXmlStreamReader( &( context_->getStream() ) ) );
	graphOp_.reset( new GraphOperations( context_->getGraph() ) );
	readNodes_.reset( new ReadNodesStore() );

	// default types
	edgeType_ = NULL;
	nodeType_ = NULL;
	( void )graphOp_->addDefaultTypes( edgeType_, nodeType_ );

	bool ok = true;

	// check XML
	if ( ok ) {
		ok = !xml_->hasError();

		context_->getInfoHandler().reportError( ok, "XML format error." );
	}

	bool graphElementFound = false;
	//parsovanie grafu
	while ( ok && !xml_->atEnd() && !graphElementFound ) {
		if ( ok ) {
			QXmlStreamReader::TokenType token = xml_->readNext();
			if (
				( token == QXmlStreamReader::StartElement )
				&&
				( xml_->name() == "graph" )
			) {
				graphElementFound = true;
			}
		}

		if ( ok ) {
			ok = !xml_->hasError();

			context_->getInfoHandler().reportError( ok, "XML format error." );
		}
	}

	if ( ok ) {
		ok = graphElementFound;

		context_->getInfoHandler().reportError( ok, "Unable to find graph element." );
	}

	QXmlStreamAttributes attrs;
	if ( ok ) {
		attrs = xml_->attributes();
	}


	if ( ok ) {
		ok = parseGraph();
	}

	xml_->clear();

	return ok;
}

bool GXLImporter::parseGraph( void )
{
	bool ok = true;

	bool edgeOrientedDefault = false;
	bool edgeOrientedDefaultForce = false;

	// current "graph" node
	if ( ok ) {
		QXmlStreamAttributes attrs = xml_->attributes();

		QStringRef graphEdgeMode = attrs.value( "edgemode" );
		if ( !graphEdgeMode.isEmpty() ) {
			if ( graphEdgeMode == "defaultdirected" ) {
				edgeOrientedDefault = true;
				edgeOrientedDefaultForce = false;
			}
			else if ( graphEdgeMode == "defaultundirected" ) {
				edgeOrientedDefault = false;
				edgeOrientedDefaultForce = false;
			}
			else if ( graphEdgeMode == "directed" ) {
				edgeOrientedDefault = true;
				edgeOrientedDefaultForce = true;
			}
			else if ( graphEdgeMode == "undirected" ) {
				edgeOrientedDefault = false;
				edgeOrientedDefaultForce = true;
			}
			else {
				ok = false;

				context_->getInfoHandler().reportError( "Invalid edgemode value." );
			}
		}
	}

	osg::ref_ptr<Data::Node> currentNode( NULL );
	osg::ref_ptr<Data::Edge> currentEdge( NULL );
	osg::ref_ptr<Data::Node> hyperEdgeNode( NULL );
	bool inHyperedge = false;

	while ( ok && !xml_->atEnd() ) {
		if ( ok ) {
			QXmlStreamReader::TokenType token = xml_->readNext();
			// vnoreny graf
			if (
				( token == QXmlStreamReader::StartElement )
				&&
				( xml_->name() == "graph" )
			) {
				if ( ok ) {
					if ( currentNode ) {
						context_->getGraph().createNestedGraph( currentNode );
					}
					else if ( currentEdge ) {
						// moznost pridania vnoreneho grafu do hrany
					}
					else {
						ok = false;

						context_->getInfoHandler().reportError( "Subgraph found, but it is not placed in node/edge." );
					}
				}

				if ( ok ) {
					ok = parseGraph();
				}

				if ( ok ) {
					context_->getGraph().closeNestedGraph();
				}
			}

			// parsovanie uzla
			if (
				( token == QXmlStreamReader::StartElement )
				&&
				( xml_->name() == "node" )
			) {
				if ( ok ) {
					ok = ( !currentNode ) && ( !currentEdge ) && ( !inHyperedge );

					context_->getInfoHandler().reportError( ok, "Node in node/edge/hyperedge found." );
				}

				QXmlStreamAttributes attrs = xml_->attributes();

				QString nodeName;
				if ( ok ) {
					nodeName = attrs.value( "id" ).toString();

					ok = !( nodeName.isEmpty() );

					context_->getInfoHandler().reportError( ok, "Node ID can not be empty." );
				}

				osg::ref_ptr<Data::Node> node( NULL );
				if ( ok ) {
					node = context_->getGraph().addNode( nodeName, nodeType_ );

					ok = node.valid();

					context_->getInfoHandler().reportError( ok, "Unable to add new node." );
				}

				if ( ok ) {
					readNodes_->addNode( nodeName, node );
				}

				if ( ok ) {
					currentNode = node;
				}
			}

			if (
				( token == QXmlStreamReader::EndElement )
				&&
				( xml_->name() == "node" )
			) {
				if ( ok ) {
					ok = currentNode != nullptr;

					context_->getInfoHandler().reportError( ok, "Node end without matched node begin." );
				}

				if ( ok ) {
					( void )currentNode.release();
				}
			}

			// parsovanie hrany
			if (
				( token == QXmlStreamReader::StartElement )
				&&
				( xml_->name() == "edge" )
			) {
				if ( ok ) {
					ok = ( !currentNode ) && ( !currentEdge ) && ( !inHyperedge );

					context_->getInfoHandler().reportError( ok, "Edge in node/edge/hyperedge found." );
				}

				QXmlStreamAttributes attrs = xml_->attributes();

				bool oriented = edgeOrientedDefault;
				if ( ok ) {
					QStringRef edgeIsDirected = attrs.value( "isdirected" );

					if ( !edgeIsDirected.isEmpty() ) {
						if ( edgeIsDirected == "true" ) {
							if (
								( !edgeOrientedDefaultForce )
								||
								( edgeOrientedDefault )
							) {
								oriented = true;
							}
							else {
								ok = false;
								context_->getInfoHandler().reportError( "Can not replace global edge mode with edgeIsDirected=\"true\"." );
							}
						}
						else if ( edgeIsDirected == "false" ) {
							if (
								( !edgeOrientedDefaultForce )
								||
								( !edgeOrientedDefault )
							) {
								oriented = false;
							}
							else {
								ok = false;
								context_->getInfoHandler().reportError( "Can not replace global edge mode with edgeIsDirected=\"false\"." );
							}
						}
						else {
							context_->getInfoHandler().reportError( ok, "Invalid edgeIsDirected value." );
						}
					}
				}

				//cielovy a zdrojovy uzol hrany
				QString nodeFromName;
				if ( ok ) {
					nodeFromName = attrs.value( "from" ).toString();

					ok = !( nodeFromName.isEmpty() );

					context_->getInfoHandler().reportError( ok, "Edge \"from\" attribute can not be empty." );
				}

				QString nodeToName;
				if ( ok ) {
					nodeToName = attrs.value( "to" ).toString();

					ok = !( nodeToName.isEmpty() );

					context_->getInfoHandler().reportError( ok, "Edge \"to\" attribute can not be empty." );
				}

				QString edgeName = nodeFromName + nodeToName;

				if ( ok ) {
					ok = readNodes_->contains( nodeFromName );

					context_->getInfoHandler().reportError( ok, "Edge references invalid source node." );
				}

				if ( ok ) {
					ok = readNodes_->contains( nodeToName );

					context_->getInfoHandler().reportError( ok, "Edge references invalid destination node." );
				}

				osg::ref_ptr<Data::Edge> edge( NULL );
				if ( ok ) {
					edge = context_->getGraph().addEdge(
							   edgeName,
							   readNodes_->get( nodeFromName ),
							   readNodes_->get( nodeToName ),
							   edgeType_,
							   oriented
						   );
				}

				if ( ok ) {
					currentEdge = edge;
				}
			}

			if (
				( token == QXmlStreamReader::EndElement )
				&&
				( xml_->name() == "edge" )
			) {
				if ( ok ) {
					ok = currentEdge != nullptr;

					context_->getInfoHandler().reportError( ok, "Edge end without matched edge begin." );
				}

				if ( ok ) {
					( void )currentEdge.release();
				}
			}

			// hyperhrana

			if (
				( token == QXmlStreamReader::StartElement )
				&&
				( xml_->name() == "rel" )
			) {

				if ( ok ) {
					ok = ( !currentNode ) && ( !currentEdge ) && ( !inHyperedge );

					context_->getInfoHandler().reportError( ok, "Hyperedge in node/edge/hyperedge found." );
				}
				QXmlStreamAttributes attrs = xml_->attributes();
				QString hyperEdgeName;
				if ( ok ) {
					hyperEdgeName = attrs.value( "id" ).toString();

					ok = !( hyperEdgeName.isEmpty() );

					context_->getInfoHandler().reportError( ok, "Node ID can not be empty." );
				}

				if ( ok ) {
					// zaciatok hyperhrany
					hyperEdgeNode = context_->getGraph().addHyperEdge( hyperEdgeName );
				}

				if ( ok ) {
					inHyperedge = true;
				}
			}

			if (
				( token == QXmlStreamReader::EndElement )
				&&
				( xml_->name() == "rel" )
			) {
				if ( ok ) {
					ok = inHyperedge;

					context_->getInfoHandler().reportError( ok, "Hyperedge end without matched hyperedge begin." );
				}

				if ( ok ) {
					inHyperedge = false;
				}
			}

			// ukoncenie hyperhrany
			if (
				( token == QXmlStreamReader::StartElement )
				&&
				( xml_->name() == "relend" )
			) {
				if ( ok ) {
					ok = inHyperedge;

					context_->getInfoHandler().reportError( ok, "Hyperedge endpoint without hyperedge." );
				}

				QXmlStreamAttributes attrs = xml_->attributes();

				QString targetName;
				if ( ok ) {
					targetName = attrs.value( "target" ).toString();

					ok = !( targetName.isEmpty() );

					context_->getInfoHandler().reportError( ok, "Hyperedge endpoint \"target\" attribute can not be empty." );
				}

				if ( ok ) {
					ok = readNodes_->contains( targetName );

					context_->getInfoHandler().reportError( ok, "Hyperedge endpoint references invalid target node." );
				}

				osg::ref_ptr<Data::Node> target( NULL );
				if ( ok ) {
					target = readNodes_->get( targetName );
				}

				QString direction;
				if ( ok ) {
					direction = attrs.value( "direction" ).toString();

					if ( direction == QString( "none" ) ) {
						direction = QString();
					}

					ok =
						direction.isEmpty()
						||
						( direction == QString( "in" ) )
						||
						( direction == QString( "out" ) )
						;

					context_->getInfoHandler().reportError( ok, "Hyperedge endpoint - invalid direction." );
				}

				if ( ok ) {
					if ( direction.isEmpty() ) {
						direction = "none";
					}
				}

				if ( ok ) {
					// TODO: add endpoint
					if ( direction==QString( "in" ) ) {
						context_->getGraph().addEdge( "", target, hyperEdgeNode, nodeType_, true );
					}
					else if ( direction==QString( "out" ) ) {
						context_->getGraph().addEdge( "", hyperEdgeNode, target, nodeType_, true );
					}
					else {
						context_->getGraph().addEdge( "", hyperEdgeNode, target, nodeType_, false );
					}
				}
			}

			if (
				( token == QXmlStreamReader::EndElement )
				&&
				( xml_->name() == "relend" )
			) {

			}

			// this graph end
			if (
				( token == QXmlStreamReader::EndElement )
				&&
				( xml_->name() == "graph" )
			) {
				break;
			}
		}

		if ( ok ) {
			ok = !xml_->hasError();

			context_->getInfoHandler().reportError( ok, "XML format error." );
		}
	}

	// TODO: graf vnoreny v hyperhrane

	return ok;
}

} // namespace
