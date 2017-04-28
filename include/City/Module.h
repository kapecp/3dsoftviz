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
	//osg::ref_ptr<Data::Node> moduleNode;

	QList<osg::ref_ptr<Data::Node>> functionNodes;
	osg::ref_ptr<osg::PositionAttitudeTransform> functionsPAT;

	QList<osg::ref_ptr<Data::Node>> variableNodes;
	osg::ref_ptr<osg::PositionAttitudeTransform> variablesPAT;

	QList<osg::ref_ptr<Data::Node>> interfaceNodes;
	osg::ref_ptr<osg::PositionAttitudeTransform> interfacesPAT;

	QMap<osg::ref_ptr<Data::Node>, osg::ref_ptr<Data::Node>> otherNodes;

public:
	Module();	
	void addFunctionNode( osg::ref_ptr<Data::Node> functionNode );
	void addVariableNode( osg::ref_ptr<Data::Node> variableNode );
	void addInterfaceNode( osg::ref_ptr<Data::Node> interfaceNode );
	void addOtherNode( osg::ref_ptr<Data::Node> parentNode, osg::ref_ptr<Data::Node> otherNode );	
	osg::ref_ptr<osg::PositionAttitudeTransform> getFunctionBuildingPAT() { return functionsPAT; }
	osg::ref_ptr<osg::PositionAttitudeTransform> getVariableBuildingPAT() { return variablesPAT; }
	osg::ref_ptr<osg::PositionAttitudeTransform> getInterfaceBuildingPAT() { return interfacesPAT; }

	osg::ref_ptr<osg::PositionAttitudeTransform> getNodeParentPAT( osg::ref_ptr<Data::Node> node );

	void addBuilding( osg::ref_ptr<City::Building> building );

	//void setModuleNode( osg::ref_ptr<Data::Node> moduleNode ) { this->moduleNode = moduleNode; }
	void showLabels( bool state );
	void selectAll( bool state );
	void refresh();
	void updateNodesPosition();

};
}
