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

	QList<osg::ref_ptr<Data::Node>> variableNodes;
	osg::ref_ptr<osg::PositionAttitudeTransform> variablesPAT;

	QList<osg::ref_ptr<Data::Node>> functionNodes;
	osg::ref_ptr<osg::PositionAttitudeTransform> functionsPAT;

	QList<osg::ref_ptr<Data::Node>> interfaceNodes;
	osg::ref_ptr<osg::PositionAttitudeTransform> interfacesPAT;

	QMap<osg::ref_ptr<Data::Node>, osg::ref_ptr<Data::Node>> otherNodes;

public:
	Module();	
	void addVariableNode( osg::ref_ptr<Data::Node> variableNode );
	void addFunctionNode( osg::ref_ptr<Data::Node> functionNode );	
	void addInterfaceNode( osg::ref_ptr<Data::Node> interfaceNode );
	void addOtherNode( osg::ref_ptr<Data::Node> parentNode, osg::ref_ptr<Data::Node> otherNode );		

	QList<osg::ref_ptr<Data::Node>> getVariableNodes() { return variableNodes; }
	QList<osg::ref_ptr<Data::Node>> getFunctionNodes() { return functionNodes; }
	QList<osg::ref_ptr<Data::Node>> getInterfaceNodes() { return interfaceNodes; }
	QMap<osg::ref_ptr<Data::Node>, osg::ref_ptr<Data::Node>> getOtherNodes() { return otherNodes; }

	osg::ref_ptr<osg::PositionAttitudeTransform> getVariableBuildingPAT() { return variablesPAT; }
	osg::ref_ptr<osg::PositionAttitudeTransform> getFunctionBuildingPAT() { return functionsPAT; }
	osg::ref_ptr<osg::PositionAttitudeTransform> getInterfaceBuildingPAT() { return interfacesPAT; }

	osg::ref_ptr<osg::PositionAttitudeTransform> getNodeParentPAT( osg::ref_ptr<Data::Node> node );

	void addBuilding( osg::ref_ptr<City::Building> building );

	void setModuleNode( osg::ref_ptr<Data::Node> moduleNode ) { this->moduleNode = moduleNode; }
	osg::ref_ptr<Data::Node> getModuleNode() { return moduleNode; }
	void showLabels( bool state );
	void selectAll( bool state );
	void refresh();
	void decompose();
	void updateNodesPosition();

private:
	void forEachList( std::function<void( osg::ref_ptr<Data::Node> )> func );

};
}
