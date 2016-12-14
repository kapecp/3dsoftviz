#include "SpecialMatrix/FileParser.h"

#include <QtGui>

#pragma warning(push)
#pragma warning(disable:4244)

SpecialMatrix::FileParser::FileParser( QString fileName, Data::Graph* matrixGraph, SpecialMatrix::NodeConnections* connections )
{
	createSettings( matrixGraph );
	parseFile( matrixGraph, fileName, connections );
}

SpecialMatrix::FileParser::~FileParser()
{
	delete eNodeType;
	delete iFullNodeType;
	delete iHalfNodeType;
	delete nNodeType;
	delete iEdgeType;
}

void SpecialMatrix::FileParser::parseFile( Data::Graph* matrixGraph, QString fileName, SpecialMatrix::NodeConnections* connections )
{
	static int nodeCounter = 1;
	QFile file( fileName );
	if ( !file.open( QIODevice::ReadOnly ) ) {
		QMessageBox::information( 0, "error", file.errorString() );
	}

	QTextStream in( &file );

	while ( !in.atEnd() ) {
		QString line = in.readLine();
		QStringList fields = line.split( '\t' );
		//qDebug() << "line: "<< fields.at(0) << ", " << fields.at(1) << ", " << fields.at(2);

		if ( ( fields.at( 0 ).contains( QRegExp( "[Ee]" ) ) && fields.at( 1 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 2 ).contains( QRegExp( "[Nn]" ) ) ) ) {
			matrixGraph = addNormalNodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}
		else if ( ( fields.at( 0 ).contains( QRegExp( "[Nn]" ) ) && fields.at( 1 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 2 ).contains( QRegExp( "[Ee]" ) ) ) ) {
			matrixGraph = addNormalNodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}

		else if ( ( fields.at( 0 ).contains( QRegExp( "[Ee]" ) ) && fields.at( 1 ).contains( QRegExp( "[Ee]" ) ) && fields.at( 2 ).contains( QRegExp( "[Ii]" ) ) ) ) {
			matrixGraph = addSpecialCase1NodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}
		else if ( ( fields.at( 0 ).contains( QRegExp( "[Ee]" ) ) && fields.at( 1 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 2 ).contains( QRegExp( "[Ee]" ) ) ) ) {
			matrixGraph = addSpecialCase1NodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}
		else if ( ( fields.at( 0 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 1 ).contains( QRegExp( "[Ee]" ) ) && fields.at( 2 ).contains( QRegExp( "[Ee]" ) ) ) ) {
			matrixGraph = addSpecialCase1NodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}
		else if ( ( fields.at( 0 ).contains( QRegExp( "[Nn]" ) ) && fields.at( 1 ).contains( QRegExp( "[Nn]" ) ) && fields.at( 2 ).contains( QRegExp( "[Ii]" ) ) ) ) {
			matrixGraph = addSpecialCase1NodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}
		else if ( ( fields.at( 0 ).contains( QRegExp( "[Nn]" ) ) && fields.at( 1 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 2 ).contains( QRegExp( "[Nn]" ) ) ) ) {
			matrixGraph = addSpecialCase1NodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}
		else if ( ( fields.at( 0 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 1 ).contains( QRegExp( "[Nn]" ) ) && fields.at( 2 ).contains( QRegExp( "[Nn]" ) ) ) ) {
			matrixGraph = addSpecialCase1NodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}

		else if ( ( fields.at( 0 ).contains( QRegExp( "[Ee]" ) ) && fields.at( 1 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 2 ).contains( QRegExp( "[Ii]" ) ) ) ) {
			matrixGraph = addSpecialCase2NodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}
		else if ( ( fields.at( 0 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 1 ).contains( QRegExp( "[Ee]" ) ) && fields.at( 2 ).contains( QRegExp( "[Ii]" ) ) ) ) {
			matrixGraph = addSpecialCase2NodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}
		else if ( ( fields.at( 0 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 1 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 2 ).contains( QRegExp( "[Ee]" ) ) ) ) {
			matrixGraph = addSpecialCase2NodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}
		else if ( ( fields.at( 0 ).contains( QRegExp( "[Nn]" ) ) && fields.at( 1 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 2 ).contains( QRegExp( "[Ii]" ) ) ) ) {
			matrixGraph = addSpecialCase2NodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}
		else if ( ( fields.at( 0 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 1 ).contains( QRegExp( "[Nn]" ) ) && fields.at( 2 ).contains( QRegExp( "[Nn]" ) ) ) ) {
			matrixGraph = addSpecialCase2NodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}
		else if ( ( fields.at( 0 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 1 ).contains( QRegExp( "[Ii]" ) ) && fields.at( 2 ).contains( QRegExp( "[Nn]" ) ) ) ) {
			matrixGraph = addSpecialCase2NodesToGraph( matrixGraph, fields, connections, nodeCounter );
		}
	}
	file.close();
}

