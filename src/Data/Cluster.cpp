
#include "Data/Cluster.h"

using namespace Data;

Cluster::Cluster(qlonglong id, QString name, Data::Type* type, float scaling, Data::Graph* graph, osg::Vec3f position)
    : Node(id, name, type, scaling, graph, position) {

    this->id = id;
}

QSet<Node*> Cluster::getIncidentNodes() const {
    QSet<Node*> nodes;

    QSet<Node *>::const_iterator i = clusteredNodes.constBegin();
    while (i != clusteredNodes.constEnd()) {
        Node* v = *i;
        nodes.unite(v->getIncidentNodes());
        ++i;
    }
    nodes.subtract(clusteredNodes);

    Cluster* n = const_cast<Cluster*>(this);
    nodes.remove(n);
    nodes.remove(n);

    return nodes;
}

QSet<Node*> Cluster::getALLClusteredNodes() const {
    QSet<Node*> nodes;

    QSet<Node *>::const_iterator i = clusteredNodes.constBegin();
    while (i != clusteredNodes.constEnd()) {
        Node* clusteredNode = *i;

        Cluster* cluster = dynamic_cast<Cluster*>(clusteredNode);
        if(cluster != 0) { // je to cluster
            nodes.unite(cluster->getALLClusteredNodes());
        } else { // je to node
            nodes.insert(clusteredNode);
        }
        ++i;
    }

    return nodes;
}
