#ifndef MATRIXVIEWER_H
#define MATRIXVIEWER_H 1


#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osgQt/GraphicsWindowQt>
#include <qtimer.h>
#include <QObject>
#include <QGridLayout>

#include "Data/Graph.h"
#include "Data/Node.h"
#include "Data/Edge.h"
#include "Data/GraphLayout.h"
#include "Viewer/CoreGraph.h"
#include "Layout/LayoutThread.h"
#include "Layout/FRAlgorithm.h"
#include "SpecialMatrix/NodeConnections.h"
#include "SpecialMatrix/FileParser.h"

namespace SpecialMatrix {
/**
	*  \class SpacialMatrix
	*  \brief
	*  \author Denis Illes
	*  \date 13. 4. 2016
	*/
class MatrixViewer
{


public:

	MatrixViewer( Data::Graph* matrixGraph, QString fileName );

	~MatrixViewer();

	/**
	 * @brief createAxis add nodes and edges to create x and y axis
	 */
	void createAxis();

	/**
	 * @brief exchangeNodes swap two axis nodes and the connected i nodes
	 * @param osg::ref_ptr<Data::Node> srcNode
	 * @param osg::ref_ptr<Data::Node> desNode
	 */
	void exchangeNodes( osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> desNode );

	/**
	 * @brief createEvolutionTab add elements to QWidget for evolution functionality
	 */
	void adjustPositions();

	/**
	 * @brief getMatrixGraph
	 * @return Data::Graph* active matrix graph
	 */
	Data::Graph* getMatrixGraph()
	{
		return matrixGraph;
	}

protected:

	QTimer _timer;

	Data::Graph* matrixGraph;

	SpecialMatrix::NodeConnections* connections;

	SpecialMatrix::FileParser* fileParser;

};
}

#endif
