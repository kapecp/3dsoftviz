#ifndef NODECLUSTER_H_
#define NODECLUSTER_H_

#include "Data/Node.h"
#include <QSet>

#include "Clustering/Figures/Cube.h"
#include "Clustering/Figures/Sphere.h"
#include "Layout/ShapeGetter_Cube.h"

namespace Data {
class Graph;

class Cluster : public Node
{
public:
	Cluster( qlonglong id, QString name, Data::Type* type, float scaling, Data::Graph* graph, osg::Vec3f position );

	qlonglong getId() const
	{
		return id;
	}

//	void setCluster(Cluster* cluster);
	QSet<Node*> getIncidentNodes() const;
	void addNodeToCluster( Node* node )
	{
		clusteredNodes.insert( node );
	}
	// vracia nody a clustre kt. su vo vnutri tohto clustru
	QSet<Node*> getClusteredNodes() const
	{
		return clusteredNodes;
	}
	// vracia nody kt. su v tomto clustri spolu s nodmi ktore su v clastroch v tomto clustri - rekurzivne
	QSet<Node*> getALLClusteredNodes() const;
	int getClusteredNodesCount() const
	{
		return clusteredNodes.size();
	}
	osg::Vec3 getMidpoint();
	void setSelected( bool selected, QLinkedList<osg::ref_ptr<Data::Node> >* pickedNodes );
	Node* getRandomNode();

	void setCube( Clustering::Cube* cube )
	{
		this->cube = cube;
	}
	Clustering::Cube* getCube()
	{
		return cube;
	}

	void setSphere( Clustering::Sphere* sphere )
	{
		this->sphere = sphere;
	}
	Clustering::Sphere* getSphere()
	{
		return sphere;
	}

	void setRepulsiveForceInside( double repulsiveForceInside );
	double getRepulsiveForceInside()
	{
		return repulsiveForceInside;
	}

	// zaregistruj obmedzovac na tento cluster
	void registerShapeGetter( Layout::ShapeGetter_Cube* shapeGetter )
	{
		this->shapeGetter = shapeGetter;
	}
	Layout::ShapeGetter_Cube* getShapeGetter()
	{
		return shapeGetter;
	}

private:

	qlonglong id;
	QSet<Node*> clusteredNodes;
	osg::Vec4 originalColor;

	double repulsiveForceInside;

	Clustering::Cube* cube;
	Clustering::Sphere* sphere;

	Layout::ShapeGetter_Cube* shapeGetter;

};

}

#endif  /*NODECLUSTER_H_*/
