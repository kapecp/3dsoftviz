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


	void createAxis();

	void adjustPositions();

	/**
	 * \fn getMatrixGraph
	 * \brief Returns active matrix graphs.
	 */
	Data::Graph* getMatrixGraph()
	{
		return matrixGraph;
	}

protected:

	QTimer _timer;

	Data::Graph* matrixGraph;

	SpecialMatrix::NodeConnections* connections;

};
}

#endif
