#ifndef MATRIXCONNECTIONS_H
#define MATRIXCONNECTIONS_H 1

#include <QObject>
#include <QGridLayout>
#include <QString>
#include <QDebug>

#include "Data/Graph.h"
#include "Data/Node.h"

namespace SpecialMatrix {

/**
	*  \class SpacialMatrix
	*  \brief
	*  \author Denis Illes
	*  \date 13. 4. 2016
	*/
class NodeConnections
{

public:

	NodeConnections();

	~NodeConnections();

	/**
	 * @brief getXAxisNodes
	 * @return QList<qlonglong>* list of nodes on x axis
	 */
	QList<qlonglong>* getXAxisNodes()
	{
		return xAxisNodes;
	}

	/**
	 * @brief getYAxisNodes
	 * @return QList<qlonglong>* list of nodes on y axis
	 */
	QList<qlonglong>* getYAxisNodes()
	{
		return yAxisNodes;
	}

	/**
	 * @brief getINodes
	 * @return QList<qlonglong>* list of i nodes
	 */
	QList<qlonglong>* getINodes()
	{
		return iNodes;
	}

	/**
	 * @brief getConnectedNodes
	 * @return QMap<qlonglong, QList<qlonglong>* >* map of all connections in the matrix graph
	 */
	QMap<qlonglong, QList<qlonglong>* >* getConnectedNodes()
	{
		return connectedNodes;
	}

	/**
	 * @brief getNodePositionsArray
	 * @return std::vector<std::vector<qlonglong> > 2D array with info about node positions and ID
	 */
	std::vector<std::vector<qlonglong> > getNodePositionsArray()
	{
		return nodePositionsArray;
	}

	/**
	 * @brief getNodePositionsArrayField gets the ID of node on given position
	 * @param integer row
	 * @param integer column
	 * @return qlonglong ID of given node
	 */
	qlonglong getNodePositionsArrayField( int row, int column )
	{
		return nodePositionsArray[row][column];
	}

	/**
	 * @brief setAvailabilityPosArray resizes availabilityPosArray
	 * @param integer nodesCount
	 */
	void setNodePositionsArray( int nodesCount );

	/**
	 * @brief setNodePositionsArrayField sets nodePositionsArray[row][column] to value
	 * @param integer row
	 * @param integer column
	 * @param qlonglong value
	 */
	void setNodePositionsArrayField( int column, int row, qlonglong value )
	{
		nodePositionsArray[column][row] = value;
	}

private:

	QList<qlonglong>* xAxisNodes;
	QList<qlonglong>* yAxisNodes;
	QList<qlonglong>* iNodes;
	QMap<qlonglong, QList<qlonglong>* >* connectedNodes;
	std::vector<std::vector<qlonglong> > nodePositionsArray;

};
}

#endif
