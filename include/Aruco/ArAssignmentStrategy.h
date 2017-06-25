#ifndef ARASSIGNMENTSTRATEGY_H
#define ARASSIGNMENTSTRATEGY_H

#include <osg/Vec3f>
#include "Data/Node.h"
#include "Core/Core.h"

namespace Data {
class Graph;
}

namespace ArucoModul {


class ArAssignmentStrategy: public QObject
{
	Q_OBJECT

public:
	ArAssignmentStrategy();
	osg::ref_ptr<Data::Node> assign( osg::Vec3f position );
private:
	virtual osg::ref_ptr<Data::Node> pickNodeToAssign( osg::Vec3f position ) = 0;
};




class ArAssignmentStrategyPosition: public ArAssignmentStrategy
{
public:
	ArAssignmentStrategyPosition(): ArAssignmentStrategy() {}
private:
	osg::ref_ptr<Data::Node> pickNodeToAssign( osg::Vec3f position );
	bool chckIfNearPosition( osg::Vec3f source, osg::Vec3f target );
};





class ArAssignmentStrategyEdgeCount: public ArAssignmentStrategy
{
public:
	ArAssignmentStrategyEdgeCount(): ArAssignmentStrategy() {}
private:
	osg::ref_ptr<Data::Node> pickNodeToAssign( osg::Vec3f position );
};





class ArAssignmentStrategyNearest: public ArAssignmentStrategy
{
public:
	ArAssignmentStrategyNearest(): ArAssignmentStrategy() {}
private:
	osg::ref_ptr<Data::Node> pickNodeToAssign( osg::Vec3f position );
};

}
#endif // ARASSIGNMENTSTRATEGY_H
