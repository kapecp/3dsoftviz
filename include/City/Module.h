#pragma once

#include <osg/PositionAttitudeTransform>
#include "Building.h"
#include "Ball.h"
#include <QtCore/QList>
#include <functional>
#include "Data/Node.h"

namespace City {
class Module : public osg::PositionAttitudeTransform
{
private:
	osg::ref_ptr<Data::Node> moduleNode;

	QList<osg::ref_ptr<Data::Node>> interfaceNodes;
	QList<osg::ref_ptr<City::Building>> interfaceBuildings;
	osg::ref_ptr<osg::PositionAttitudeTransform> interfaceBuildingPAT;

	QList<osg::ref_ptr<Data::Node>> functionNodes;
	QList<osg::ref_ptr<City::Building>> functionBuildings;
	osg::ref_ptr<osg::PositionAttitudeTransform> functionBuildingPAT;

	QList<osg::ref_ptr<Data::Node>> variableNodes;
	QList<osg::ref_ptr<City::Building>> variableBuildings;
	osg::ref_ptr<osg::PositionAttitudeTransform> variableBuildingPAT;

	QList<osg::ref_ptr<Data::Node>> otherNodes;
	QList<osg::ref_ptr<City::Building>> otherBuildings;
	osg::ref_ptr<osg::PositionAttitudeTransform> otherBuildingPAT;

public:
	Module();
	void addInterfaceBuilding( osg::ref_ptr<City::Building> interfaceBuilding, osg::ref_ptr<Data::Node> interfaceNode );
	void addFunctionBuilding( osg::ref_ptr<City::Building> functionBuilding, osg::ref_ptr<Data::Node> functionNode );
	void addVariableBuilding( osg::ref_ptr<City::Building> variableBuilding, osg::ref_ptr<Data::Node> variableNode );
	void addOtherBuilding( osg::ref_ptr<City::Building> otherBuilding, osg::ref_ptr<Data::Node> otherNode );
	osg::ref_ptr<osg::PositionAttitudeTransform> getInterfaceBuildingPAT() { return interfaceBuildingPAT; }
	osg::ref_ptr<osg::PositionAttitudeTransform> getFunctionBuildingPAT() { return functionBuildingPAT; }
	osg::ref_ptr<osg::PositionAttitudeTransform> getVariableBuildingPAT() { return variableBuildingPAT; }
	osg::ref_ptr<osg::PositionAttitudeTransform> getOtherBuildingPAT() { return otherBuildingPAT; }

	void setModuleNode( osg::ref_ptr<Data::Node> moduleNode ) { this->moduleNode = moduleNode; }
	void showLabels( bool state );
	void selectAll( bool state );
	void refresh();
	void updateNodesPosition();
};
}
