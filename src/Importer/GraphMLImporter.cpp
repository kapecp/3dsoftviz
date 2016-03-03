#include "Importer/GraphMLImporter.h"
//-----------------------------------------------------------------------------
#include "Data/Graph.h"
#include "Util/ApplicationConfig.h"

namespace Importer {

bool GraphMLImporter::import(
	ImporterContext& context
)
{
	// context
	context_ = &context;
	// helpers
	graphOp_.reset( new GraphOperations( context_->getGraph() ) );
	readNodes_.reset( new ReadNodesStore() );

	// default types
	edgeType_ = NULL;
	nodeType_ = NULL;
	( void )graphOp_->addDefaultTypes( edgeType_, nodeType_ );

	// ziskame pristup ku nastaveniam
	Util::ApplicationConfig* appConf = Util::ApplicationConfig::get();
	edgeTypeAttribute_ = appConf->getValue( "GraphMLParser.edgeTypeAttribute" );
	nodeTypeAttribute_ = appConf->getValue( "GraphMLParser.nodeTypeAttribute" );

	// pole farieb FIXME oddelit farby hran od farieb uzlov
	colors_.push_back( ColorType( 0, 1, 0, 1 ) );
	colors_.push_back( ColorType( 0, 1, 1, 1 ) );
	colors_.push_back( ColorType( 1, 0, 0, 1 ) );
	colors_.push_back( ColorType( 1, 0, 1, 1 ) );
	colors_.push_back( ColorType( 1, 1, 0, 1 ) );
	colors_.push_back( ColorType( 1, 1, 1, 1 ) );

	iColor_ = 0;

	bool ok = true;

	// ziskame graph element
	QDomElement graphElement;
	if ( ok ) {
		QDomNode graphNode;

		QDomDocument doc( "graphMLDocument" );
		if ( doc.setContent( &( context_->getStream() ) ) ) {
			QDomElement docElem = doc.documentElement();
			if ( !docElem.isNull() && docElem.nodeName() == "graphml" ) {
				QDomNodeList graphNodes = docElem.elementsByTagName( "graph" );
				if ( graphNodes.length() > 0 ) {
					graphNode = graphNodes.item( 0 );
					if ( !graphNode.isNull() && graphNode.parentNode() == docElem && graphNode.isElement() ) {
						graphElement = graphNode.toElement();
					}
				}
			}
		}

		ok = !graphElement.isNull();

		context_->getInfoHandler().reportError( ok, "Zvoleny subor nie je validny GraphML subor." );
	}

	if ( ok ) {
		// for progress reporting
		entitiesProcessed_ = 0;
		entitiesCount_ = graphElement.elementsByTagName( "node" ).size() + graphElement.elementsByTagName( "edge" ).count();
	}

	if ( ok ) {
		ok = processGraph( graphElement );
	}

	return ok;
}

bool GraphMLImporter::processGraph(
	QDomElement& graphElement
)
{
	bool ok;

	ok =
		processGraph_Nodes( graphElement )
		&&
		processGraph_Edges( graphElement )
		&&
		processGraph_Hyperedges( graphElement )
		;

	return ok;
}

bool GraphMLImporter::processGraph_Nodes(
	QDomElement& graphElement
)
{
	bool ok = true;

	iColor_ = 0;

	// nodes
	for ( QDomElement nodeElement = graphElement.firstChildElement( "node" ); ok && !nodeElement.isNull(); nodeElement = nodeElement.nextSiblingElement( "node" ) ) {
		QString nameId = nodeElement.attribute( "id" );
		QString name = NULL;
		// pozerame sa na data ktore nesie
		Data::Type* newNodeType;
		newNodeType = NULL;
		QDomNodeList nodeDataList = nodeElement.elementsByTagName( "data" );
		for ( unsigned int j = 0; j < nodeDataList.length(); j++ ) {
			QDomNode nodeData = nodeDataList.item( static_cast<int>( j ) );
			if ( !nodeData.isNull() && nodeData.isElement() ) {
				QDomElement nodeDataElement = nodeData.toElement();
				QString dataName = nodeDataElement.attribute( "key" );
				QString dataValue = nodeDataElement.text();
				// rozpoznavame typy
				if ( dataName == nodeTypeAttribute_ ) {
					// overime ci uz dany typ existuje v grafe
					QList<Data::Type*> types = context_->getGraph().getTypesByName( dataValue );
					if ( types.isEmpty() ) {
						QMap<QString, QString>* settings = new QMap<QString, QString>;

						settings->insert( "color.R", QString::number( colors_[iColor_][0] ) );
						settings->insert( "color.G", QString::number( colors_[iColor_][1] ) );
						settings->insert( "color.B", QString::number( colors_[iColor_][2] ) );
						settings->insert( "color.A", QString::number( colors_[iColor_][3] ) );
						settings->insert( "scale",		Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
						settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );

						newNodeType = context_->getGraph().addType( dataValue, settings );

						iColor_++;
						if ( iColor_ == colors_.size() ) {
							iColor_ = 0;
						}
					}
					else {
						newNodeType = types.first();
					}

				}
				else {
					// kazde dalsie data nacitame do nosica dat - Node.name
					// FIXME potom prerobit cez Adamove Node.settings
					if ( name == NULL ) {
						name = dataName+":"+dataValue;
					}
					else {
						name += " | "+dataName+":"+dataValue;
					}
				}
			}
		}

		// ak sme nenasli name, tak ako name pouzijeme aspon ID
		if ( name == NULL ) {
			name = nameId;
		}

		// ak nebol najdeny ziaden typ, tak pouzijeme defaultny typ
		osg::ref_ptr<Data::Node> node;
		if ( newNodeType == NULL ) {
			node = context_->getGraph().addNode( name, nodeType_ );
		}
		else {
			node = context_->getGraph().addNode( name, newNodeType );
		}
		readNodes_->addNode( nameId, node );

		// subgraphs
		for ( QDomElement subgraphElement = nodeElement.firstChildElement( "graph" ); ok && !subgraphElement.isNull(); subgraphElement = subgraphElement.nextSiblingElement( "graph" ) ) {
			if ( ok ) {
				context_->getGraph().createNestedGraph( node );
			}

			if ( ok ) {
				ok = processGraph( subgraphElement );
			}

			if ( ok ) {
				context_->getGraph().closeNestedGraph();
			}
		}

		entitiesProcessed_++;
		context_->getInfoHandler().setProgress( static_cast<unsigned int>( entitiesProcessed_ * 100 / entitiesCount_ ) );
	}

	return ok;
}

bool GraphMLImporter::processGraph_Edges(
	QDomElement& graphElement
)
{
	bool ok = true;

	iColor_ = 0;

	// default direction
	bool defaultDirection;
	if ( graphElement.attribute( "edgedefault" ) == "directed" ) {
		defaultDirection = true;
	}
	else {
		defaultDirection = false;
	}

	// edges
	for ( QDomElement edgeElement = graphElement.firstChildElement( "edge" ); ok && !edgeElement.isNull(); edgeElement = edgeElement.nextSiblingElement( "edge" ) ) {
		QString sourceId = edgeElement.attribute( "source" );
		QString targetId = edgeElement.attribute( "target" );

		QString direction = NULL;
		bool directed = false;
		direction = edgeElement.attribute( "directed" );
		if ( direction == NULL ) {
			directed = defaultDirection;
			if ( directed ) {
				direction = "_directed";
			}
			else {
				direction = "";
			}
		}
		else {
			if ( direction == "true" ) {
				direction = "_directed";
				directed = true;
			}
			else {
				direction = "";
				directed = false;
			}
		}

		// pozerame sa na data ktore hrana nesie
		Data::Type* newEdgeType;
		newEdgeType = NULL;
		QDomNodeList edgeDataList = edgeElement.elementsByTagName( "data" );
		for ( unsigned int j = 0; j < edgeDataList.length(); j++ ) {
			QDomNode edgeData = edgeDataList.item( static_cast<int>( j ) );
			if ( !edgeData.isNull() && edgeData.isElement() ) {
				QDomElement edgeDataElement = edgeData.toElement();
				QString dataName = edgeDataElement.attribute( "key" );
				QString dataValue = edgeDataElement.text();
				// rozpoznavame typy deklarovane atributom relation
				if ( dataName == edgeTypeAttribute_ ) {
					// overime ci uz dany typ existuje v grafe
					QList<Data::Type*> types = context_->getGraph().getTypesByName( dataValue+direction );
					if ( types.isEmpty() ) {
						QMap<QString, QString>* settings = new QMap<QString, QString>;

						// FIXME spravit tak, aby to rotovalo po tom poli - palo az to budes prerabat tak pre hrany pouzi ine pole, take co ma alfu na 0.5.. a to sa tyka aj uzlov s defaultnym typom
						settings->insert( "color.R", QString::number( colors_[iColor_][0] ) );
						settings->insert( "color.G", QString::number( colors_[iColor_][1] ) );
						settings->insert( "color.B", QString::number( colors_[iColor_][2] ) );
						settings->insert( "color.A", QString::number( colors_[iColor_][3] ) );
						settings->insert( "scale",		Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );

						if ( !directed ) {
							settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );
						}
						else {
							settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.OrientedEdgePrefix" ) );
							settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.OrientedEdgeSuffix" ) );
						}

						newEdgeType = context_->getGraph().addType( dataValue+direction, settings );

						iColor_++;
						if ( iColor_ == colors_.size() ) {
							iColor_ = 0;
						}
					}
					else {
						newEdgeType = types.first();
					}

				}
				else {
					// kazde dalsie data nacitame do nosica dat - Edge.name
					// FIXME potom prerobit cez Adamove Node.settings
				}
			}
		}

		// ak nebol najdeny typ, tak pouzijeme defaulty
		if ( newEdgeType == NULL ) {
			newEdgeType = edgeType_;
		}

		context_->getGraph().addEdge( sourceId+targetId, readNodes_->get( sourceId ), readNodes_->get( targetId ), newEdgeType, directed );

		// vnorene grafy
		for ( QDomElement subgraphElement = edgeElement.firstChildElement( "graph" ); ok && !subgraphElement.isNull(); subgraphElement = subgraphElement.nextSiblingElement( "graph" ) ) {
			if ( ok ) {
				// TODO: begin subgraph in edge
			}

			if ( ok ) {
				ok = processGraph( subgraphElement );
			}

			if ( ok ) {
				// TODO: end subgraph in edge
			}
		}

		entitiesProcessed_++;
		context_->getInfoHandler().setProgress( static_cast<unsigned int>( entitiesProcessed_ * 100 / entitiesCount_ ) );
	}

