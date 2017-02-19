#include "SpecialMatrix/MatrixViewer.h"

#include "osg/LineWidth"
#include "Importer/GraphOperations.h"

#include <QDebug>

#pragma warning(push)
#pragma warning(disable:4244)

SpecialMatrix::MatrixViewer::MatrixViewer( Data::Graph* matrixGraph, QString fileName )
{
	this->matrixGraph = matrixGraph;
	this->connections = new SpecialMatrix::NodeConnections();

	fileParser = new SpecialMatrix::FileParser( fileName, matrixGraph, connections );

	createAxis();
	connections->setNodePositionsArrayField( 0, 0, true );

	adjustPositions();

	//printout nodePositionsArray
	/*for(size_t i=0; i<connections->getNodePositionsArray().size(); i++) {
		for(size_t j=0; j<connections->getNodePositionsArray().size(); j++) {
			if(connections->getNodePositionsArrayField(i, j))
				printf("1");
			else
				printf("0");
		}
		printf("\n");
	}*/

	//printout connectedNodes
	/*QMapIterator<qlonglong, QList<qlonglong>* > i(*connections->getConnectedNodes());

	while ( i.hasNext() ) {
		i.next();
		QList<qlonglong>* list = i.value();
		qDebug() << i.key() << "size: " << list->size() << " list has these connected nodes: ";

		for(int j=0; j<list->size(); j++) {
			qDebug() << list->at(j);
		}
	}*/

}

SpecialMatrix::MatrixViewer::~MatrixViewer()
{
	delete connections;
	delete fileParser;
}

