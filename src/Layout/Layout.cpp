#include "Layout/Layout.h"

#include <osg/ref_ptr>

using namespace Layout;
using namespace Vwr;

LayoutGenerator::LayoutGenerator(QMap<qlonglong, osg::ref_ptr<Data::Node> > *in_nodes,QMap<qlonglong, osg::ref_ptr<Data::Edge> > *in_edges,QMap<qlonglong, Data::Type*> *in_types)
{
	qnodes = in_nodes;
	qedges = in_edges;
	qtypes = in_types;
}
Layout::LayoutGenerator::~LayoutGenerator()
{
	igraph_destroy(&graph);
	igraph_matrix_destroy(&nodes_layout);
}
/*
int LayoutGenerator::InitTestCubeFRLayout()
{
    //DataHelper::generateCube2(nodes,edges,types);
	igraph_matrix_init(&nodes_layout, 0, 0);
	createIgraph(&graph);
	//printDataState();
	step_index = 0;
	getFruchtermanReingoldLayout(false);
	//printDataState();

	return 0;	
}
*/
int LayoutGenerator::InitTestCylinderFRLayout(int nodesInBottom, int height)
{
        DataHelper::generateCylinder(qnodes,qedges,qtypes, nodesInBottom, height);
        igraph_matrix_init(&nodes_layout, 0, 0);
        createIgraph(&graph);
        //printDataState();
		step_index = 0;
        getFruchtermanReingoldLayout(false);
        //printDataState();

        return 0;
}
/*
int LayoutGenerator::InitTestFRLayout()
{
        //DataHelper::generateTestingGraph(nodes,edges,types);
        igraph_matrix_init(&nodes_layout, 0, 0);
        createIgraph(&graph);
        //printDataState();
                step_index = 0;
        getFruchtermanReingoldLayout(false);
        //printDataState();

        return 0;
}
*/
int Layout::LayoutGenerator::InitFRLayout()
{
	igraph_matrix_init(&nodes_layout, 0, 0);
	createIgraph(&graph);
	step_index = 0;
	getFruchtermanReingoldLayout(false);	

	return 0;	
}
int Layout::LayoutGenerator::getFruchtermanReingoldLayout(bool isInit)
{
	
	// Layout
	double maxdelta, area, coolexp, repulserad;
	igraph_bool_t use_seed;
	if(isInit)
	{
		use_seed = 1;
	}
	else
	{
		use_seed = 0;
	}
	maxdelta = igraph_vcount(&graph);
	area = maxdelta*maxdelta;
	coolexp = COOL_EXP;
	repulserad = area * maxdelta;

	igraph_vector_t *weights = 0;

	igraph_layout_fruchterman_reingold_3d(&graph, &nodes_layout, STEP_COUNT,maxdelta,area,coolexp,repulserad,use_seed,weights);

	float xpos, ypos, zpos;
        QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator j;
	j = qnodes->begin();
	for (int i = 0; i < igraph_matrix_nrow(&nodes_layout); i++,++j)
	{		
		xpos = (float)MATRIX(nodes_layout, i, 0);
		ypos = (float)MATRIX(nodes_layout, i, 1);
		zpos = (float)MATRIX(nodes_layout, i, 2);

		osg::Vec3f* position = new osg::Vec3f(xpos,ypos,zpos);
		j.value()->setTargetPosition(*position);
	}

	return 0;
}

void LayoutGenerator::createIgraph(igraph_t* graph)
{
	
	igraph_empty(graph, 0, true);

	igraph_add_vertices(graph, qnodes->count(), 0);
        QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i;
	for (i = qedges->begin();i != qedges->end();++i)
	{		
		igraph_add_edge(graph, i.value()->getSrcNode()->getId(), i.value()->getDstNode()->getId());
	}
}

void LayoutGenerator::printDataState()
{
	std::cout << "Nodes:\n";
	for(int i = 0;i < qnodes->count();i++)
	{
		
		osg::Vec3f position = qnodes->value(i)->getTargetPosition();
		float xpos = (float)position.x();
		float ypos = (float)position.y();
		float zpos = (float)position.z();
		std::cout << qnodes->value(i)->getId() << " " << qnodes->value(i)->getName().toStdString() << " typeId:" << qnodes->value(i)->getType()->getId() << " x:" << xpos << " y:" << ypos << " z:" << zpos << " edgesId:";
        QMap<qlonglong, osg::ref_ptr<Data::Edge> > * edg = qnodes->value(i)->getEdges();
		/*
		for(int j = 0;j < edg.count();j++)
		{
			std::cout << edg.value(j)->getId() << ", ";
		}
		*/
		std::cout << "\n";
	}

	std::cout << "\n\nEdges:\n";
	for(int i = 0;i < qedges->count();i++)
	{
		std::cout << qedges->value(i)->getId() << " " << qedges->value(i)->getName().toStdString() << " srcId:" << qedges->value(i)->getSrcNode()->getId() << " dstId:" << qedges->value(i)->getDstNode()->getId() << " typeId:" << qedges->value(i)->getType()->getId() << "\n";
	}
	std::cout << "\n\nTypes:\n";
	for(int i = 0;i < qtypes->count();i++)
	{
		std::cout << qtypes->value(i)->getId() << " " << qtypes->value(i)->getName().toStdString() << "\n";
	}
	//std::cout << "\nDatova struktura kocky je vytvorena.\n\n";
}

int Layout::LayoutGenerator::GetNextStep()
{
	if(step_index < STEP_COUNT)
	{
		step_index++;
	
		getFruchtermanReingoldLayout(true);	
	}
	//printDataState();
	return 0;
}