Data::Graph* SpecialMatrix::FileParser::addNormalNodesToGraph( Data::Graph* matrixGraph, QStringList fields, SpecialMatrix::NodeConnections* connections, int& nodeCounter )
{
	//cases: E - I - N, N - I - E
	float separator = Util::ApplicationConfig::get()->getValue( "Viewer.Display.MatrixNodeSeparator" ).toFloat();
	osg::Vec3f nodePos;
	osg::ref_ptr<Data::Node> foundNode;
	qlonglong foundNodeId;

	QList<qlonglong>* xAxisNodes = connections->getXAxisNodes();
	QList<qlonglong>* yAxisNodes = connections->getYAxisNodes();
	QList<qlonglong>* iNodes = connections->getINodes();
	QMap<qlonglong, QList<qlonglong>* >* connectedNodes = connections->getConnectedNodes();
	QList<qlonglong>* tempList;

	//=====================start node (E||N)=====================
	ParsedNode startNode;
	startNode.nodeName = fields.at( 0 );
	startNode.nodeId = qHash( startNode.nodeName );

	if ( startNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
		if ( !xAxisNodes->contains( startNode.nodeId ) ) {
			//insert node into xAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			xAxisNodes->push_back( startNode.nodeId );
			startNode.nodePos = xAxisNodes->indexOf( startNode.nodeId )+1;
			nodePos = osg::Vec3f( static_cast<float>( startNode.nodePos )*separator, 0.0f, 0.0f );
			matrixGraph->addNode( startNode.nodeId, startNode.nodeName, nNodeType, nodePos );
			connections->setNodePositionsArrayField( startNode.nodePos, 0, startNode.nodeId );
		}
		//get list from Qmap where key is xFrstNodeId and insert I node into connectedNodes list
		tempList = connectedNodes->value( startNode.nodeId );
		if ( tempList == nullptr ) {
			tempList = new QList<qlonglong>;
		}
		tempList->push_back( qHash( fields.at( 1 ) ) );
		connectedNodes->insert( startNode.nodeId, tempList );
	}

	else if ( startNode.nodeName.contains( QRegExp( "[Ee]" ) ) ) {
		if ( !yAxisNodes->contains( startNode.nodeId ) ) {
			//insert node into yAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			yAxisNodes->push_back( startNode.nodeId );
			startNode.nodePos = yAxisNodes->indexOf( startNode.nodeId )+1;
			nodePos = osg::Vec3f( 0.0f, static_cast<float>( startNode.nodePos )*separator, 0.0f );
			matrixGraph->addNode( startNode.nodeId, startNode.nodeName, eNodeType, nodePos );
			connections->setNodePositionsArrayField( 0, startNode.nodePos, startNode.nodeId );
		}
		//get list from Qmap where key is xFrstNodeId and insert I node into connectedNodes list
		tempList = connectedNodes->value( startNode.nodeId );
		if ( tempList == nullptr ) {
			tempList = new QList<qlonglong>;
		}
		tempList->push_back( qHash( fields.at( 1 ) ) );
		connectedNodes->insert( startNode.nodeId, tempList );
	}

	//=====================end node (E||N)=====================
	ParsedNode endNode;
	endNode.nodeName = fields.at( 2 );
	endNode.nodeId = qHash( endNode.nodeName );

	if ( endNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
		if ( !xAxisNodes->contains( endNode.nodeId ) ) {
			//insert node into xAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			xAxisNodes->push_back( endNode.nodeId );
			endNode.nodePos = xAxisNodes->indexOf( endNode.nodeId )+1;
			nodePos = osg::Vec3f( static_cast<float>( endNode.nodePos )*separator, 0.0f, 0.0f );
			matrixGraph->addNode( endNode.nodeId, endNode.nodeName, nNodeType, nodePos );
			connections->setNodePositionsArrayField( endNode.nodePos, 0, endNode.nodeId );
		}
		//get list from Qmap where key is xFrstNodeId and insert I node into connectedNodes list
		tempList = connectedNodes->value( endNode.nodeId );
		if ( tempList == nullptr ) {
			tempList = new QList<qlonglong>;
		}
		tempList->push_back( qHash( fields.at( 1 ) ) );
		connectedNodes->insert( endNode.nodeId, tempList );
	}

	else if ( endNode.nodeName.contains( QRegExp( "[Ee]" ) ) ) {
		if ( !yAxisNodes->contains( endNode.nodeId ) ) {
			//insert node into yAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			yAxisNodes->push_back( endNode.nodeId );
			endNode.nodePos = yAxisNodes->indexOf( endNode.nodeId )+1;
			nodePos = osg::Vec3f( 0.0f, static_cast<float>( endNode.nodePos )*separator, 0.0f );
			matrixGraph->addNode( endNode.nodeId, endNode.nodeName, eNodeType, nodePos );
			connections->setNodePositionsArrayField( 0, endNode.nodePos, endNode.nodeId );
		}
		//get list from Qmap where key is xFrstNodeId and insert I node into connectedNodes list
		tempList = connectedNodes->value( endNode.nodeId );
		if ( tempList == nullptr ) {
			tempList = new QList<qlonglong>;
		}
		tempList->push_back( qHash( fields.at( 1 ) ) );
		connectedNodes->insert( endNode.nodeId, tempList );
	}

	//=====================middle node (I)=====================
	ParsedNode midNode;
	midNode.nodeName = fields.at( 1 );
	midNode.nodeId = qHash( midNode.nodeName );

	if ( !iNodes->contains( midNode.nodeId ) ) {
		//insert node into iNodes list only if missing
		iNodes->push_back( midNode.nodeId );
		osg::Vec2f pos;
		if ( startNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
			pos.x() = static_cast<float>( xAxisNodes->indexOf( startNode.nodeId )+1 );
			pos.y() = static_cast<float>( yAxisNodes->indexOf( endNode.nodeId )+1 );

		}
		else {
			pos.x() = static_cast<float>( xAxisNodes->indexOf( endNode.nodeId )+1 );
			pos.y() = static_cast<float>( yAxisNodes->indexOf( startNode.nodeId )+1 );
		}

		//CHECK AVAIBILITY - if there is a node, move it
		foundNodeId = connections->getNodePositionsArrayField( static_cast<int>( pos.x() ), static_cast<int>( pos.y() ) );
		if ( foundNodeId ) {
			foundNode = matrixGraph->findNodeById( foundNodeId );
			osg::Vec2f foundNodePos = getAvailablePosition( connections, static_cast<int>( pos.x() ), static_cast<int>( pos.y() ) );
			foundNode->setTargetPosition( osg::Vec3f( foundNodePos.x()*separator , foundNodePos.y()*separator , 0.0f ) );
			connections->setNodePositionsArrayField( static_cast<int>( foundNodePos.x() ), static_cast<int>( foundNodePos.y() ), foundNodeId );
		}

		//nodePos = osg::Vec3f( osg::Vec3f( pos.x()*separator,  pos.y()*separator, 0.0f ) );
		nodePos = osg::Vec3f( pos.x()*separator,  pos.y()*separator, 0.0f );
		matrixGraph->addNode( midNode.nodeId, midNode.nodeName, iFullNodeType, nodePos );
		connections->setNodePositionsArrayField( static_cast<int>( pos.x() ), static_cast<int>( pos.y() ), midNode.nodeId );
	}
	return matrixGraph;
}

