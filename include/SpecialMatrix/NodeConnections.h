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

	/**
	 * \fn getXAxisNodes
	 * \brief Returns QList of Nodes positioned on the X axis.
	 */
	QList<qlonglong>* getXAxisNodes()
	{
		return xAxisNodes;
	}

	/**
	 * \fn getYAxisNodes
	 * \brief Returns QList of Nodes positioned on the Y axis.
	 */
	QList<qlonglong>* getYAxisNodes()
	{
		return yAxisNodes;
	}

	/**
	 * \fn getINodes
	 * \brief Returns QList of i Nodes.
	 */
	QList<qlonglong>* getINodes()
	{
		return iNodes;
	}

	/**
	 * \fn getConnectedNodes
	 * \brief Returns QMap of all connections in the matrix graph.
	 */
	QMap<qlonglong, QList<qlonglong>* >* getConnectedNodes()
	{
		return connectedNodes;
	}

private:

	QList<qlonglong>* xAxisNodes;
	QList<qlonglong>* yAxisNodes;
	QList<qlonglong>* iNodes;
	QMap<qlonglong, QList<qlonglong>* >* connectedNodes;


};
}

#endif
