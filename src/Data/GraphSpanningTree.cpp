#include "Data/GraphSpanningTree.h"


void Data::GraphSpanningTree::addGroup(QList<qlonglong> nodes, int depth, qlonglong parentId){
	qlonglong groupId;
	if (parentId != -1) groupId=parentId;
	else groupId = getNextGroupId();
	groupToDepth.insert(groupId,depth);
	QList<qlonglong>::iterator itNodeId;
	for(itNodeId=nodes.begin(); itNodeId!=nodes.end(); ++itNodeId){
		nodeToGroup.insert((*itNodeId),groupId);
	}

}

QList<qlonglong> Data::GraphSpanningTree::getAllGroups(){
	return groupToDepth.keys();
}

QList<qlonglong> Data::GraphSpanningTree::getGroupsInDepth(int depth){
	return groupToDepth.keys(depth);
}

QList<qlonglong> Data::GraphSpanningTree::getNodesInGroup(qlonglong groupId){
	return nodeToGroup.keys(groupId);
}

qlonglong Data::GraphSpanningTree::getRandomNodeInGroup(qlonglong groupId){
	return nodeToGroup.key(groupId);
}

qlonglong Data::GraphSpanningTree::getNextGroupId(){
	if (groupToDepth.empty()) return 1;
	else {
		QMap<qlonglong,int>::iterator mapIt;
		qlonglong max =0;
		for(mapIt=groupToDepth.begin();mapIt!=groupToDepth.end(); ++mapIt){
			if (mapIt.key() > max) max = mapIt.key();
		}

		return (max+1);
	}
}

int Data::GraphSpanningTree::getMaxDepth(){
	if (groupToDepth.empty()) return -1;
	QMap<qlonglong,int>::iterator mapIt;
	int max =0;
	for(mapIt=groupToDepth.begin();mapIt!=groupToDepth.end(); ++mapIt){
		if (mapIt.value() > max) max = mapIt.value();
	}
	return max;
}