Data::Graph* SpecialMatrix::FileParser::addSpecialCase1NodesToGraph( Data::Graph* matrixGraph, QStringList fields, SpecialMatrix::NodeConnections* connections, int& nodeCounter )
{
	//cases: E - I - E, E - E - I, I - E - E || N - I - N, N - N - I, I - N - N
	int separator = Util::ApplicationConfig::get()->getValue( "Viewer.Display.MatrixNodeSeparator" ).toInt();
	osg::Vec3f nodePos;
	QString edgeName;
	qlonglong edgeId;
	osg::ref_ptr<Data::Node> startGraphNode, midGraphNode, endGraphNode;

	QList<qlonglong>* xAxisNodes = connections->getXAxisNodes();
	QList<qlonglong>* yAxisNodes = connections->getYAxisNodes();
	QList<qlonglong>* iNodes = connections->getINodes();

	//=====================start node (E||N||I)=====================
	ParsedNode startNode;
	startNode.nodeName = fields.at( 0 );
	startNode.nodeId = qHash( startNode.nodeName );

	if ( startNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
		if ( !xAxisNodes->contains( startNode.nodeId ) ) {
			//insert node into xAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			xAxisNodes->push_back( startNode.nodeId );
			startNode.nodePos = xAxisNodes->indexOf( startNode.nodeId )+1;
			nodePos = osg::Vec3f( static_cast<float>( startNode.nodePos *separator ), 0.0f, 0.0f );
			startGraphNode = matrixGraph->addNode( startNode.nodeId, startNode.nodeName, nNodeType, nodePos );
			connections->setNodePositionsArrayField( startNode.nodePos, 0, startNode.nodeId );
		}
		else {
			startGraphNode = matrixGraph->findNodeById( startNode.nodeId );
		}
	}

	else if ( startNode.nodeName.contains( QRegExp( "[Ee]" ) ) ) {
		if ( !yAxisNodes->contains( startNode.nodeId ) ) {
			//insert node into yAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			yAxisNodes->push_back( startNode.nodeId );
			startNode.nodePos = yAxisNodes->indexOf( startNode.nodeId )+1;
			nodePos = osg::Vec3f( 0.0f, static_cast<float>( startNode.nodePos *separator ), 0.0f );
			startGraphNode = matrixGraph->addNode( startNode.nodeId, startNode.nodeName, eNodeType, nodePos );
			connections->setNodePositionsArrayField( 0, startNode.nodePos, startNode.nodeId );
		}
		else {
			startGraphNode = matrixGraph->findNodeById( startNode.nodeId );
		}
	}

	//=====================mid node (E||N||I)=====================
	ParsedNode midNode;
	midNode.nodeName = fields.at( 1 );
	midNode.nodeId = qHash( midNode.nodeName );

	if ( midNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
		if ( !xAxisNodes->contains( midNode.nodeId ) ) {
			//insert node into xAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			xAxisNodes->push_back( midNode.nodeId );
			midNode.nodePos = xAxisNodes->indexOf( midNode.nodeId )+1;
			nodePos = osg::Vec3f( static_cast<float>( midNode.nodePos *separator ), 0.0f, 0.0f );
			midGraphNode = matrixGraph->addNode( midNode.nodeId, midNode.nodeName, nNodeType, nodePos );
			connections->setNodePositionsArrayField( midNode.nodePos, 0, midNode.nodeId );
		}
		else {
			midGraphNode = matrixGraph->findNodeById( midNode.nodeId );
		}
	}

	else if ( midNode.nodeName.contains( QRegExp( "[Ee]" ) ) ) {
		if ( !yAxisNodes->contains( midNode.nodeId ) ) {
			//insert node into yAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			yAxisNodes->push_back( midNode.nodeId );
			midNode.nodePos = yAxisNodes->indexOf( midNode.nodeId )+1;
			nodePos = osg::Vec3f( 0.0f, static_cast<float>( midNode.nodePos *separator ), 0.0f );
			midGraphNode = matrixGraph->addNode( midNode.nodeId, midNode.nodeName, eNodeType, nodePos );
			connections->setNodePositionsArrayField( 0, midNode.nodePos, midNode.nodeId );
		}
		else {
			midGraphNode = matrixGraph->findNodeById( midNode.nodeId );
		}
	}

	//=====================end node (E||N||I)=====================
	ParsedNode endNode;
	endNode.nodeName = fields.at( 2 );
	endNode.nodeId = qHash( endNode.nodeName );

	if ( endNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
		if ( !xAxisNodes->contains( endNode.nodeId ) ) {
			//insert node into xAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			xAxisNodes->push_back( endNode.nodeId );
			endNode.nodePos = xAxisNodes->indexOf( endNode.nodeId )+1;
			nodePos = osg::Vec3f( static_cast<float>( endNode.nodePos *separator ), 0.0f, 0.0f );
			endGraphNode = matrixGraph->addNode( endNode.nodeId, endNode.nodeName, nNodeType, nodePos );
			connections->setNodePositionsArrayField( endNode.nodePos, 0, endNode.nodeId );
		}
		else {
			endGraphNode = matrixGraph->findNodeById( endNode.nodeId );
		}
	}

	else if ( endNode.nodeName.contains( QRegExp( "[Ee]" ) ) ) {
		if ( !yAxisNodes->contains( endNode.nodeId ) ) {
			//insert node into yAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			yAxisNodes->push_back( endNode.nodeId );
			endNode.nodePos = yAxisNodes->indexOf( endNode.nodeId )+1;
			nodePos = osg::Vec3f( 0.0f, static_cast<float>( endNode.nodePos *separator ), 0.0f );
			endGraphNode = matrixGraph->addNode( endNode.nodeId, endNode.nodeName, eNodeType, nodePos );
			connections->setNodePositionsArrayField( 0, endNode.nodePos, endNode.nodeId );
		}
		else {
			endGraphNode = matrixGraph->findNodeById( endNode.nodeId );
		}
	}

	//=====================calculate I node=====================
	if ( startNode.nodeName.contains( QRegExp( "[Ii]" ) ) ) {
		if ( !iNodes->contains( startNode.nodeId ) ) {
			//insert node into iNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			iNodes->push_back( startNode.nodeId );
			osg::Vec2f pos;
			if ( midNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
				//case: I - N - N
				//odstranenie pretypovania na int
				int midPos =  xAxisNodes->indexOf( midNode.nodeId )+1 ;
				int endPos =  xAxisNodes->indexOf( endNode.nodeId )+1 ;
				int delta = abs( midPos - endPos );
				pos.x() = ( midPos > endPos ) ? static_cast<float>( ( midPos - delta ) ) : static_cast<float>( ( endPos - delta ) );
				pos.y() = pos.x();
			}
			else {
				//case: I - E - E
				int midPos =  yAxisNodes->indexOf( midNode.nodeId )+1 ;
				int endPos =  yAxisNodes->indexOf( endNode.nodeId )+1 ;
				int delta = abs( midPos - endPos );
				pos.x() = ( midPos > endPos ) ? static_cast<float>( ( midPos - delta ) ) : static_cast<float>( ( endPos - delta ) );
				pos.y() = pos.x();
			}
			pos = getAvailablePosition( connections, static_cast<int>( pos.x() ), static_cast<int>( pos.y() ) );
			nodePos =  osg::Vec3f( pos.x()*static_cast<float>( separator ), pos.y()*static_cast<float>( separator ), 0.0f ) ;
			startGraphNode = matrixGraph->addNode( startNode.nodeId, startNode.nodeName, iHalfNodeType, nodePos );
			connections->setNodePositionsArrayField( static_cast<int>( pos.x() ), static_cast<int>( pos.y() ), startNode.nodeId );
		}
		else {
			startGraphNode = matrixGraph->findNodeById( startNode.nodeId );
		}

	}
	else if ( midNode.nodeName.contains( QRegExp( "[Ii]" ) ) ) {
		if ( !iNodes->contains( midNode.nodeId ) ) {
			//insert node into iNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			iNodes->push_back( midNode.nodeId );
			osg::Vec2f pos;
			if ( startNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
				//case: N - I - N
				int startPos =  xAxisNodes->indexOf( startNode.nodeId )+1 ;
				int endPos =  xAxisNodes->indexOf( endNode.nodeId )+1 ;
				int delta = abs( startPos - endPos );
				pos.x() = ( startPos > endPos ) ? static_cast<float>( ( startPos - delta ) ) : static_cast<float>( ( endPos - delta ) );
				pos.y() = pos.x();
			}
			else {
				//case: E - I - E
				int startPos =  yAxisNodes->indexOf( startNode.nodeId )+1 ;
				int endPos =  yAxisNodes->indexOf( endNode.nodeId )+1 ;
				int delta = abs( startPos - endPos );
				pos.x() = ( startPos > endPos ) ? static_cast<float>( ( startPos - delta ) ) : static_cast<float>( ( endPos - delta ) );
				pos.y() = pos.x();
			}
			pos = getAvailablePosition( connections, static_cast<int>( pos.x() ), static_cast<int>( pos.y() ) );
			nodePos =  osg::Vec3f( pos.x()*static_cast<float>( separator ), pos.y()*static_cast<float>( separator ), 0.0f ) ;
			midGraphNode = matrixGraph->addNode( midNode.nodeId, midNode.nodeName, iHalfNodeType, nodePos );
			connections->setNodePositionsArrayField( static_cast<int>( pos.x() ), static_cast<int>( pos.y() ), midNode.nodeId );
		}
		else {
			midGraphNode = matrixGraph->findNodeById( midNode.nodeId );
		}

	}
	else if ( endNode.nodeName.contains( QRegExp( "[Ii]" ) ) ) {
		if ( !iNodes->contains( endNode.nodeId ) ) {
			//insert node into iNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			iNodes->push_back( endNode.nodeId );
			osg::Vec2f pos;
			if ( startNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
				//case: N - N - I
				//int startPos = static_cast<int>( xAxisNodes->indexOf( startNode.nodeId )+1 );
				int startPos =  xAxisNodes->indexOf( startNode.nodeId )+1 ;
				int midPos =  xAxisNodes->indexOf( midNode.nodeId )+1 ;
				int delta = abs( startPos - midPos );
				pos.x() = ( startPos > midPos ) ? static_cast<float>( ( startPos - delta ) ) : static_cast<float>( ( midPos - delta ) );
				pos.y() = pos.x();
			}
			else {
				//case: E - E - I
				int startPos =  yAxisNodes->indexOf( startNode.nodeId )+1 ;
				int midPos =  yAxisNodes->indexOf( midNode.nodeId )+1 ;
				int delta = abs( startPos - midPos );
				pos.x() = ( startPos > midPos ) ? static_cast<float>( ( startPos - delta ) ) : static_cast<float>( ( midPos - delta ) );
				pos.y() = pos.x();
			}
			pos = getAvailablePosition( connections, static_cast<int>( pos.x() ), static_cast<int>( pos.y() ) );
			nodePos =  osg::Vec3f( pos.x()*static_cast<float>( separator ), pos.y()*static_cast<float>( separator ), 0.0f ) ;
			endGraphNode = matrixGraph->addNode( endNode.nodeId, endNode.nodeName, iHalfNodeType, nodePos );
			connections->setNodePositionsArrayField( static_cast<int>( pos.x() ), static_cast<int>( pos.y() ), endNode.nodeId );
		}
		else {
			endGraphNode = matrixGraph->findNodeById( endNode.nodeId );
		}
	}

	//same in every condition
	edgeName = midNode.nodeName;
	edgeName.append( startNode.nodeName );
	edgeId = qHash( edgeName );
	matrixGraph->addEdge( edgeId, edgeName, midGraphNode, startGraphNode, iEdgeType, false );

	edgeName = midNode.nodeName;
	edgeName.append( endNode.nodeName );
	edgeId = qHash( edgeName );
	matrixGraph->addEdge( edgeId, edgeName, midGraphNode, endGraphNode, iEdgeType, false );

	return matrixGraph;
}

