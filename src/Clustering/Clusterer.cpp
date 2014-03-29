
#include "Clustering/Clusterer.h"
#include "Data/Graph.h"
#include "Data/Node.h"
#include "Data/Edge.h"
#include "Data/Type.h"
#include "Data/Cluster.h"
#include "Manager/Manager.h"
#include <math.h>

#include "QOSG/OptionsWindow.h"
#include "QOSG/CheckBoxList.h"

using namespace Data;

using namespace Clustering;
/*
Clusterer::Clusterer() {
    graph = NULL;
}
*/
void Clusterer::cluster(Graph* graph, QProgressDialog* clusteringProgressBar) {
    this->progressBar = clusteringProgressBar;
    if (graph == NULL) {
        qDebug() << "[Clustering::Clusterer::cluster] Nothing to cluster! Graph is null. ";
        return;
    }
    this->graph = graph;
    clusters.clear();

    qDebug() << "***** clustering algorithm BEGIN";

    switch(algorithmType)
    {
    case AlgorithmType::ADJACENCY:
        clusterAdjacency(graph->getNodes(), clusteringDepth);
        break;
    case AlgorithmType::LEAF:
        clusterLeafs(graph->getNodes(), clusteringDepth);
        break;
    case AlgorithmType::NEIGHBOUR:
        clusterNeighbours(graph->getNodes(), clusteringDepth);
        break;
    default:
        qDebug() << "Clusterer:cluster : not suported algorithmType " << algorithmType;
        break;
    }

    qDebug() << "***** clustering algorithm END";
    qDebug() << "number of new Clusters: " << clusters.size();

    qDebug() << "***** DEBUG VNORENE CLUSTRE BEGIN";
    QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator i;

    for (i = clusters.begin(); i != clusters.end(); i++)
    {
        osg::ref_ptr<Data::Node> node = i.value();
        Data::Cluster* cluster = dynamic_cast<Data::Cluster*>(node.get());

        qDebug() << "cluster #" << cluster->getId() << " has nodes inside: " << cluster->getClusteredNodesCount();

        QSet<Node*> clusteredNodes = cluster->getClusteredNodes();
        foreach (Node *clusteredNode, clusteredNodes) {

            Cluster* c = dynamic_cast<Cluster*>(clusteredNode);
            if(c != 0) {
               qDebug() << "-- cluster #" << c->getId() << " has nodes inside: " << c->getClusteredNodesCount();

            } else {
//                qDebug() << "-- node #" << clusteredNode->getId();
            }
        }
    }


    qDebug() << "***** DEBUG VNORENE CLUSTRE END";

}

osg::Vec4 getNewColor(int colorCounter) {

    osg::Vec4 colors [21] = {osg::Vec4(1.0f, 0.0f, 0.0f, 0.5f),
                           osg::Vec4(0.0f, 1.0f, 0.0f, 0.5f),
                           osg::Vec4(0.0f, 0.0f, 1.0f, 0.5f),
                           osg::Vec4(1.0f, 1.0f, 0.0f, 0.5f),
                           osg::Vec4(1.0f, 0.0f, 1.0f, 0.5f),
                           osg::Vec4(0.0f, 1.0f, 1.0f, 0.5f),
                           osg::Vec4(0.5f, 0.5f, 0.5f, 0.5f),
                           osg::Vec4(0.7f, 0.3f, 0.2f, 0.5f),
                           osg::Vec4(0.1f, 0.6f, 0.4f, 0.5f),
                           osg::Vec4(0.2f, 0.7f, 0.4f, 0.5f),
                           osg::Vec4(0.6f, 0.8f, 0.0f, 0.5f),
                           osg::Vec4(0.1f, 0.2f, 0.3f, 0.5f),
                           osg::Vec4(0.2f, 0.3f, 0.4f, 0.5f),
                           osg::Vec4(0.4f, 0.5f, 0.6f, 0.5f),
                           osg::Vec4(0.7f, 0.8f, 0.9f, 0.5f),
                           osg::Vec4(0.5f, 0.5f, 0.1f, 0.5f),
                           osg::Vec4(0.9f, 0.1f, 0.5f, 0.5f),
                           osg::Vec4(0.2f, 0.7f, 0.3f, 0.5f),
                           osg::Vec4(0.5f, 0.2f, 1.0f, 0.5f),
                           osg::Vec4(0.7f, 0.7f, 0.3f, 0.5f),
                           osg::Vec4(0.3f, 0.4f, 0.5f, 0.5f)};

    return colors[colorCounter % 21];
}

