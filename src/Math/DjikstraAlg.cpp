#include "Math/DjikstraAlg.h"
#include <limits>

// source: http://www.vogella.de/articles/JavaAlgorithmsDijkstra/article.html
// altered to compute all shortest paths

DjikstraAlg::DjikstraAlg(Data::Graph * graph)
{
	this->nodes = graph->getNodes();
	this->edges = graph->getEdges();
}

QMap<qlonglong, qlonglong> DjikstraAlg::execute(Data::Node * source)
{
	distances.insert(source->getId(), 0);
	unSettledNodes.insert(source->getId());

	while (unSettledNodes.size() > 0)
	{
		Data::Node * node = getMinimum(unSettledNodes);
		settledNodes.insert(node->getId());
		unSettledNodes.remove(node->getId());
		findMinimalDistances(node);
	}

	return distances;
}

void DjikstraAlg::findMinimalDistances(Data::Node * node) 
{
	QVector<osg::ref_ptr<Data::Node> > adjacentNodes = getNeighbors(node);
		
	for (int x = 0; x < adjacentNodes.size(); x++) 
	{
		Data::Node * target = adjacentNodes.at(x);

		int tSh = getShortestDistance(target);
		int nSh = getShortestDistance(node);
		int d = getDistance(node, target);

		if ( tSh > nSh + d) 
		{
			distances.insert(target->getId(), getShortestDistance(node) + getDistance(node, target));
			predecessors.insert(target->getId(), node->getId());
			unSettledNodes.insert(target->getId());
		}
		else if (tSh == nSh + d)
		{
			predecessors.insertMulti(target->getId(), node->getId());
			unSettledNodes.insert(target->getId());
		}
	}
}

int DjikstraAlg::getDistance(Data::Node * node, Data::Node * target) 
{
	QMapIterator<qlonglong, osg::ref_ptr<Data::Edge> > i(*edges);
	 
	while (i.hasNext()) 
	{
		i.next();
		Data::Edge * edge = i.value();

		if ((edge->getSrcNode()->getId() == node->getId()) && (edge->getDstNode()->getId() == target->getId())) 
		{
				return edge->getWeight();
		}
	}

	cout << "Error, no data!";

	return 0;
}

QVector<osg::ref_ptr<Data::Node> > DjikstraAlg::getNeighbors(Data::Node * node) 
{
	QVector<osg::ref_ptr<Data::Node> > neighbors;

	QMapIterator<qlonglong, osg::ref_ptr<Data::Edge> > i(*edges);

	while (i.hasNext()) 
	{
		i.next();
		Data::Edge * edge = i.value();

		if (edge->getSrcNode()->getId() == node->getId() && !isSettled(edge->getDstNode())) 
		{
			neighbors.push_back(edge->getDstNode());
		}
	}
	return neighbors;
}

Data::Node * DjikstraAlg::getMinimum(QSet<qlonglong> vertexes) 
{
	Data::Node * minimum = NULL;
	
	QSetIterator<qlonglong> i(vertexes);

	while(i.hasNext())
	{
		qlonglong nodeId = i.next();

		if (minimum == NULL) 
		{
			minimum = nodes->value(nodeId);
		} 
		else 
		{
			if (getShortestDistance(nodes->value(nodeId)) < getShortestDistance(minimum)) 
			{
				minimum = nodes->value(nodeId);
			}
		}
	}

	return minimum;
}

bool DjikstraAlg::isSettled(Data::Node * vertex) 
{
	return settledNodes.contains(vertex->getId());
}

int DjikstraAlg::getShortestDistance(Data::Node * destination) 
{
	qlonglong d = -1;

	if (distances.contains(destination->getId()))		
		d = distances.value(destination->getId());
	
	if (d == -1) {
		return 999999;
	} else {
		return d;
	}
}

void DjikstraAlg::getPath(Data::Node * target, QLinkedList<osg::ref_ptr<Data::Node> > * path) 
{
	Data::Node * step = target;
	
	// Check if a path exists
	if (! predecessors.contains(step->getId()) && path == NULL) 
	{
		return;
	}

	if (path == NULL)
		path = new QLinkedList<osg::ref_ptr<Data::Node> >();
	
	path->push_front(step);
	
	while (predecessors.contains(step->getId())) 
	{
		QList<qlonglong> values = predecessors.values(step->getId());

		if (values.size() > 1)
		{
			for (int x = 0; x < values.size(); x++ )
			{
				getPath(nodes->value(values.at(x)), new QLinkedList<osg::ref_ptr<Data::Node> >(*path));
			}

			break;
		}
		else
		{
			step = nodes->value(predecessors.value(step->getId()));
			path->push_front(step);
		}
	}

	if (! predecessors.contains(step->getId()))
	{
		allPaths.push_back(*path);

		QLinkedList<osg::ref_ptr<Data::Node> >::iterator i;
	}
}

QVector<QLinkedList<osg::ref_ptr<Data::Node> > > DjikstraAlg::getAllPaths(Data::Node * target)
{
	allPaths.clear();
	getPath(target);
	return allPaths;
}

DjikstraAlg::~DjikstraAlg(void)
{
}
