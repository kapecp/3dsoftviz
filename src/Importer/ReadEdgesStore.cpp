#include "Importer/ReadEdgesStore.h"
#include "Data/Edge.h"

void Importer::ReadEdgesStore::addEdge(
	const QString& name,
	osg::ref_ptr<Data::Edge> edge
)
{
	edges[name] = edge;
}

bool Importer::ReadEdgesStore::contains(
	const QString& name
)
{
	return edges.contains( name );
}

osg::ref_ptr<Data::Edge> Importer::ReadEdgesStore::get(
	const QString& name
)
{
	if ( edges.contains( name ) ) {
		return edges.value( name );
	}
	else {
		return nullptr;
	}

}

