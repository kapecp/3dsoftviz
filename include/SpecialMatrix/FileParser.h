#ifndef MATRIXFILEPARSER_H
#define MATRIXFILEPARSER_H 1

#include <QObject>
#include <QGridLayout>
#include <QString>
#include <QDebug>

#include "Data/Graph.h"
#include "Data/Node.h"
#include "SpecialMatrix/NodeConnections.h"


namespace SpecialMatrix {
/**
	*  \class FileParser
	*  \brief
	*  \author Denis Illes
	*  \date 13. 4. 2016
	*/
class FileParser
{

public:

	typedef struct {
		qlonglong nodeId;
		int nodePos;
		QString nodeName;
	} ParsedNode;


	FileParser( QString fileName, Data::Graph* matrixGraph, SpecialMatrix::NodeConnections* connections );

	~FileParser();

	/**
	 * @brief parseFile parses the given file
	 * @param Data::Graph* matrixGraph
	 * @param QString fileName
	 * @param SpecialMatrix::NodeConnections* connections
	 */
	void parseFile( Data::Graph* matrixGraph, QString fileName, SpecialMatrix::NodeConnections* connections );

	/**
	 * @brief addNormalNodesToGraph adds [EN]-I-[EN] edge to graph
	 * @param Data::Graph* matrixGraph
	 * @param QStringList fields
	 * @param SpecialMatrix::NodeConnections* connections
	 * @param int &nodeCounter
	 * @return Data::Graph* matrix graph with new nodes/edges
	 */
	Data::Graph* addNormalNodesToGraph( Data::Graph* matrixGraph, QStringList fields, SpecialMatrix::NodeConnections* connections, int& nodeCounter );

	/**
	 * @brief addSpecialCase1NodesToGraph adds [EN]-I-[EN] || [EN]-[EN]-I || I-[EN]-[EN] edge to graph
	 * @param Data::Graph* matrixGraph
	 * @param QStringList fields
	 * @param SpecialMatrix::NodeConnections* connections
	 * @param int &nodeCounter
	 * @return Data::Graph* matrix graph with new nodes/edges
	 */
	Data::Graph* addSpecialCase1NodesToGraph( Data::Graph* matrixGraph, QStringList fields, SpecialMatrix::NodeConnections* connections, int& nodeCounter );

	/**
	 * @brief addSpecialCase2NodesToGraph adds [EN]-I-I || I-[EN]-I || I-I-[EN] edge to graph
	 * @param Data::Graph* matrixGraph
	 * @param QStringList fields
	 * @param SpecialMatrix::NodeConnections* connections
	 * @param int &nodeCounter
	 * @return Data::Graph* matrix graph with new nodes/edges
	 */
	Data::Graph* addSpecialCase2NodesToGraph( Data::Graph* matrixGraph, QStringList fields, SpecialMatrix::NodeConnections* connections, int& nodeCounter );

	/**
	 * @brief getAvailablePosition calculates a new available position
	 * @param SpecialMatrix::NodeConnections* connections
	 * @param int row
	 * @param int column
	 * @return NodePos a new position of node
	 */
	osg::Vec2f getAvailablePosition( SpecialMatrix::NodeConnections* connections, int row, int column );

	/**
	 * @brief createSettings initializes the used types
	 * @param Data::Graph* matrixGraph
	 */
	void createSettings( Data::Graph* matrixGraph );

private:

	Data::Type* eNodeType;

	Data::Type* iFullNodeType;

	Data::Type* iHalfNodeType;

	Data::Type* nNodeType;

	Data::Type* iEdgeType;
};
}

#endif
