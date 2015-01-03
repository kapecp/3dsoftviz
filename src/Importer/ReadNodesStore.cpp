#include "Importer/ReadNodesStore.h"

namespace Importer {

void ReadNodesStore::addNode (
        const QString &name,
        osg::ref_ptr<Data::Node> node
        ) {
    nodes_[name] = node;
}

bool ReadNodesStore::contains (
        const QString &name
        ) {
    return nodes_.find (name) != nodes_.end ();
}

osg::ref_ptr<Data::Node> ReadNodesStore::get (
        const QString &name
        ) {
    NodesMapType::iterator it = nodes_.find (name);
    if (it != nodes_.end ()) {
        return it->second;
    } else {
        return NULL;
    }
}

} // namespace
