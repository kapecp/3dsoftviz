#ifndef DBNODE_H
#define DBNODE_H

#include "Data/VizNode.h"

namespace Data {
class Node;
class Graph;
class Type;


class DbNode: public VizNode
{
public:
	DbNode( qlonglong id, QString name, Data::Type* type, Data::Graph* graph, float scaling );
	/**
	    *  \fn inline public  isInDB
	    *  \brief Returns if the Node is already in database or not
	    *  \return bool true, if the Node is in database
	    */
	bool isInDB()
	{
		return inDB;
	}

	/**
	    *  \fn inline public  setIsInDB
	    *  \brief  Sets the inDB flag of the Node to true (meaning that the Node is in database)
	    */
	void setIsInDB()
	{
		inDB = true;
	}

	/**
	    *	bool inDB
	    *	\brief Flag if the Type is in database
	    */
	bool inDB;
};
}

#endif // DBNODE_H
