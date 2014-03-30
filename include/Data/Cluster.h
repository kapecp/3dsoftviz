#ifndef NODECLUSTER_H_
#define NODECLUSTER_H_

#include "Data/Node.h"
#include <QSet>

#include "Clustering/Figures/Cube.h"

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
    osg::Vec3 getMidpoint();
    void setSelected(bool selected, QLinkedList<osg::ref_ptr<Data::Node> > * pickedNodes);
    Node * getRandomNode();

    void setCube(Cube * cube) {this->cube = cube;}
    Cube * getCube() {return cube;}

private:

    qlonglong id;
    QSet<Node *> clusteredNodes;
    osg::Vec4 originalColor;

    Cube * cube;

};

}

#endif  /*NODECLUSTER_H_*/
