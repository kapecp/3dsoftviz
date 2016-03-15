#include "Importer/ReadNodesStore.h"

namespace Importer {

void ReadNodesStore::addNode(
	const QString& name,
	osg::ref_ptr<Data::Node> node
)
{
	nodes[name] = node;
}

bool ReadNodesStore::contains(
	const QString& name
)
{
	return nodes.contains( name );
}

osg::ref_ptr<Data::Node> ReadNodesStore::get(
	const QString& name
)
{
	if ( nodes.contains( name ) ) {
		return nodes.value( name );
	}
	else {
		return nullptr;
	}
}

} // namespace
