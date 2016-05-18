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
	*  \class SpacialMatrix
	*  \brief
	*  \author Denis Illes
	*  \date 13. 4. 2016
	*/
class FileParser
{

public:

	FileParser( QString fileName, Data::Graph* matrixGraph, SpecialMatrix::NodeConnections* connections );

	void parseFile( Data::Graph* matrixGraph, QString fileName, SpecialMatrix::NodeConnections* connections );

	Data::Graph* addEINNodesToGraph( Data::Graph* matrixGraph, QStringList fields, SpecialMatrix::NodeConnections* connections );

	qlonglong getHashIdFromNodeName( QString nodeName, char axis );

	void createSettings( Data::Graph* matrixGraph );

private:

	Data::Type* eNodeType;

	Data::Type* iNodeType;

	Data::Type* nNodeType;
};
}

#endif