Data::Graph* SpecialMatrix::FileParser::addSpecialCase2NodesToGraph( Data::Graph* matrixGraph, QStringList fields, SpecialMatrix::NodeConnections* connections, int& nodeCounter )
{
	//cases: E - I - I, I - E - I, I - I - E || N - I - I, I - N - I, I - I - N
	int separator = Util::ApplicationConfig::get()->getValue( "Viewer.Display.MatrixNodeSeparator" ).toInt();
	osg::Vec3f nodePos;
	QString edgeName;
	qlonglong edgeId;
	osg::ref_ptr<Data::Node> startGraphNode, midGraphNode, endGraphNode;

	QList<qlonglong>* xAxisNodes = connections->getXAxisNodes();
	QList<qlonglong>* yAxisNodes = connections->getYAxisNodes();
	QList<qlonglong>* iNodes = connections->getINodes();

	//=====================start node (E||N||I)=====================
	ParsedNode startNode;
	startNode.nodeName = fields.at( 0 );
	startNode.nodeId = qHash( startNode.nodeName );

	if ( startNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
		if ( !xAxisNodes->contains( startNode.nodeId ) ) {
			//insert node into xAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			xAxisNodes->push_back( startNode.nodeId );
			startNode.nodePos = xAxisNodes->indexOf( startNode.nodeId )+1;
			//nodePos = osg::Vec3f( static_cast<float>( startNode.nodePos )*separator, 0.0f, 0.0f );
			nodePos = osg::Vec3f( static_cast<float>( startNode.nodePos *separator ), 0.0f, 0.0f );
			startGraphNode = matrixGraph->addNode( startNode.nodeId, startNode.nodeName, nNodeType, nodePos );
			connections->setNodePositionsArrayField( startNode.nodePos, 0, startNode.nodeId );
		}
		else {
			startGraphNode = matrixGraph->findNodeById( startNode.nodeId );
		}
	}

	else if ( startNode.nodeName.contains( QRegExp( "[Ee]" ) ) ) {
		if ( !yAxisNodes->contains( startNode.nodeId ) ) {
			//insert node into yAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			yAxisNodes->push_back( startNode.nodeId );
			startNode.nodePos = yAxisNodes->indexOf( startNode.nodeId )+1;
			nodePos = osg::Vec3f( 0.0f, static_cast<float>( startNode.nodePos *separator ), 0.0f );
			startGraphNode = matrixGraph->addNode( startNode.nodeId, startNode.nodeName, eNodeType, nodePos );
			connections->setNodePositionsArrayField( 0, startNode.nodePos, startNode.nodeId );
		}
		else {
			startGraphNode = matrixGraph->findNodeById( startNode.nodeId );
		}
	}

	//=====================mid node (E||N||I)=====================
	ParsedNode midNode;
	midNode.nodeName = fields.at( 1 );
	midNode.nodeId = qHash( midNode.nodeName );

	if ( midNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
		if ( !xAxisNodes->contains( midNode.nodeId ) ) {
			//insert node into xAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			xAxisNodes->push_back( midNode.nodeId );
			midNode.nodePos = xAxisNodes->indexOf( midNode.nodeId )+1;
			nodePos = osg::Vec3f( static_cast<float>( midNode.nodePos *separator ), 0.0f, 0.0f );
			midGraphNode = matrixGraph->addNode( midNode.nodeId, midNode.nodeName, nNodeType, nodePos );
			connections->setNodePositionsArrayField( midNode.nodePos, 0, midNode.nodeId );
		}
		else {
			midGraphNode = matrixGraph->findNodeById( midNode.nodeId );
		}
	}

	else if ( midNode.nodeName.contains( QRegExp( "[Ee]" ) ) ) {
		if ( !yAxisNodes->contains( midNode.nodeId ) ) {
			//insert node into yAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			yAxisNodes->push_back( midNode.nodeId );
			midNode.nodePos = yAxisNodes->indexOf( midNode.nodeId )+1;
			nodePos = osg::Vec3f( 0.0f, static_cast<float>( midNode.nodePos *separator ), 0.0f );
			midGraphNode = matrixGraph->addNode( midNode.nodeId, midNode.nodeName, eNodeType, nodePos );
			connections->setNodePositionsArrayField( 0, midNode.nodePos, midNode.nodeId );
		}
		else {
			midGraphNode = matrixGraph->findNodeById( midNode.nodeId );
		}
	}

	//=====================end node (E||N||I)=====================
	ParsedNode endNode;
	endNode.nodeName = fields.at( 2 );
	endNode.nodeId = qHash( endNode.nodeName );

	if ( endNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
		if ( !xAxisNodes->contains( endNode.nodeId ) ) {
			//insert node into xAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			xAxisNodes->push_back( endNode.nodeId );
			endNode.nodePos = xAxisNodes->indexOf( endNode.nodeId )+1;
			nodePos = osg::Vec3f( static_cast<float>( endNode.nodePos *separator ), 0.0f, 0.0f );
			endGraphNode = matrixGraph->addNode( endNode.nodeId, endNode.nodeName, nNodeType, nodePos );
			connections->setNodePositionsArrayField( endNode.nodePos, 0, endNode.nodeId );
		}
		else {
			endGraphNode = matrixGraph->findNodeById( endNode.nodeId );
		}
	}

	else if ( endNode.nodeName.contains( QRegExp( "[Ee]" ) ) ) {
		if ( !yAxisNodes->contains( endNode.nodeId ) ) {
			//insert node into yAxisNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			yAxisNodes->push_back( endNode.nodeId );
			endNode.nodePos = yAxisNodes->indexOf( endNode.nodeId )+1;
			nodePos = osg::Vec3f( 0.0f, static_cast<float>( endNode.nodePos *separator ), 0.0f );
			endGraphNode = matrixGraph->addNode( endNode.nodeId, endNode.nodeName, eNodeType, nodePos );
			connections->setNodePositionsArrayField( 0, endNode.nodePos, endNode.nodeId );
		}
		else {
			endGraphNode = matrixGraph->findNodeById( endNode.nodeId );
		}
	}

	//=====================calculate I node=====================
	if ( startNode.nodeName.contains( QRegExp( "[EeNn]" ) ) ) {
		//case: E/N - I - I
		osg::Vec2f pos;
		if ( startNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
			//case: N - I - I
			pos.x() = static_cast<float>( xAxisNodes->indexOf( startNode.nodeId )+1 );
			pos.y() = pos.x();
		}
		else {
			//case: E - I - I
			pos.y() = static_cast<float>( yAxisNodes->indexOf( startNode.nodeId )+1 );
			pos.x() = pos.y();
		}

		if ( !iNodes->contains( endNode.nodeId ) ) {
			//insert node into iNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			iNodes->push_back( endNode.nodeId );
			//int help_pos_x =static_cast<int>( pos.x() );
			//int help_pos_y =static_cast<int>( pos.y() );
			pos = getAvailablePosition( connections, static_cast<int>( pos.x() ), static_cast<int>( pos.y() ) );
			//pos = getAvailablePosition( connections, pos.x(), pos.y() );
			nodePos =  osg::Vec3f( pos.x()*static_cast<float>( separator ), pos.y()*static_cast<float>( separator ), 0.0f ) ;
			endGraphNode = matrixGraph->addNode( endNode.nodeId, endNode.nodeName, iHalfNodeType, nodePos );
			connections->setNodePositionsArrayField( static_cast<int>( pos.x() ), static_cast<int>( pos.y() ), endNode.nodeId );
		}
		else {
			endGraphNode = matrixGraph->findNodeById( endNode.nodeId );
		}

		if ( !iNodes->contains( midNode.nodeId ) ) {
			//insert node into iNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			iNodes->push_back( midNode.nodeId );

			if ( startNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
				pos = getAvailablePosition( connections, static_cast<int>( pos.x() ), static_cast<int>( pos.y() )/2 );
			}
			else {
				pos = getAvailablePosition( connections, static_cast<int>( pos.x() )/2, static_cast<int>( pos.y() ) );
			}
			nodePos =  osg::Vec3f( pos.x()*static_cast<float>( separator ), pos.y()*static_cast<float>( separator ), 0.0f ) ;
			midGraphNode = matrixGraph->addNode( midNode.nodeId, midNode.nodeName, iHalfNodeType, nodePos );
			connections->setNodePositionsArrayField( static_cast<int>( pos.x() ), static_cast<int>( pos.y() ), midNode.nodeId );
		}
		else {
			midGraphNode = matrixGraph->findNodeById( midNode.nodeId );
		}

	}
	else if ( midNode.nodeName.contains( QRegExp( "[EeNn]" ) ) ) {
		//case: I - E/N - I
		osg::Vec2f pos;
		if ( midNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
			//case: I - N - I
			pos.x() = static_cast<float>( xAxisNodes->indexOf( midNode.nodeId )+1 );
			pos.y() = pos.x();
		}
		else {
			//case: I - E - I
			pos.y() = static_cast<float>( yAxisNodes->indexOf( midNode.nodeId )+1 );
			pos.x() = pos.y();
		}

		if ( !iNodes->contains( endNode.nodeId ) ) {
			//insert node into iNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			iNodes->push_back( endNode.nodeId );

			pos = getAvailablePosition( connections, static_cast<int>( pos.x() ), static_cast<int>( pos.y() ) );
			nodePos =  osg::Vec3f( pos.x()*static_cast<float>( separator ), pos.y()*static_cast<float>( separator ), 0.0f ) ;
			endGraphNode = matrixGraph->addNode( endNode.nodeId, endNode.nodeName, iHalfNodeType, nodePos );
			connections->setNodePositionsArrayField( static_cast<int>( pos.x() ), static_cast<int>( pos.y() ), endNode.nodeId );
		}
		else {
			endGraphNode = matrixGraph->findNodeById( endNode.nodeId );
		}

		if ( !iNodes->contains( startNode.nodeId ) ) {
			//insert node into iNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			iNodes->push_back( startNode.nodeId );

			if ( midNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
				pos = getAvailablePosition( connections, static_cast<int>( pos.x() ), static_cast<int>( pos.y() )/2 );
			}
			else {
				pos = getAvailablePosition( connections, static_cast<int>( pos.x() )/2, static_cast<int>( pos.y() ) );
			}
			nodePos =  osg::Vec3f( pos.x()*static_cast<float>( separator ), pos.y()*static_cast<float>( separator ), 0.0f ) ;
			startGraphNode = matrixGraph->addNode( startNode.nodeId, startNode.nodeName, iHalfNodeType, nodePos );
			connections->setNodePositionsArrayField( static_cast<int>( pos.x() ), static_cast<int>( pos.y() ), startNode.nodeId );
		}
		else {
			startGraphNode = matrixGraph->findNodeById( startNode.nodeId );
		}

	}
	else if ( endNode.nodeName.contains( QRegExp( "[EeNn]" ) ) ) {
		//case: I - I - E/N
		osg::Vec2f pos;
		if ( endNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
			//case: I - I - N
			pos.x() = static_cast<float>( xAxisNodes->indexOf( endNode.nodeId )+1 );
			pos.y() = pos.x();
		}
		else {
			//case: I - I - E
			pos.y() = static_cast<float>( yAxisNodes->indexOf( endNode.nodeId )+1 );
			pos.x() = pos.y();
		}

		if ( !iNodes->contains( midNode.nodeId ) ) {
			//insert node into iNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			iNodes->push_back( midNode.nodeId );

			pos = getAvailablePosition( connections, static_cast<int>( pos.x() ), static_cast<int>( pos.y() ) );
			nodePos =  osg::Vec3f( pos.x()*static_cast<float>( separator ), pos.y()*static_cast<float>( separator ), 0.0f ) ;
			midGraphNode = matrixGraph->addNode( midNode.nodeId, midNode.nodeName, iHalfNodeType, nodePos );
			connections->setNodePositionsArrayField( static_cast<int>( pos.x() ), static_cast<int>( pos.y() ), midNode.nodeId );
		}
		else {
			midGraphNode = matrixGraph->findNodeById( midNode.nodeId );
		}

		if ( !iNodes->contains( startNode.nodeId ) ) {
			//insert node into iNodes list only if missing
			connections->setNodePositionsArray( ++nodeCounter );
			iNodes->push_back( startNode.nodeId );

			if ( midNode.nodeName.contains( QRegExp( "[Nn]" ) ) ) {
				pos = getAvailablePosition( connections, static_cast<int>( pos.x() ), static_cast<int>( pos.y() )/2 );
			}
			else {
				pos = getAvailablePosition( connections, static_cast<int>( pos.x() )/2, static_cast<int>( pos.y() ) );
			}
			nodePos =  osg::Vec3f( pos.x()*static_cast<float>( separator ), pos.y()*static_cast<float>( separator ), 0.0f ) ;
			startGraphNode = matrixGraph->addNode( startNode.nodeId, startNode.nodeName, iHalfNodeType, nodePos );
			connections->setNodePositionsArrayField( static_cast<int>( pos.x() ), static_cast<int>( pos.y() ), startNode.nodeId );
		}
		else {
			startGraphNode = matrixGraph->findNodeById( startNode.nodeId );
		}
	}

	//same in every condition
	edgeName = midNode.nodeName;
	edgeName.append( startNode.nodeName );
	edgeId = qHash( edgeName );
	matrixGraph->addEdge( edgeId, edgeName, midGraphNode, startGraphNode, iEdgeType, false );

	edgeName = midNode.nodeName;
	edgeName.append( endNode.nodeName );
	edgeId = qHash( edgeName );
	matrixGraph->addEdge( edgeId, edgeName, midGraphNode, endGraphNode, iEdgeType, false );

	return matrixGraph;
}

