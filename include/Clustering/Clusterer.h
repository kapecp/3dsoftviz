#ifndef CLUSTERER_H_
#define CLUSTERER_H_

#include <QMap>
#include <QSet>
#include <osg/ref_ptr>

#include <QProgressDialog>

namespace Data {
    class Graph;
    class Node;
}

namespace Clustering {

/*
class IClusterable
{
    public:
        virtual QSet<Node*> getIncidentNodes() = 0;
};
*/

class Clusterer {
public:

    class AlgorithmType
    {
    public:
        static const int ADJACENCY = 0;
        static const int LEAF = 1;
        static const int NEIGHBOUR = 2;
    };

    //Clusterer();

    void cluster(Data::Graph* graph, QProgressDialog* clusteringProgressBar);

    // singleton
    static Clusterer& getInstance()
    {
        static Clusterer    instance;
        return instance;
    }

    QMap<qlonglong, osg::ref_ptr<Data::Node> > getClusters() {return clusters;}
    int getMaxCountOfNodesInClusters();

    void setAlgorithmType(int algorithmType) {this->algorithmType = algorithmType;}
    void setClusteringDepth(int clusteringDepth) {this->clusteringDepth = clusteringDepth;}

private:

    Data::Graph* graph;
    QMap<qlonglong, osg::ref_ptr<Data::Node> > clusters;
    int algorithmType;
    int clusteringDepth;
    QProgressDialog * progressBar;

    void clusterNeighbours(QMap<qlonglong, osg::ref_ptr<Data::Node> >* someNodes, int maxLevels);
    void clusterLeafs(QMap<qlonglong, osg::ref_ptr<Data::Node> >* someNodes, int maxLevels);
    void clusterAdjacency(QMap<qlonglong, osg::ref_ptr<Data::Node> >* someNodes, int maxLevels);

    int getNextId() {return idGen++;}

    // singleton start
    Clusterer() {
        colorCounter = 0;
        idGen = 0;
        clusteringDepth = 0;
        algorithmType = AlgorithmType::ADJACENCY;
    }
    Clusterer(Clusterer const&);              // Don't Implement.
    void operator=(Clusterer const&); // Don't implement
    // singleton end

    int colorCounter;
    int idGen;
    //osg::Vec4 getNewColor();
};

}

#endif /* CLUSTERER_H_ */