	return ok;
}

bool GraphMLImporter::processGraph_Hyperedges(
	QDomElement& graphElement
)
{
	bool ok = true;
	osg::ref_ptr<Data::Node> hyperEdgeNode;
	iColor_ = 0;
	int count=0;
	// hyperedges
	for ( QDomElement hyperedgeElement = graphElement.firstChildElement( "hyperedge" ); ok && !hyperedgeElement.isNull(); hyperedgeElement = hyperedgeElement.nextSiblingElement( "hyperedge" ) ) {
		if ( ok ) {
			// pridanie hyperhrany
			hyperEdgeNode = context_->getGraph().addHyperEdge( QString::number( count ) );
		}

		if ( ok ) {
			for ( QDomElement endpointElement = hyperedgeElement.firstChildElement( "endpoint" ); ok && !endpointElement.isNull(); endpointElement = endpointElement.nextSiblingElement( "endpoint" ) ) {
				QString targetName;
				if ( ok ) {
					targetName = endpointElement.attribute( "node" );
					ok = !( targetName.isEmpty() );

					context_->getInfoHandler().reportError( ok, "Hyperedge endpoint \"node\" attribute can not be empty." );
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
					direction = endpointElement.attribute( "type" );

					if ( direction == QString( "undir" ) ) {
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
					//orientovana hrana
					if ( direction==QString( "in" ) ) {
						context_->getGraph().addEdge( "", readNodes_->get( targetName ), hyperEdgeNode, nodeType_, true );
					}
					else if ( direction==QString( "out" ) ) {
						context_->getGraph().addEdge( "", hyperEdgeNode, readNodes_->get( targetName ), nodeType_, true );
					}
					else {
						context_->getGraph().addEdge( "", hyperEdgeNode, target, nodeType_, false );
					}
				}
			}
		}
		count++;
	}

	return ok;
}

} // namespace
