#ifndef GITGRAPHIMPORTER_H
#define GITGRAPHIMPORTER_H

#include "Importer/StreamImporter.h"

#include <QList>
#include <QMap>

namespace Data {
class Type;
}
namespace Repository {
namespace Git {
class GitFile;
}
}
namespace Importer {
class ReadNodesStore;
class ReadEdgesStore;
class GraphOperations;

class GitGraphImporter : public StreamImporter
{
public:
	//public virtual methods
	/**
	 * Explicit default destructor
	 * @brief ~GitGraphImporter
	 */
	virtual ~GitGraphImporter();

	/**
	 * virtual bool import( ImporterContext &context )
	 * @brief Imports nodes and edges with their specific types into the graph.
	 * @param context Context for necessary items for graph import
	 * @return Returns true, if import of nodes and edges was successful, otherwise returns false.
	 */
	virtual bool import( ImporterContext& context );

	/**
	 * virtual bool makeGraph( QList<Repository::Git::GitFile*> gitFiles )
	 * @brief Creates nodes and edges and appends them to the Data::Graph
	 * @param gitFiles List of files in first version of git repository
	 * @return Returns true, if import of nodes and edges was successful, otherwise returns false.
	 */
	virtual bool makeGraph( QList<Repository::Git::GitFile*> gitFiles );

	/**
	 * virtual bool addNode( QStringList &list )
	 * @brief Adds nodes for single project path of git file
	 * @param list List of nodes path for single project path of git file
	 * @return Returns true, if import of nodes was successful, otherwise returns false.
	 */
	virtual bool addNode( QStringList& list );

	/**
	 * virtual bool addEdge( QStringList &list )
	 * @brief Adds edges for single project path of git file
	 * @param list List of edges path for single project path of git file
	 * @return Returns true, if import of edges was successful, otherwise returns false.
	 */
	virtual bool addEdge( QStringList& list );

private:
	//private variables
	/**
	 * ImporterContext *context
	 * @brief Context for necessary items for graph import
	 */
	ImporterContext* context;

	/**
	 * QString filepath
	 * @brief Path to the git repository on file system
	 */
	QString filepath;

	/**
	 * std::auto_ptr<Importer::GraphOperations> graphOp
	 * @brief Pointer to class, which sets up default edge and node type
	 */
	std::auto_ptr<Importer::GraphOperations> graphOp;

	/**
	 * std::auto_ptr<Importer::ReadNodesStore> readNodes
	 * @brief Pointer to store of processed nodes, which restrains appearance of node duplicity
	 */
	std::auto_ptr<Importer::ReadNodesStore> readNodes;

	/**
	 * std::auto_ptr<Importer::ReadEdgesStore> readEdges
	 * @brief Pointer to store of processed edges, which restrains appearance of edge duplicity
	 */
	std::auto_ptr<Importer::ReadEdgesStore> readEdges;

	// default types
	/**
	 * Data::Type* edgeType
	 * @brief Default edge type
	 */
	Data::Type* edgeType;

	/**
	 * Data::Type* edgeType
	 * @brief Default node type
	 */
	Data::Type* nodeType;

}; // class
} // namespace
#endif // GITGRAPHIMPORTER_H