void Clusterer::clusterNeighbours(QMap<qlonglong, osg::ref_ptr<Data::Node> > *someNodes, int maxLevels) {

    progressBar->reset();
    progressBar->setLabelText(QString("Initializing. Depth = %1").arg(clusteringDepth - maxLevels));
    progressBar->setMaximum(someNodes->size());
    int step = 0;

    QMap<qlonglong, osg::ref_ptr<Data::Node> > newClusters;

    Manager::GraphManager * manager = Manager::GraphManager::getInstance();
    QMap<qlonglong, Data::Type*> * types = manager->getActiveGraph()->getTypes();
    Data::Type * type = types->value(1);

    progressBar->setLabelText(QString("Clustering the graph. Depth = %1").arg(clusteringDepth - maxLevels));

    QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator i;
    for (i = someNodes->begin(); i != someNodes->end(); i++)
    {
        progressBar->setValue(step++);
        osg::ref_ptr<Data::Node> node = i.value();
        if (node->getCluster() == NULL) {
            Cluster* cluster = NULL;
            QSet<Node*> incidentNodes = node->getIncidentNodes();
            foreach (Node *incidentNode, incidentNodes) {
                if (!newClusters.contains(incidentNode->getId()) && incidentNode->getCluster() == NULL) {
                    if (cluster == NULL) {
                        cluster = new Cluster(getNextId(), "name", type, graph->getNodeScale(), graph, osg::Vec3f(0,0,0));
                        clusters.insert(cluster->getId(), cluster);
                        newClusters.insert(cluster->getId(), cluster);

                        cluster->setColor(getNewColor(colorCounter));
                        colorCounter++;
                    }
                    incidentNode->setCluster(cluster);
                    incidentNode->setColor(cluster->getColor());
                    cluster->addNodeToCluster(incidentNode);
                }
            }
            if (cluster != NULL) {
                node->setCluster(cluster);
                // POZNAMKA ... vnoreny cluster dostava farbu ako jeho rodic cluster
                // ak ma mat svoju povodnu farbu (kt. dostal v predoslej rekurzii), treba kontrolovat typ nodu
                // a farbu nastavovat len ak node je typu Node a nie Cluster
                node->setColor(cluster->getColor());
                cluster->addNodeToCluster(node);
            }
        }
    }

    progressBar->setValue(someNodes->size());

    if (newClusters.size() > 1 && maxLevels != 0) {
        QMap<qlonglong, osg::ref_ptr<Data::Node> > newNodes(newClusters);
        newClusters.clear();
        clusterNeighbours(&newNodes, maxLevels - 1);
    }

}

void Clusterer::clusterLeafs(QMap<qlonglong, osg::ref_ptr<Data::Node> >* someNodes, int maxLevels) {

    progressBar->reset();
    progressBar->setLabelText(QString("Initializing. Depth = %1").arg(clusteringDepth - maxLevels));
    progressBar->setMaximum(someNodes->size());
    int step = 0;

    qDebug() << "*+* clusterLeafs level #" << maxLevels;
    QMap<qlonglong, osg::ref_ptr<Data::Node> > newClusters;

    Manager::GraphManager * manager = Manager::GraphManager::getInstance();
    QMap<qlonglong, Data::Type*> * types = manager->getActiveGraph()->getTypes();
    Data::Type * type = types->value(1);

    progressBar->setLabelText(QString("Clustering the graph. Depth = %1").arg(clusteringDepth - maxLevels));

    QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator i;
    for (i = someNodes->begin(); i != someNodes->end(); i++)
    {
        progressBar->setValue(step++);
        osg::ref_ptr<Data::Node> node = i.value();
        if (node->getCluster() == NULL) {
            QSet<Node*> incidentNodes = node->getIncidentNodes();

            // je to list
            if (incidentNodes.size() == 1) {
                Node* parent = *(incidentNodes.constBegin());

                osg::ref_ptr<Data::Node> c = clusters.value(parent->getId());
                Data::Cluster* cluster = dynamic_cast<Data::Cluster*>(c.get());

                //Cluster* cluster = dynamic_cast<Cluster*>(clusters.value(v->getId()).get);
                // pridaj rodica do clustru (ak uz nie je v clustri - tzn. spracuvame dalsi list toho rodica)
                if (cluster == NULL) {
                    cluster = new Cluster(parent->getId(), "name", type, graph->getNodeScale(), graph, osg::Vec3f(0,0,0));
                    clusters.insert(cluster->getId(), cluster);
                    newClusters.insert(cluster->getId(), cluster);

                    cluster->setColor(getNewColor(colorCounter));
                    colorCounter++;

                    parent->setCluster(cluster);
                    parent->setColor(cluster->getColor());
                    cluster->addNodeToCluster(parent);
                }
                // pridaj seba (list) do clustru
                node->setCluster(cluster);
                node->setColor(cluster->getColor());
                cluster->addNodeToCluster(node);
            }

            // ignoruj uzol lebo nie je list
        }
    }

    progressBar->setValue(someNodes->size());

    if (newClusters.size() > 1 && maxLevels != 0) {
        QMap<qlonglong, osg::ref_ptr<Data::Node> > newNodes(newClusters);
        newClusters.clear();
        clusterLeafs(&newNodes, maxLevels - 1);
    }

}

