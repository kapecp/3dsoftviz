#include "Importer/GXLImporter.h"
//-----------------------------------------------------------------------------

namespace Importer {

bool GXLImporter::parseSubGraph (void) {
	bool ok = true;

	bool edgeOrientedDefault = false;
	bool edgeOrientedDefaultForce = false;

	// current "graph" node
	if (ok) {
		QXmlStreamAttributes attrs = xml_->attributes();

		QStringRef graphEdgeMode = attrs.value ("edgemode");
		if (!graphEdgeMode.isEmpty ()) {
			if (graphEdgeMode == "defaultdirected") {
				edgeOrientedDefault = true;
				edgeOrientedDefaultForce = false;
			} else if (graphEdgeMode == "defaultundirected") {
				edgeOrientedDefault = false;
				edgeOrientedDefaultForce = false;
			} else if (graphEdgeMode == "directed") {
				edgeOrientedDefault = true;
				edgeOrientedDefaultForce = true;
			} else if (graphEdgeMode == "undirected") {
				edgeOrientedDefault = false;
				edgeOrientedDefaultForce = true;
			} else {
				ok = false;

				context_->getInfoHandler ().reportError ("Invalid edgemode value.");
			}
		}
	}

	while (ok && !xml_->atEnd ()) {
		QXmlStreamReader::TokenType token;
		if (ok) {
			token = xml_->readNext();
		}

		if (ok) {
			if (
				(token == QXmlStreamReader::StartElement)
				&&
				(xml_->name () == "node")
			) {
				QXmlStreamAttributes attrs = xml_->attributes();

				QString nodeName;
				if (ok) {
					nodeName = attrs.value ("id").toString ();

					ok = !(nodeName.isEmpty ());

					context_->getInfoHandler ().reportError (ok, "Node ID can not be empty.");
				}

				osg::ref_ptr<Data::Node> node (NULL);
				if (ok) {
					node = context_->getGraph ().addNode (nodeName, nodeType);

					ok = node.valid ();

					context_->getInfoHandler ().reportError (ok, "Unable to add new node.");
				}

				if (ok) {
					readNodes_->addNode (nodeName, node);
				}
			}

			if (
				(token == QXmlStreamReader::StartElement)
				&&
				(xml_->name () == "edge")
			) {
				QXmlStreamAttributes attrs = xml_->attributes();

				bool oriented = edgeOrientedDefault;
				if (ok) {
					QStringRef edgeIsDirected = attrs.value ("isdirected");

					if (!edgeIsDirected.isEmpty ()) {
						if (edgeIsDirected == "true") {
							if (
								(!edgeOrientedDefaultForce)
								||
								(edgeOrientedDefault)
							) {
								oriented = true;
							} else {
								ok = false;
								context_->getInfoHandler ().reportError ("Can not replace global edge mode with edgeIsDirected=\"true\".");
							}
						} else if (edgeIsDirected == "false") {
							if (
								(!edgeOrientedDefaultForce)
								||
								(!edgeOrientedDefault)
							) {
								oriented = false;
							} else {
								ok = false;
								context_->getInfoHandler ().reportError ("Can not replace global edge mode with edgeIsDirected=\"false\".");
							}
						} else {
							context_->getInfoHandler ().reportError (ok, "Invalid edgeIsDirected value.");
						}
					}
				}

				QString nodeFromName;
				if (ok) {
					nodeFromName = attrs.value ("from").toString ();

					ok = !(nodeFromName.isEmpty ());

					context_->getInfoHandler ().reportError (ok, "Edge \"from\" attribute can not be empty.");
				}

				QString nodeToName;
				if (ok) {
					nodeToName = attrs.value ("to").toString ();

					ok = !(nodeToName.isEmpty ());

					context_->getInfoHandler ().reportError (ok, "Edge \"to\" attribute can not be empty.");
				}

				QString edgeName = nodeFromName + nodeToName;

				if (ok) {
					ok = readNodes_->contains (nodeFromName);

					context_->getInfoHandler ().reportError (ok, "Edge references invalid source node.");
				}

				if (ok) {
					ok = readNodes_->contains (nodeToName);

					context_->getInfoHandler ().reportError (ok, "Edge references invalid destination node.");
				}

				if (ok) {
					osg::ref_ptr<Data::Edge> edge = context_->getGraph().addEdge(
						edgeName,
						readNodes_->get (nodeFromName),
						readNodes_->get (nodeToName),
						edgeType,
						oriented
					);

					// ok = edge.valid ();

					// context_->getInfoHandler ().reportError (ok, "Unable to add new edge.");
					// can not be checked because addEdge returns null when a multiedge is added
				}
			}
			if (
				(token == QXmlStreamReader::EndElement)
				&&
				(xml_->name () == "graph")
			) {
				break;
			}
		}

		if (ok) {
			ok = !xml_->hasError ();

			context_->getInfoHandler ().reportError (ok, "XML format error.");
		}
	}

	return ok;
}

bool GXLImporter::import (
	ImporterContext &context
) {
	// context
	context_ = &context;

	// helpers
	xml_.reset (new QXmlStreamReader (&(context_->getStream ())));
	graphOp_.reset (new GraphOperations (context_->getGraph ()));
	readNodes_.reset (new ReadNodesStore());

	bool ok = true;

	// check XML
	if (ok) {
		ok = !xml_->hasError ();

		context_->getInfoHandler ().reportError (ok, "XML format error.");
	}

	// default types
	edgeType = NULL;
	nodeType = NULL;
	(void)graphOp_->addDefaultTypes (edgeType, nodeType);

	while (ok && !xml_->atEnd ()) {
		QXmlStreamReader::TokenType token;
		if (ok) {
			token = xml_->readNext();
		}

		if (ok) {
			if (
				(token == QXmlStreamReader::StartElement)
				&&
				(xml_->name () == "graph")
			) {
				QXmlStreamAttributes attrs = xml_->attributes();

				QString graphName;
				if (ok) {
					graphName = attrs.value ("id").toString ();

					ok = ("" != graphName);

					context_->getInfoHandler ().reportError (ok, "Graph name can not be empty.");
				}

				if (ok) {
					// ok = (graphName == context_->getGraph ().setName (graphName));

					context_->getInfoHandler ().reportError (ok, "Unable to set graph name.");
				}

				if (ok) {
					ok = parseSubGraph ();

					context_->getInfoHandler ().reportError (ok, "Unable to parse subgraph.");
				}
			}
		}

		if (ok) {
			ok = !xml_->hasError ();

			context_->getInfoHandler ().reportError (ok, "XML format error.");
		}
	}

	xml_->clear ();

	return ok;
}

} // namespace
