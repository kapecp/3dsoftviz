#include "Importer/GraphMLImporter.h"
//-----------------------------------------------------------------------------

namespace Importer {

bool GraphMLImporter::import (
	ImporterContext &context
) {
	edgeTypeAttribute_ = appConf->getValue("GraphMLParser.edgeTypeAttribute");
	nodeTypeAttribute_ = appConf->getValue("GraphMLParser.nodeTypeAttribute");

	// nodes
	for (QDomElement nodeElement = graphElement.firstChildElement("node"); ok && !nodeElement.isNull(); nodeElement = nodeElement.nextSiblingElement("node"))
	{
		QString nameId = nodeElement.attribute("id");
		QString name = NULL;
		// pozerame sa na data ktore nesie
		Data::Type *newNodeType;
		newNodeType = NULL;
		QDomNodeList nodeDataList = nodeElement.elementsByTagName("data");
		for (unsigned int j = 0; j < nodeDataList.length(); j++){
			QDomNode nodeData = nodeDataList.item(j);
			if (!nodeData.isNull() && nodeData.isElement())
			{

	// default direction
	bool defaultDirection;
	if(graphElement.attribute("edgedefault") == "directed"){
		defaultDirection = true;
	} else {
		defaultDirection = false;
	}

	// edges
	for (QDomElement edgeElement = graphElement.firstChildElement("edge"); ok && !edgeElement.isNull(); edgeElement = edgeElement.nextSiblingElement("edge"))
	{
		QString sourceId = edgeElement.attribute("source");
		QString targetId = edgeElement.attribute("target");

		QString direction = NULL;
		bool directed = false;
		direction = edgeElement.attribute("directed");
		if(direction == NULL) {
			directed = defaultDirection;
			if(directed)
				direction = "_directed";
			else
				direction = "";
		} else {
			if(direction == "true"){
				direction = "_directed";
				directed = true;
			} else {
				direction = "";
				directed = false;
			}
		}


					} else {
						// kazde dalsie data nacitame do nosica dat - Edge.name
						// FIXME potom prerobit cez Adamove Node.settings
					}
			}
		}


			if (ok) {
				ok = processGraph(subgraphElement);
			}

			if (ok) {
				// TODO: end subgraph in edge
			}

					if (direction == QString("undir")) {
						direction = QString();
					}

					ok =
						direction.isEmpty()
						||
						(direction == QString("in"))
						||
						(direction == QString("out"))
					;

					context_->getInfoHandler ().reportError (ok, "Hyperedge endpoint - invalid direction.");
				}

				if (ok) {
					if (direction.isEmpty()) {
						direction = "none";
					}
				}

				if (ok) {
					// TODO: add endpoint
				}
			}
		}
	}

	return ok;
}

} // namespace