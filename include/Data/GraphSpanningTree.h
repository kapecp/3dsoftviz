#ifndef GRAPHSPANNINGTREE_H
#define GRAPHSPANNINGTREE_H
//-----------------------------------------------------------------------------
#include "QMap"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------


namespace Data {


class GraphSpanningTree {
public:

	/**
	*  \fn public  addGroup(QList<qlonglong> nodes, int depth)
	*  \brief Create new group of graph siblings
	*  \param       nodes   IDs of sibling nodes
	*  \param       depth   graph depth of siblings
	*   \param      parentId ID of parent node of siblings. If is set, then is used as groupId.
	*/

	void addGroup(QList<qlonglong> nodes, int depth, qlonglong parentId=-1);

	/**
	*  \fn public  getMaxDepth()
	*  \brief Find max depth of graph spanning tree
	*  \return  max depth in groupToDepth
	*/
	int getMaxDepth();

	/**
	*  \fn public   getAllGroups()
	*  \brief Get all IDs of groups in graph
	*  \return  list of group IDs
	*/
	QList<qlonglong> getAllGroups();

	/**
	*  \fn public  getGroupsInDepth(int depth)
	*  \brief Get all IDs of groups in graph depth
	*   \param       depth   graph depth
	*  \return  list of group IDs
	*/
	QList<qlonglong> getGroupsInDepth(int depth);

	/**
	*  \fn public  getNodesInGroup(qlonglong groupId)
	*  \brief Get all IDs of nodes in group of siblings
	*   \param       groupId   id of group of siblings
	*  \return  list of node IDs
	*/
	QList<qlonglong> getNodesInGroup(qlonglong groupId);

	/**
	*  \fn public  getRandomNodeInGroup(qlonglong groupId)
	*  \brief Get ID of some nodes in group of siblings
	*   \param       groupId   id of group of siblings
	*  \return  node ID
	*/
	qlonglong getRandomNodeInGroup(qlonglong groupId);

private:
	/**
	*  QMap<qlonglong,qlonglong> nodeToGroup
	*  \brief QMap<nodeId, groupId> join id of node and id of group of graph siblings
	*/
	QMap<qlonglong, qlonglong> nodeToGroup;

	/**
	*  QMap<qlonglong,qlonglong> groupToDepth
	*  \brief QMap<groupId, depth> join id of group of siblings and its depth in graph
	*/
	QMap<qlonglong, int> groupToDepth;

	/**
	*  \fn private  getNextGroupId()
	*  \brief Return id of next siblings group in graph
	*  \return qlonglong incremented value of max grouptToDepth.key() or value 1
	*/
	qlonglong getNextGroupId();

};

}
#endif
