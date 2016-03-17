#ifndef Importer_GXLImporter_H
#define Importer_GXLImporter_H
//-----------------------------------------------------------------------------
#include "Importer/StreamImporter.h"
#include "Importer/GraphOperations.h"
#include "Importer/ReadNodesStore.h"
//-----------------------------------------------------------------------------
#include <QtXml/QXmlStreamReader>
//-----------------------------------------------------------------------------
#include <memory>

namespace Importer {

/**
 * \brief Imports graph data from data in GXL format.
 */
class GXLImporter
	: public StreamImporter
{

public:

	/***/
	virtual ~GXLImporter( void ) {}

	/***/
	virtual bool import(
		ImporterContext& context
	);

private:

	/**
	 * \brief Parses one graph (or subgraph).
	 * [recursive]
	 * <graph> XML node must already have been processed
	 * </graph> XML node is processed by this function
	 */
	bool parseGraph( void );

private:

	/**
	 * \brief Holds input (file stream) and output (graph structure and objects used for import process information reporting).
	 */
	ImporterContext* context_;

	// helpers
	std::shared_ptr<QXmlStreamReader> xml_;
	std::shared_ptr<GraphOperations> graphOp_;

	/**
	 * \brief Used for searching for a node when an edge references it.
	 */
	std::shared_ptr<ReadNodesStore> readNodes_;

	//default types
	Data::Type* edgeType_;
	Data::Type* nodeType_;

}; // class

} // namespace

#endif // Importer_GXLImporter_H
