#ifndef Importer_GraphMLImporter_H
#define Importer_GraphMLImporter_H
//-----------------------------------------------------------------------------
#include "Importer/StreamImporter.h"
#include "Importer/GraphOperations.h"
#include "Importer/ReadNodesStore.h"
//-----------------------------------------------------------------------------
#include <QtXml/QDomElement>
//-----------------------------------------------------------------------------

namespace Importer {

/**
 * \brief Imports graph data from data in GraphML format.
 */
class GraphMLImporter
	: public StreamImporter {

public:

	/***/
	virtual ~GraphMLImporter (void) {};

	/***/
	virtual bool import (
		ImporterContext &context
	);

private:

	/**
	 * [recursive]
	 */
	bool processGraph (
		QDomElement &graphElement
	);

	bool processGraph_Nodes (
		QDomElement &graphElement
	);

	bool processGraph_Edges (
		QDomElement &graphElement
	);

	bool processGraph_Hyperedges (
		QDomElement &graphElement
	);

private:

	// context
	ImporterContext *context_;

	// helpers
	std::auto_ptr<GraphOperations> graphOp_;
	std::auto_ptr<ReadNodesStore> readNodes_;

	//default types
	Data::Type *edgeType_;
	Data::Type *nodeType_;

	QString edgeTypeAttribute_;
	QString nodeTypeAttribute_;

	int colors_;
	qint8 nodeTypeSettings_[6][4];
	int iColor_;

	// for progress reporting
	uint32_t entitiesProcessed_;
	uint32_t entitiesCount_;

}; // class

} // namespace

#endif // Importer_GraphMLImporter_H
