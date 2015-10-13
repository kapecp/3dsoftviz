#include "Importer/ReadEdgesStore.h"

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
    return edges.find( name ) != edges.end();
}

osg::ref_ptr<Data::Edge> Importer::ReadEdgesStore::get(
	const QString& name
)
{
    QMap<QString, osg::ref_ptr<Data::Edge>>::iterator it = edges.find( name );
    if ( it != edges.end() ) {
        return it.value();
	}
	else {
		return NULL;
	}
}

