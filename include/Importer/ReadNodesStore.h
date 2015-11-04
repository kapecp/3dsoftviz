#ifndef Importer_ReadNodesStore_H
#define Importer_ReadNodesStore_H
//-----------------------------------------------------------------------------
#include "Data/Node.h"
//-----------------------------------------------------------------------------
#include <osg/ref_ptr>
#include <QString>
//-----------------------------------------------------------------------------
#include <QMap>

namespace Importer {

/**
 * \brief Stores a "node name" to "node" map.
 * Used by the importers to store imported nodes and search them by name when the
 * edges reference them.
 */
class ReadNodesStore
{

public:

	/***/
	virtual ~ReadNodesStore( void ) {}

	/**
	 * \brief Adds a new node.
	 * \param[in] name Node name.
	 * \param[in] node Node to add.
	 */
	void addNode(
		const QString& name,
		osg::ref_ptr<Data::Node> node
	);

	/**
	 * \brief Checks if this store contains a node with the specified name.
	 * \param[in] name Node name to check.
	 * \return True if there is a node with the name.
	 */
	bool contains(
		const QString& name
	);

	/**
	 * \brief Gets the node with the specified name.
	 * \param[in] name Node name.
	 * \return Node with the name or NULL, if it does not exist.
	 */
	osg::ref_ptr<Data::Node> get(
		const QString& name
	);

private:

	QMap<QString, osg::ref_ptr<Data::Node>> nodes;

}; // class

} // namespace

#endif // Importer_ReadNodesStore_H
