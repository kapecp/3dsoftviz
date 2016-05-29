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

	typedef struct
	{
		qlonglong xId;
		qlonglong yId;
	} NodeIds;

	typedef struct
	{
		qlonglong xPos;
		qlonglong yPos;
	} NodePos;

	typedef struct
	{
		QString xName;
		QString yName;
	} NodeNames;

	typedef struct
	{
		NodeIds nodeIds;
		NodePos nodePos;
		NodeNames nodeNames;
	} ParsedNode;

	typedef struct
	{
		ParsedNode frstNode;
		ParsedNode secNode;
		ParsedNode trdNode;
	} NodeTrio;

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
	Data::Graph* addNormalNodesToGraph( Data::Graph* matrixGraph, QStringList fields, SpecialMatrix::NodeConnections* connections, int &nodeCounter );

	/**
	 * @brief addNormalNodesToGraph adds I-I-[EN] || [EN]-I-I edge to graph
	 * @param Data::Graph* matrixGraph
	 * @param QStringList fields
	 * @param SpecialMatrix::NodeConnections* connections
	 * @param int &nodeCounter
	 * @return Data::Graph* matrix graph with new nodes/edges
	 */
	Data::Graph* addSpecialNodesToGraph( Data::Graph* matrixGraph, QStringList fields, SpecialMatrix::NodeConnections* connections, int &nodeCounter );

	/**
	 * @brief getHashIdFromNodeName
	 * @param QString nodeName
	 * @param char axis
	 * @return qlonglong ID for given nodeName and axis
	 */
	qlonglong getHashIdFromNodeName( QString nodeName, char axis );

	/**
	 * @brief getAvailablePosition calculates a new available position
	 * @param SpecialMatrix::NodeConnections* connections
	 * @param int row
	 * @param int column
	 * @return NodePos a new position of node
	 */
	NodePos getAvailablePosition( SpecialMatrix::NodeConnections* connections, int row, int column );

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
