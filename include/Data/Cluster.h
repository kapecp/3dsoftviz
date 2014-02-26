#ifndef NODECLUSTER_H_
#define NODECLUSTER_H_

#include "Data/Node.h"
#include <QSet>

namespace Data {
class Graph;

class Cluster : public Node {
public:
    Cluster(qlonglong id, QString name, Data::Type* type, float scaling, Data::Graph* graph, osg::Vec3f position);

    qlonglong getId() const { return id; }

//	void setCluster(Cluster* cluster);
    QSet<Node*> getIncidentNodes() const;
    void addNodeToCluster(Node* node) {clusteredNodes.insert(node);}
    // vracia nody a clustre kt. su vo vnutri tohto clustru
    QSet<Node*> getClusteredNodes() const {return clusteredNodes;}
    // vracia nody kt. su v tomto clustri spolu s nodmi ktore su v clastroch v tomto clustri - rekurzivne
    QSet<Node*> getALLClusteredNodes() const;
    int getClusteredNodesCount() const {return clusteredNodes.size();}

private:

    qlonglong id;
    QSet<Node *> clusteredNodes;

};

}

#endif  /*NODECLUSTER_H_*/