void SpecialMatrix::MatrixViewer::exchangeNodes( osg::ref_ptr<Data::Node> srcNode, osg::ref_ptr<Data::Node> desNode )
{
	if ( srcNode->Data::AbsNode::getName().contains( 'x' ) && desNode->Data::AbsNode::getName().contains( 'y' ) ) {
		return;
	}
	if ( srcNode->Data::AbsNode::getName().contains( 'y' ) && desNode->Data::AbsNode::getName().contains( 'x' ) ) {
		return;
	}

	int separator = Util::ApplicationConfig::get()->getValue( "Viewer.Display.MatrixNodeSeparator" ).toInt();
	osg::ref_ptr<Data::Node> tempNode, foundNode;
	osg::Vec3f diffVector, finalPosVector;
	osg::Vec2f iNodeOldPos, iNodeNewPos;
	qlonglong foundNodeId;
	int srcNodePos, desNodePos, diff;

	if ( srcNode->Data::AbsNode::getName().contains( 'x' ) ) {
		srcNodePos = connections->getXAxisNodes()->indexOf( srcNode->getId() )+1;
		desNodePos = connections->getXAxisNodes()->indexOf( desNode->getId() )+1;
	}
	else {
		srcNodePos = connections->getYAxisNodes()->indexOf( srcNode->getId() )+1;
		desNodePos = connections->getYAxisNodes()->indexOf( desNode->getId() )+1;
	}

	if ( desNodePos < srcNodePos ) {
		tempNode = desNode;
		desNode = srcNode;
		srcNode = tempNode;
	}

	diff = desNodePos - srcNodePos;

	//srcNode, desNode are axisNodes
	if ( srcNode->Data::AbsNode::getName().contains( 'x' ) ) {
		srcNodePos = connections->getXAxisNodes()->indexOf( srcNode->getId() )+1;
		desNodePos = connections->getXAxisNodes()->indexOf( desNode->getId() )+1;
		connections->getXAxisNodes()->swap( srcNodePos, desNodePos );
		diffVector = osg::Vec3f( static_cast<float>( diff*separator ), 0.0f, 0.0f );
	}
	else {
		srcNodePos = connections->getYAxisNodes()->indexOf( srcNode->getId() )+1;
		desNodePos = connections->getYAxisNodes()->indexOf( desNode->getId() )+1;
		connections->getYAxisNodes()->swap( srcNodePos, desNodePos );
		diffVector = osg::Vec3f( 0.0f, static_cast<float>( diff*separator ), 0.0f );
	}

	//Src +diff
	QList<qlonglong>* connToSrcNodes = connections->getConnectedNodes()->value( srcNode->getId() );
	for ( int i=0; i<connToSrcNodes->size(); ++i ) {
		//get the nodes connected to scrNode
		tempNode = matrixGraph->findNodeById( connToSrcNodes->indexOf( i ) );
		//get the old position of the iNode, and delete from the positionArray
		iNodeOldPos.set( tempNode->getTargetPosition().x()/static_cast<float>( separator ), tempNode->getTargetPosition().y()/static_cast<float>( separator ) );
		connections->setNodePositionsArrayField( static_cast<int>( iNodeOldPos.x() ), static_cast<int>( iNodeOldPos.y() ), 0 );
		//get the new position
		finalPosVector = tempNode->getTargetPosition() + diffVector;
		//iNodeNewPos.set( static_cast<int>( finalPosVector.x()/static_cast<float>(separator)) , static_cast<int>( finalPosVector.y()/static_cast<float>(separator))  );//zaokruhlenie??
		iNodeNewPos.set( static_cast<float>( static_cast<int>( finalPosVector.x()/static_cast<float>( separator ) ) ) , static_cast<float>( static_cast<int>( finalPosVector.y()/static_cast<float>( separator ) ) ) );

		//CHECK AVAIBILITY
		foundNodeId = connections->getNodePositionsArrayField( static_cast<int>( iNodeNewPos.x() ), static_cast<int>( iNodeNewPos.y() ) );
		if ( foundNodeId ) {
			foundNode = matrixGraph->findNodeById( foundNodeId );
			osg::Vec2f foundNodePos = fileParser->getAvailablePosition( connections, static_cast<int>( iNodeNewPos.x() ), static_cast<int>( iNodeNewPos.y() ) );
			foundNode->setTargetPosition( osg::Vec3f( foundNodePos.x()*static_cast<float>( separator ),  foundNodePos.y()*static_cast<float>( separator ), 0.0f ) );
			connections->setNodePositionsArrayField( static_cast<int>( foundNodePos.x() ), static_cast<int>( foundNodePos.y() ), foundNodeId );
		}

		tempNode->setTargetPosition( finalPosVector );
		tempNode->setRestrictedTargetPosition( tempNode->getTargetPosition() );
		connections->setNodePositionsArrayField( static_cast<int>( iNodeNewPos.x() ), static_cast<int>( iNodeNewPos.y() ), tempNode->getId() );
	}
	srcNode->setTargetPosition( srcNode->getTargetPosition() + diffVector );
	srcNode->setRestrictedTargetPosition( srcNode->getTargetPosition() );
	if ( srcNode->Data::AbsNode::getName().contains( 'x' ) ) {
		connections->setNodePositionsArrayField( desNodePos, 0, srcNode->getId() );
	}
	else {
		connections->setNodePositionsArrayField( 0, desNodePos, srcNode->getId() );
	}

	//Des -diff
	QList<qlonglong>* connToDesNodes = connections->getConnectedNodes()->value( desNode->getId() );
	for ( int i=0; i<connToDesNodes->size(); ++i ) {
		//get the nodes connected to scrNode
		tempNode = matrixGraph->findNodeById( connToDesNodes->indexOf( i ) );
		//get the old position of the iNode, and delete from the positionArray
		iNodeOldPos.set( static_cast<float>( static_cast<int>( tempNode->getTargetPosition().x() )/separator ),static_cast<float>( static_cast<int>( tempNode->getTargetPosition().y() )/separator ) );
		connections->setNodePositionsArrayField( static_cast<int>( iNodeOldPos.x() ), static_cast<int>( iNodeOldPos.y() ), 0 );
		//get the new position
		finalPosVector = tempNode->getTargetPosition() - diffVector;
		iNodeNewPos.set( static_cast<float>( static_cast<int>( finalPosVector.x()/static_cast<float>( separator ) ) ), static_cast<float>( static_cast<int>( finalPosVector.y()/static_cast<float>( separator ) ) ) );

		//CHECK AVAIBILITY
		foundNodeId = connections->getNodePositionsArrayField( static_cast<int>( iNodeNewPos.x() ), static_cast<int>( iNodeNewPos.y() ) );
		if ( foundNodeId ) {
			foundNode = matrixGraph->findNodeById( foundNodeId );
			osg::Vec2f foundNodePos = fileParser->getAvailablePosition( connections, static_cast<int>( iNodeNewPos.x() ), static_cast<int>( iNodeNewPos.y() ) );
			foundNode->setTargetPosition( osg::Vec3f( foundNodePos.x()*static_cast<float>( separator ),  foundNodePos.y()*static_cast<float>( separator ), 0.0f ) );
			connections->setNodePositionsArrayField( static_cast<int>( foundNodePos.x() ), static_cast<int>( foundNodePos.y() ), foundNodeId );
		}

		tempNode->setTargetPosition( finalPosVector );
		tempNode->setRestrictedTargetPosition( tempNode->getTargetPosition() );
		connections->setNodePositionsArrayField( static_cast<int>( iNodeNewPos.x() ), static_cast<int>( iNodeNewPos.y() ), tempNode->getId() );
	}
	desNode->setTargetPosition( desNode->getTargetPosition() - diffVector );
	desNode->setRestrictedTargetPosition( desNode->getTargetPosition() );
	if ( desNode->Data::AbsNode::getName().contains( 'x' ) ) {
		connections->setNodePositionsArrayField( srcNodePos, 0, desNode->getId() );
	}
	else {
		connections->setNodePositionsArrayField( 0, srcNodePos, desNode->getId() );
	}
}