//osg::Vec2f SpecialMatrix::FileParser::getAvailablePosition( SpecialMatrix::NodeConnections* connections, float row, float column )
osg::Vec2f SpecialMatrix::FileParser::getAvailablePosition( SpecialMatrix::NodeConnections* connections, int row, int column )
{
	//int newRow = static_cast<int>( row ), newCol = static_cast<int>( column );
	int newRow =  row , newCol =  column ;
	int cornerRow = newRow, cornerCol = newCol;
	int distance = 1, phase = 0;
	int nodeCount = ( int ) connections->getNodePositionsArray().size();
	osg::Vec2f nodePos;
	while ( true ) {
		//if the position is taken
		if ( connections->getNodePositionsArrayField( newRow, newCol ) ) {
			switch ( phase ) {
				case 0:	//right
					cornerCol += distance;
					if ( cornerCol > static_cast<int>( nodeCount ) ) {
						++phase;
						break;
					}
					while ( newCol != cornerCol ) {
						++newCol;
						if ( !connections->getNodePositionsArrayField( newRow, newCol ) ) {
							break;
						}
					}
					++phase;
					break;
				case 1:	//down
					cornerRow += distance;
					if ( cornerRow > static_cast<int>( nodeCount ) ) {
						++distance;
						++phase;
						break;
					}
					while ( newRow != cornerRow ) {
						++newRow;
						if ( !connections->getNodePositionsArrayField( newRow, newCol ) ) {
							break;
						}
					}
					++distance;
					++phase;
					break;
				case 2:	//left
					cornerCol -= distance;
					if ( cornerCol < 0 ) {
						++phase;
						break;
					}
					while ( newCol != cornerCol ) {
						--newCol;
						if ( !connections->getNodePositionsArrayField( newRow, newCol ) ) {
							break;
						}
					}
					++phase;
					break;
				case 3:	//up
					cornerRow -= distance;
					if ( cornerRow < 0 ) {
						++distance;
						++phase;
						break;
					}
					while ( newRow != cornerRow ) {
						--newRow;
						if ( !connections->getNodePositionsArrayField( newRow, newCol ) ) {
							break;
						}
					}
					++distance;
					++phase;
					break;
				default:
					break;
			}
			phase %= 4;
		}
		else {
			//is available
			nodePos.x() = static_cast<float>( newRow );
			nodePos.y() = static_cast<float>( newCol );
			return nodePos;
		}
	}
	return nodePos;
}

void SpecialMatrix::FileParser::createSettings( Data::Graph* matrixGraph )
{
	QMap<QString, QString>* settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "0" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.SquareNode" ) );
	eNodeType = matrixGraph->addType( "eNodeType", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "0" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.SquareNode" ) );
	nNodeType = matrixGraph->addType( "nNodeType", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.SquareNode" ) );
	iFullNodeType = matrixGraph->addType( "iFullNodeType", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "0.25" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.SquareNode" ) );
	iHalfNodeType = matrixGraph->addType( "iHalfNodeType", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultEdgeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );
	iEdgeType = matrixGraph->addType( "iEdgeType", settings );
}

