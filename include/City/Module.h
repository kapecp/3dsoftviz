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

	/**
		 * @author Denis Illes
		 * @fn private/public
		 * @brief move nodes from module node back to default place
		 * @param
		 * @return
		 */

	/**
		 * osg::ref_ptr moduleNode
		 * @brief pointer to the parent node, type: Module
		 */
	osg::ref_ptr<Data::Node> moduleNode;

	/**
		 * QList variableNodes
		 * @brief list of pointers to Data::Node, type: Local variable / Global variable
		 */
	QList<osg::ref_ptr<Data::Node>> variableNodes;

	/**
		 * osg::ref_ptr variablesPAT
		 * @brief pointer to PAT for every variable node
		 */
	osg::ref_ptr<osg::PositionAttitudeTransform> variablesPAT;

	/**
		 * QList functionNodes
		 * @brief list of pointers to Data::Node, type: Function / Global function
		 */
	QList<osg::ref_ptr<Data::Node>> functionNodes;

	/**
		 * osg::ref_ptr functionsPAT
		 * @brief pointer to PAT for every function node
		 */
	osg::ref_ptr<osg::PositionAttitudeTransform> functionsPAT;

	/**
		 * QList interfaceNodes
		 * @brief list of pointers to Data::Node, type: Interface
		 */
	QList<osg::ref_ptr<Data::Node>> interfaceNodes;

	/**
		 * osg::ref_ptr interfacesPAT
		 * @brief pointer to PAT for every interface node
		 */
	osg::ref_ptr<osg::PositionAttitudeTransform> interfacesPAT;

	/**
		 * Qmap otherNodes
		 * @brief map of pointers to Data::Node, type: undefined
		 */
	QMap<osg::ref_ptr<Data::Node>, osg::ref_ptr<Data::Node>> otherNodes;