void SpecialMatrix::MatrixViewer::createAxis()
{
	float separator = Util::ApplicationConfig::get()->getValue( "Viewer.Display.MatrixNodeSeparator" ).toFloat();
	int xNodesCount = connections->getXAxisNodes()->count()+1;
	int yNodesCount = connections->getYAxisNodes()->count()+1;

	QMap<QString, QString>* settings = new QMap<QString, QString>;
	settings->insert( "color.R", "0" );
	settings->insert( "color.G", "0" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
	Data::Type* axisNodeType = matrixGraph->addType( "axisNodeType", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "0" );
	settings->insert( "color.G", "0" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultEdgeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );
	Data::Type* axisEdgeType = matrixGraph->addType( "axisEdgeType", settings );

	osg::ref_ptr<Data::Node> center = matrixGraph->addNode( qHash( "Center" ), "Center", axisNodeType, osg::Vec3f( 0.0f, 0.0f, 0.0f ) );
	osg::ref_ptr<Data::Node> xAxis = matrixGraph->addNode( qHash( "xAxis" ), "xAxis", axisNodeType, osg::Vec3f( static_cast<float>( xNodesCount )*separator, 0.0f, 0.0f ) );
	osg::ref_ptr<Data::Node> yAxis = matrixGraph->addNode( qHash( "yAxis" ), "yAxis", axisNodeType, osg::Vec3f( 0.0f, static_cast<float>( yNodesCount )*separator, 0.0f ) );

	matrixGraph->addEdge( qHash( "xAxisEdge" ), "xAxisEdge", center, xAxis, axisEdgeType, false );
	matrixGraph->addEdge( qHash( "yAxisEdge" ), "yAxisEdge", center, yAxis, axisEdgeType, false );

}

void SpecialMatrix::MatrixViewer::adjustPositions()
{
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = matrixGraph->getNodes();

	QMapIterator<qlonglong, osg::ref_ptr<Data::Node> > i( *allNodes );

	while ( i.hasNext() ) {
		i.next();
		osg::Vec3f targetPos = i.value()->getTargetPosition();

		//[NOTE] by default:
		//addNode
		//this->mTargetPosition = position;
		//this->currentPosition = position * Util::ApplicationConfig::get()->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();

		i.value()->setRestrictedTargetPosition( targetPos );
	}
}