void Clusterer::clusterAdjacency(QMap<qlonglong, osg::ref_ptr<Data::Node> >* someNodes, int maxLevels) {

    progressBar->reset();
    progressBar->setLabelText(QString("Initializing. Depth = %1").arg(clusteringDepth - maxLevels));
    progressBar->setMaximum(someNodes->size() * 2);
    int step = 0;

    QMap<qlonglong, osg::ref_ptr<Data::Node> > newClusters;

    Manager::GraphManager * manager = Manager::GraphManager::getInstance();
    QMap<qlonglong, Data::Type*> * types = manager->getActiveGraph()->getTypes();
    Data::Type * type = types->value(1);

    int n = someNodes->size();
    std::vector<bool> p(7);
    std::vector<std::vector<bool> > matrix(n, std::vector<bool>(n, false));
    std::vector<std::vector<unsigned char> > w(n, std::vector<unsigned char>(n, 0));
    // we don't use float, floats are multiplied by K and stored as unsigned char;
    unsigned char K = 100;

    int i = 0, j = 0;
    QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator iterator;
    QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator iterator2;

    // prepare adjacency matrix
    for (iterator = someNodes->begin(); iterator != someNodes->end(); ++iterator, i++)
    {
        osg::ref_ptr<Data::Node> node = iterator.value();
        matrix[i][i] = true;
        QSet<Node*> neighbours = node->getIncidentNodes();
        j = i+1;
        for (iterator2 = iterator + 1; iterator2 != someNodes->end(); ++iterator2, j++) {
            osg::ref_ptr<Data::Node> v = iterator2.value();
            if (neighbours.contains(v.get())) {
                matrix[i][j] = true;
                matrix[j][i] = true;
            } else {
                matrix[i][j] = false;
                matrix[j][i] = false;
            }
        }
    }

    i = 0;
    float maxW = -1;

    progressBar->setLabelText(QString("Preparing weight matrix using Pearson correlation. Depth = %1").arg(clusteringDepth - maxLevels));
    for (iterator = someNodes->begin(); iterator != someNodes->end(); ++iterator, i++) {
        progressBar->setValue(step++);
        if (progressBar->wasCanceled()) return;
        osg::ref_ptr<Data::Node> nodeU = iterator.value();
        w[i][i] = 0;
        int degU = nodeU->getIncidentNodes().size();
        j = i+1;
        for (iterator2 = iterator + 1; iterator2 != someNodes->end(); ++iterator2, j++) {
            osg::ref_ptr<Data::Node> nodeV = iterator2.value();
            int degV = nodeV->getIncidentNodes().size();

            float sum = 0;
            for (int k = 0; k < n; k++) {
                sum += matrix[i][k] && matrix[j][k] ? 1 : 0;
            }
            // apply Pearson
            float wij = ((float)((n * sum) - (degU * degV))) /
                    sqrt(degU * degV * (n - degU) * (n - degV));
            // ignore negative values
            w[j][i] = w[i][j] = qMax(0.0f, wij * K); // K is used to store 0-1 floats in uchar matrix
            if (w[j][i] > maxW) // remember largest weight
                maxW = w[j][i];
        }
    }

    float t = qMin(1.0f * K, maxW); // set correlation threashold for clustering

    // start clustering
    // prepare threshold
    t *= 0.9f;
    i = 0;

//    int tempID = 0;

    progressBar->setLabelText(QString("Clustering the graph. Depth = %1").arg(clusteringDepth - maxLevels));

    // set of clusters
    QSet<qlonglong> clustered;
    for (iterator = someNodes->begin(); iterator != someNodes->end(); ++iterator, i++) {
        progressBar->setValue(step++);
        if (progressBar->wasCanceled()) return;
        osg::ref_ptr<Data::Node> u = iterator.value();
        j = i+1;
        Cluster* c = u->getCluster();
        // set of nodes about to cluster
        QSet<Data::Node *> toCluster;
        for (iterator2 = iterator + 1; iterator2 != someNodes->end(); ++iterator2, j++) {
            osg::ref_ptr<Data::Node> v = iterator2.value();
            if (w[i][j] >= t) {
                //											qDebug() << "v: " << v->getId() << " w=" << w[i][j];
                if (c == NULL) {
                    c = v->getCluster();
                    //								qDebug() << "c = v->getParent()";
                    //								qDebug() << "c: " << (c == NULL ? "NULL" : QString("%1").arg(c->getId()));
                } else {
                    if (v->getCluster() != NULL) {
                        //								qDebug() << "v has other parent!";
                        continue;
                    }
                }
                toCluster.insert(v.get());
                clustered.insert(v->getId());

                int link = -1;
                for (int k = 0; k < n; k++) {
                    if(matrix[i][k] && matrix[j][k]) {
                        if (link < 0 && link != i && link != j) {
                            link = k;
                        } else if (link >= 0) {
                            link = -1;
                            break;
                        }
                    }
                }
                if (link >= 0) {
                    //							qDebug() << "link = " << link;

                    osg::ref_ptr<Data::Node> x = someNodes->value(someNodes->keys().at(link));
                    if (!clustered.contains(x->getId())) {
                        //								qDebug() << "x: " << x->getId();
                        if (c = NULL) {
                            c = x->getCluster();
                        } else if (x->getCluster() != NULL) {
                            continue;
                        }
                        toCluster.insert(x.get());
                        clustered.insert(x->getId());
                    }
                }

                //											qDebug() << "is clusterable";
            }
        }
        if (!toCluster.isEmpty()) {
            if (c == NULL) {
                //c = addCluster();
                c = new Cluster(getNextId(), "name", type, graph->getNodeScale(), graph, osg::Vec3f(0,0,0));
                clusters.insert(c->getId(), c);
                newClusters.insert(c->getId(), c);

                c->setColor(getNewColor(colorCounter));
                colorCounter++;
                //											qDebug() << "new c: " << c->getId();
            }
            foreach (Node *v, toCluster) {
                //											qDebug() << "v': " << v->getId();
                if (v->getCluster() == NULL) {
                    v->setCluster(c);
                    v->setColor(c->getColor());
                    c->addNodeToCluster(v);
                    //											qDebug() << "v' added to c";
                }
            }
            if (u->getCluster() == NULL) {
                u->setCluster(c);
                u->setColor(c->getColor());
                c->addNodeToCluster(u);
                //											qDebug() << "u added to c";
                clustered.insert(u->getId());
            }
        }
    }

    QMap<qlonglong, osg::ref_ptr<Data::Node> > newNodesCopy(*someNodes);

    for (QSet<qlonglong>::const_iterator i = clustered.constBegin(); i != clustered.constEnd(); ++i) {
        newNodesCopy.remove(*i);
    }

    newNodesCopy.unite(newClusters);
    newClusters.clear();

    progressBar->setValue(someNodes->size() * 2);

    if (newNodesCopy.size() > 2 && maxLevels != 0) {
        clusterAdjacency(&newNodesCopy, maxLevels - 1);
    }

}

int Clusterer::getMaxCountOfNodesInClusters() {
    int max = 0;
    QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator i;
    for (i = clusters.begin(); i != clusters.end(); i++)
    {
        osg::ref_ptr<Data::Node> node = i.value();
        // TODO pripadne prerobit vrece "clusters" nech uchovava len typ Cluster {aj tak v nom nie su Nody}
        Data::Cluster* cluster = dynamic_cast<Data::Cluster*>(node.get());

        if (cluster->getClusteredNodesCount() > max) {
            max = cluster->getClusteredNodesCount();
        }
    }
    return max;
}
