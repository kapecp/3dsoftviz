#ifndef CLUSTERER_H_
#define CLUSTERER_H_

#include <QMap>
#include <QSet>
#include <osg/ref_ptr>

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

    //Clusterer();

    void cluster(Data::Graph* graph);

    // singleton
    static Clusterer& getInstance()
    {
        static Clusterer    instance;
        return instance;
    }

    QMap<qlonglong, osg::ref_ptr<Data::Node> > getClusters() {return clusters;}

private:

    Data::Graph* graph;
    QMap<qlonglong, osg::ref_ptr<Data::Node> > clusters;

    void clusterNeighbours(QMap<qlonglong, osg::ref_ptr<Data::Node> >* someNodes, int maxLevels);
    void clusterLeafs(QMap<qlonglong, osg::ref_ptr<Data::Node> >* someNodes, int maxLevels);
    void clusterAdjacency(QMap<qlonglong, osg::ref_ptr<Data::Node> >* someNodes, int maxLevels);

    int getNextId() {return idGen++;}

    // singleton start
    Clusterer() {
        colorCounter = 0;
        idGen = 0;
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