public:

	Module();

	/**
		 * @fn public addVariableNode
		 * @brief add passed node to Qlist of nodes
		 * @param osg::ref_ptr	variableNode
		 */
	void addVariableNode( osg::ref_ptr<Data::Node> variableNode );

	/**
		 * @fn public addFunctionNode
		 * @brief add passed node to Qlist of nodes
		 * @param osg::ref_ptr	functionNode
		 */
	void addFunctionNode( osg::ref_ptr<Data::Node> functionNode );

	/**
		 * @fn public addInterfaceNode
		 * @brief add passed node to Qlist of nodes
		 * @param osg::ref_ptr	interfaceNode
		 */
	void addInterfaceNode( osg::ref_ptr<Data::Node> interfaceNode );

	/**
		 * @fn public addOtherNode
		 * @brief add passed nodes to QMap of nodes
		 * @param osg::ref_ptr	parentNode
		 * @param osg::ref_ptr	otherNode
		 */
	void addOtherNode( osg::ref_ptr<Data::Node> parentNode, osg::ref_ptr<Data::Node> otherNode );

	/**
		 * @fn public getVariableNodes
		 * @brief return list of variable nodes
		 * @return QList	variableNodes object
		 */
	QList<osg::ref_ptr<Data::Node>> getVariableNodes()
	{
		return variableNodes;
	}

	/**
		 * @fn public getFunctionNodes
		 * @brief return list of function nodes
		 * @return QList	functionNodes object
		 */
	QList<osg::ref_ptr<Data::Node>> getFunctionNodes()
	{
		return functionNodes;
	}

	/**
		 * @fn public getInterfaceNodes
		 * @brief return list of interface nodes
		 * @return QList	interfaceNodes object
		 */
	QList<osg::ref_ptr<Data::Node>> getInterfaceNodes()
	{
		return interfaceNodes;
	}

	/**
		 * @fn public getOtherNodes
		 * @brief return map of other nodes
		 * @return QMap	otherNodes object
		 */
	QMap<osg::ref_ptr<Data::Node>, osg::ref_ptr<Data::Node>> getOtherNodes()
	{
		return otherNodes;
	}

	/**
		 * @fn public getVariableBuildingPAT
		 * @brief return pointer to PAT of every variable node
		 * @return osg::ref_ptr	variablesPAT
		 */
	osg::ref_ptr<osg::PositionAttitudeTransform> getVariableBuildingPAT()
	{
		return variablesPAT;
	}

	/**
		 * @fn public getFunctionBuildingPAT
		 * @brief return pointer to PAT of every function node
		 * @return osg::ref_ptr	functionsPAT
		 */
	osg::ref_ptr<osg::PositionAttitudeTransform> getFunctionBuildingPAT()
	{
		return functionsPAT;
	}

	/**
		 * @fn public getInterfaceBuildingPAT
		 * @brief return pointer to PAT of every interface node
		 * @return osg::ref_ptr	interfacesPAT
		 */
	osg::ref_ptr<osg::PositionAttitudeTransform> getInterfaceBuildingPAT()
	{
		return interfacesPAT;
	}

	/**
		 * @fn public getNodeParentPAT
		 * @brief return pointer to PAT for the parent of passed node
		 * @param osg::ref_ptr	pointer to Data::Node
		 * @return osg::ref_ptr	pointer to PAT object
		 */
	osg::ref_ptr<osg::PositionAttitudeTransform> getNodeParentPAT( osg::ref_ptr<Data::Node> node );

	/**
		 * @fn public setModuleNode
		 * @brief set moduleNode to passed node
		 * @param osg::ref_ptr	pointer to Data::Node
		 */
	void setModuleNode( osg::ref_ptr<Data::Node> moduleNode )
	{
		this->moduleNode = moduleNode;
	}

	/**
		 * @fn public getModuleNode
		 * @brief return pointer to module node
		 * @param osg::ref_ptr	pointer to Data::Node
		 */
	osg::ref_ptr<Data::Node> getModuleNode()
	{
		return moduleNode;
	}

	/**
		 * @fn public showLabels
		 * @brief set objects visibility to passed value
		 * @param bool	boolean value state
		 */
	void showLabels( bool state );

	/**
		 * @fn public selectAll
		 * @brief set module objects to be selected or not
		 * @param bool	boolean value state
		 */
	void selectAll( bool state );

	/**
		 * @fn public refresh
		 * @brief calculate positions for every node in the module
		 */
	void refresh();

	/**
		 * @fn public decompose
		 * @brief decompose module (a clean up function)
		 */
	void decompose();

	/**
		 * @fn public updateNodesPosition
		 * @brief position of nodes in module must be manually calculated
		 */
	void updateNodesPosition();

private:

	/**
		 * @fn private forEachList
		 * @brief helper loop function
		 * @param std::function	func
		 */
	void forEachList( std::function<void( osg::ref_ptr<Data::Node> )> func );

	/**
		 * @fn private addBuildingPATPosition
		 * @brief calculate position for building PAT of the passed node
		 * @param osg::ref_ptr	pointer to Data::Node
		 */
	void addBuildingPATPosition( osg::ref_ptr<Data::Node> node );

	/**
		 * @fn private addParentPATPosition
		 * @brief calculate position for parent PAT of the passed node
		 * @param osg::ref_ptr	pointer to Data::Node
		 */
	void addParentPATPosition( osg::ref_ptr<Data::Node> node );

	/**
		 * @fn private addGroupPATPosition
		 * @brief calculate position for group PAT of the passed node
		 * @param osg::ref_ptr	pointer to Data::Node
		 */
	void addGroupPATPosition( osg::ref_ptr<Data::Node> node );

	/**
		 * @fn private addModulePATPosition
		 * @brief calculate position for module PAT of the passed node
		 * @param osg::ref_ptr	pointer to Data::Node
		 */
	void addModulePATPosition( osg::ref_ptr<Data::Node> node );

	/**
		 * @fn private addVariableNodeParentPATPosition
		 * @brief calculate position for variable node parent PAT of the passed node
		 * @param osg::ref_ptr	pointer to Data::Node
		 */
	void addVariableNodeParentPATPosition( osg::ref_ptr<Data::Node> varNode, osg::ref_ptr<Data::Node> otherNode );
};
}
