#include "Data/AbsNode.h"
#include "Data/Graph.h"

#include <QString>

Data::AbsNode::AbsNode( qlonglong id, QString name, Data::Type* type, Data::Graph* graph)
{
    this->id=id;
    this->name=name;
    this->type=type;
    this->graph = graph;
    this->type = type;
}

bool Data::AbsNode::equals( AbsNode* node )
{
    //porovnanie s inym uzlom
    if ( this == node ) {
        return true;
    }
    if ( node == NULL ) {
        return false;
    }
    if ( id != node->getId() ) {
        return false;
    }
    if ( ( graph==NULL && node->getGraph()!=NULL ) || ( graph!=NULL && node->getGraph()==NULL ) ) {
        return false;
    }
    if ( graph==NULL && node->getGraph()==NULL ) {
        return true;
    }
    if ( graph!=NULL && graph->getId() != node->getGraph()->getId() ) {
        return false;
    }
    return true;
}

