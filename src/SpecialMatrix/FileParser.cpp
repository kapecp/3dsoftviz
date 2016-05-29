#include "SpecialMatrix/FileParser.h"

SpecialMatrix::FileParser::FileParser( QString fileName, Data::Graph* matrixGraph, SpecialMatrix::NodeConnections* connections )
{
	createSettings(matrixGraph);
	parseFile(matrixGraph, fileName, connections);
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
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(0, "error", file.errorString());
	}

	QTextStream in(&file);

	while(!in.atEnd()) {
		QString line = in.readLine();
		QStringList fields = line.split('\t');
		//qDebug() << "line: "<< fields.at(0) << ", " << fields.at(1) << ", " << fields.at(2);

		if((fields.at(0).contains(QRegExp("[EN]")) && fields.at(1).contains('I') && fields.at(2).contains(QRegExp("[EN]"))))
			matrixGraph = addNormalNodesToGraph(matrixGraph, fields, connections, nodeCounter);
		else
			matrixGraph = addSpecialNodesToGraph(matrixGraph, fields, connections, nodeCounter);

	}
	file.close();
}

Data::Graph* SpecialMatrix::FileParser::addNormalNodesToGraph( Data::Graph* matrixGraph, QStringList fields, SpecialMatrix::NodeConnections* connections, int &nodeCounter )
{		
	//cases: E - I - N, N - I - E, E - I - E, N - I - N
	int separator = Util::ApplicationConfig::get()->getValue( "Viewer.Display.MatrixNodeSeparator" ).toInt();
	NodeTrio nodeTrio;
	osg::Vec3f nodePos;
	osg::ref_ptr<Data::Node> foundNode;
	qlonglong foundNodeId;

	QList<qlonglong>* xAxisNodes = connections->getXAxisNodes();
	QList<qlonglong>* yAxisNodes = connections->getYAxisNodes();
	QList<qlonglong>* iNodes = connections->getINodes();
	QMap<qlonglong, QList<qlonglong>* >* connectedNodes = connections->getConnectedNodes();
	QList<qlonglong>* tempList;

	//=====================start node (E||N)=====================
	//------------x axis------------
	nodeTrio.frstNode.nodeNames.xName = fields.at(0);
	nodeTrio.frstNode.nodeNames.xName = nodeTrio.frstNode.nodeNames.xName.append('x');
	nodeTrio.frstNode.nodeIds.xId = qHash(nodeTrio.frstNode.nodeNames.xName);

	//insert node into xAxisNodes list only if missing
	if(!xAxisNodes->contains(nodeTrio.frstNode.nodeIds.xId))
	{				
		connections->setNodePositionsArray(++nodeCounter);
		xAxisNodes->push_back(nodeTrio.frstNode.nodeIds.xId);
		nodeTrio.frstNode.nodePos.xPos = xAxisNodes->indexOf(nodeTrio.frstNode.nodeIds.xId)+1;		
		nodePos = osg::Vec3f((float)nodeTrio.frstNode.nodePos.xPos*separator, 0.0f, 0.0f);
		if(nodeTrio.frstNode.nodeNames.xName.contains('E'))
			matrixGraph->addNode( nodeTrio.frstNode.nodeIds.xId, nodeTrio.frstNode.nodeNames.xName, eNodeType, nodePos );
		if(nodeTrio.frstNode.nodeNames.xName.contains('N'))
			matrixGraph->addNode( nodeTrio.frstNode.nodeIds.xId, nodeTrio.frstNode.nodeNames.xName, nNodeType, nodePos );
		connections->setNodePositionsArrayField(nodeTrio.frstNode.nodePos.xPos, 0, nodeTrio.frstNode.nodeIds.xId);
	}
	//get list from Qmap where key is xFrstNodeId and insert I node into connectedNodes list
	tempList = connectedNodes->value(nodeTrio.frstNode.nodeIds.xId);
	if(tempList == nullptr)
		tempList = new QList<qlonglong>;
	tempList->push_back(getHashIdFromNodeName(fields.at(1), 'x'));
	connectedNodes->insert(nodeTrio.frstNode.nodeIds.xId, tempList);

	//------------y axis------------
	nodeTrio.frstNode.nodeNames.yName = fields.at(0);
	nodeTrio.frstNode.nodeNames.yName = nodeTrio.frstNode.nodeNames.yName.append('y');
	nodeTrio.frstNode.nodeIds.yId = qHash(nodeTrio.frstNode.nodeNames.yName);

	//insert node into yAxisNodes list only if missing
	if(!yAxisNodes->contains(nodeTrio.frstNode.nodeIds.yId))
	{
		//connections->setNodePositionsArray(++nodeCounter);
		yAxisNodes->push_back(nodeTrio.frstNode.nodeIds.yId);
		nodeTrio.frstNode.nodePos.yPos = yAxisNodes->indexOf(nodeTrio.frstNode.nodeIds.yId)+1;
		nodePos = osg::Vec3f(0.0f, (float)nodeTrio.frstNode.nodePos.yPos*separator, 0.0f);
		if(nodeTrio.frstNode.nodeNames.yName.contains('E'))
			matrixGraph->addNode( nodeTrio.frstNode.nodeIds.yId, nodeTrio.frstNode.nodeNames.yName, eNodeType, nodePos );
		if(nodeTrio.frstNode.nodeNames.yName.contains('N'))
			matrixGraph->addNode( nodeTrio.frstNode.nodeIds.yId, nodeTrio.frstNode.nodeNames.yName, nNodeType, nodePos );
		connections->setNodePositionsArrayField(0, nodeTrio.frstNode.nodePos.yPos, nodeTrio.frstNode.nodeIds.yId);
	}
	//get list from Qmap where key is yFrstNodeId and insert I node into connectedNodes list
	tempList = connectedNodes->value(nodeTrio.frstNode.nodeIds.yId);
	if(tempList == nullptr)
		tempList = new QList<qlonglong>;
	tempList->push_back(getHashIdFromNodeName(fields.at(1), 'y'));
	connectedNodes->insert(nodeTrio.frstNode.nodeIds.yId, tempList);

	//=====================end node (N||E)=====================
	//------------x axis------------
	nodeTrio.trdNode.nodeNames.xName = fields.at(2);
	nodeTrio.trdNode.nodeNames.xName = nodeTrio.trdNode.nodeNames.xName.append('x');
	nodeTrio.trdNode.nodeIds.xId = qHash(nodeTrio.trdNode.nodeNames.xName);

	//insert node into xAxisNodes list only if missing
	if(!xAxisNodes->contains(nodeTrio.trdNode.nodeIds.xId))
	{		
		connections->setNodePositionsArray(++nodeCounter);
		xAxisNodes->push_back(nodeTrio.trdNode.nodeIds.xId);
		nodeTrio.trdNode.nodePos.xPos = xAxisNodes->indexOf(nodeTrio.trdNode.nodeIds.xId)+1;
		nodePos = osg::Vec3f((float)nodeTrio.trdNode.nodePos.xPos*separator, 0.0f, 0.0f);
		if(nodeTrio.trdNode.nodeNames.xName.contains('N'))
			matrixGraph->addNode( nodeTrio.trdNode.nodeIds.xId, nodeTrio.trdNode.nodeNames.xName, nNodeType, nodePos );
		if(nodeTrio.trdNode.nodeNames.xName.contains('E'))
			matrixGraph->addNode( nodeTrio.trdNode.nodeIds.xId, nodeTrio.trdNode.nodeNames.xName, eNodeType, nodePos );
		connections->setNodePositionsArrayField(nodeTrio.trdNode.nodePos.xPos, 0, nodeTrio.trdNode.nodeIds.xId);
	}
	//get list from Qmap where key is xTrdNodeId and insert I node into connectedNodes list
	tempList = connectedNodes->value(nodeTrio.trdNode.nodeIds.xId);
	if(tempList == nullptr)
		tempList = new QList<qlonglong>;
	tempList->push_back(getHashIdFromNodeName(fields.at(1), 'x'));
	connectedNodes->insert(nodeTrio.trdNode.nodeIds.xId, tempList);

	//------------y axis------------
	nodeTrio.trdNode.nodeNames.yName = fields.at(2);
	nodeTrio.trdNode.nodeNames.yName = nodeTrio.trdNode.nodeNames.yName.append('y');
	nodeTrio.trdNode.nodeIds.yId = qHash(nodeTrio.trdNode.nodeNames.yName);

	//insert node into yAxisNodes list only if missing
	if(!yAxisNodes->contains(nodeTrio.trdNode.nodeIds.yId))
	{		
		//connections->setNodePositionsArray(++nodeCounter);
		yAxisNodes->push_back(nodeTrio.trdNode.nodeIds.yId);
		nodeTrio.trdNode.nodePos.yPos = yAxisNodes->indexOf(nodeTrio.trdNode.nodeIds.yId)+1;
		nodePos = osg::Vec3f(0.0f, (float)nodeTrio.trdNode.nodePos.yPos*separator, 0.0f);
		if(nodeTrio.trdNode.nodeNames.yName.contains('N'))
			matrixGraph->addNode( nodeTrio.trdNode.nodeIds.yId, nodeTrio.trdNode.nodeNames.yName, nNodeType, nodePos );
		if(nodeTrio.trdNode.nodeNames.yName.contains('E'))
			matrixGraph->addNode( nodeTrio.trdNode.nodeIds.yId, nodeTrio.trdNode.nodeNames.yName, eNodeType, nodePos );
		connections->setNodePositionsArrayField(0, nodeTrio.trdNode.nodePos.yPos, nodeTrio.trdNode.nodeIds.yId);
	}
	//get list from Qmap where key is yTrdNodeId and insert I node into connectedNodes list
	tempList = connectedNodes->value(nodeTrio.trdNode.nodeIds.yId);
	if(tempList == nullptr)
		tempList = new QList<qlonglong>;
	tempList->push_back(getHashIdFromNodeName(fields.at(1), 'y'));
	connectedNodes->insert(nodeTrio.trdNode.nodeIds.yId, tempList);

	//=====================middle node (I)=====================
	//------------x axis------------
	nodeTrio.secNode.nodeNames.xName = fields.at(1);
	nodeTrio.secNode.nodeNames.xName = nodeTrio.secNode.nodeNames.xName.append('x');
	nodeTrio.secNode.nodeIds.xId = qHash(nodeTrio.secNode.nodeNames.xName);

	//insert node into iNodes list only if missing
	if(!iNodes->contains(nodeTrio.secNode.nodeIds.xId))
	{
		//connections->setNodePositionsArray(++nodeCounter);
		iNodes->push_back(nodeTrio.secNode.nodeIds.xId);
		nodeTrio.secNode.nodePos.xPos = xAxisNodes->indexOf(nodeTrio.frstNode.nodeIds.xId)+1;
		nodeTrio.secNode.nodePos.yPos = yAxisNodes->indexOf(nodeTrio.trdNode.nodeIds.yId)+1;
		//CHECK AVAIBILITY
		foundNodeId = connections->getNodePositionsArrayField(nodeTrio.secNode.nodePos.xPos, nodeTrio.secNode.nodePos.yPos);
		if(foundNodeId) {
			foundNode = matrixGraph->findNodeById(foundNodeId);
			NodePos foundNodePos = getAvailablePosition(connections, nodeTrio.secNode.nodePos.xPos, nodeTrio.secNode.nodePos.yPos);
			foundNode->setTargetPosition(osg::Vec3f((float)foundNodePos.xPos*separator, (float)foundNodePos.yPos*separator, 0.0f));
			connections->setNodePositionsArrayField(foundNodePos.xPos, foundNodePos.yPos, foundNodeId);
		}
		nodePos = osg::Vec3f((float)nodeTrio.secNode.nodePos.xPos*separator, (float)nodeTrio.secNode.nodePos.yPos*separator, 0.0f);
		matrixGraph->addNode( nodeTrio.secNode.nodeIds.xId, nodeTrio.secNode.nodeNames.xName, iFullNodeType, nodePos );
		connections->setNodePositionsArrayField(nodeTrio.secNode.nodePos.xPos, nodeTrio.secNode.nodePos.yPos, nodeTrio.secNode.nodeIds.xId);
	} //if there is check for iHalfNodeType
	//
	//
	//

	//------------y axis------------
	nodeTrio.secNode.nodeNames.yName = fields.at(1);
	nodeTrio.secNode.nodeNames.yName = nodeTrio.secNode.nodeNames.yName.append('y');
	nodeTrio.secNode.nodeIds.yId = qHash(nodeTrio.secNode.nodeNames.yName);

	//insert node into iNodes list only if missing
	if(!iNodes->contains(nodeTrio.secNode.nodeIds.yId))
	{
		//connections->setNodePositionsArray(++nodeCounter);
		iNodes->push_back(nodeTrio.secNode.nodeIds.yId);
		nodeTrio.secNode.nodePos.xPos = xAxisNodes->indexOf(nodeTrio.trdNode.nodeIds.xId)+1;
		nodeTrio.secNode.nodePos.yPos = yAxisNodes->indexOf(nodeTrio.frstNode.nodeIds.yId)+1;
		//CHECK AVAIBILITY
		foundNodeId = connections->getNodePositionsArrayField(nodeTrio.secNode.nodePos.xPos, nodeTrio.secNode.nodePos.yPos);
		if(foundNodeId) {
			foundNode = matrixGraph->findNodeById(foundNodeId);
			NodePos foundNodePos = getAvailablePosition(connections, nodeTrio.secNode.nodePos.xPos, nodeTrio.secNode.nodePos.yPos);
			foundNode->setTargetPosition(osg::Vec3f((float)foundNodePos.xPos*separator, (float)foundNodePos.yPos*separator, 0.0f));
			connections->setNodePositionsArrayField(foundNodePos.xPos, foundNodePos.yPos, foundNodeId);
		}

		nodePos = osg::Vec3f((float)nodeTrio.secNode.nodePos.xPos*separator, (float)nodeTrio.secNode.nodePos.yPos*separator, 0.0f);
		matrixGraph->addNode( nodeTrio.secNode.nodeIds.yId, nodeTrio.secNode.nodeNames.yName, iFullNodeType, nodePos );
		connections->setNodePositionsArrayField(nodeTrio.secNode.nodePos.xPos, nodeTrio.secNode.nodePos.yPos, nodeTrio.secNode.nodeIds.yId);
	} //if there is check for iHalfNodeType
	//
	//
	//

	//qDebug() << "nodeCounter = " << nodeCounter;
	return matrixGraph;
}

Data::Graph* SpecialMatrix::FileParser::addSpecialNodesToGraph( Data::Graph* matrixGraph, QStringList fields, SpecialMatrix::NodeConnections* connections, int &nodeCounter )
{
	//cases: E - I - I, N - I - I, I - I - E, I - I - N
	int separator = Util::ApplicationConfig::get()->getValue( "Viewer.Display.MatrixNodeSeparator" ).toInt();
	NodeTrio nodeTrio;
	osg::Vec3f nodePos;
	QString edgeName;
	qlonglong edgeId;
	osg::ref_ptr<Data::Node> srcNode, midNode, desNode;

	QList<qlonglong>* xAxisNodes = connections->getXAxisNodes();
	QList<qlonglong>* yAxisNodes = connections->getYAxisNodes();
	QList<qlonglong>* iNodes = connections->getINodes();
	QMap<qlonglong, QList<qlonglong>* >* connectedNodes = connections->getConnectedNodes();
	QList<qlonglong>* tempList;

	//=====================start node (E||N||I)=====================
	//------------x axis------------
	nodeTrio.frstNode.nodeNames.xName = fields.at(0);
	nodeTrio.frstNode.nodeNames.xName = nodeTrio.frstNode.nodeNames.xName.append('x');
	nodeTrio.frstNode.nodeIds.xId = qHash(nodeTrio.frstNode.nodeNames.xName);

	if(fields.at(0).contains(QRegExp("[EN]"))) {
		//insert node into xAxisNodes list only if missing
		if(!xAxisNodes->contains(nodeTrio.frstNode.nodeIds.xId))
		{
			connections->setNodePositionsArray(++nodeCounter);
			xAxisNodes->push_back(nodeTrio.frstNode.nodeIds.xId);
			nodeTrio.frstNode.nodePos.xPos = xAxisNodes->indexOf(nodeTrio.frstNode.nodeIds.xId)+1;
			nodePos = osg::Vec3f((float)nodeTrio.frstNode.nodePos.xPos*separator, 0.0f, 0.0f);
			if(nodeTrio.frstNode.nodeNames.xName.contains('E'))
				matrixGraph->addNode( nodeTrio.frstNode.nodeIds.xId, nodeTrio.frstNode.nodeNames.xName, eNodeType, nodePos );
			if(nodeTrio.frstNode.nodeNames.xName.contains('N'))
				matrixGraph->addNode( nodeTrio.frstNode.nodeIds.xId, nodeTrio.frstNode.nodeNames.xName, nNodeType, nodePos );
			connections->setNodePositionsArrayField(nodeTrio.frstNode.nodePos.xPos, 0, true);
		}		
	}

	//------------y axis------------
	nodeTrio.frstNode.nodeNames.yName = fields.at(0);
	nodeTrio.frstNode.nodeNames.yName = nodeTrio.frstNode.nodeNames.yName.append('y');
	nodeTrio.frstNode.nodeIds.yId = qHash(nodeTrio.frstNode.nodeNames.yName);

	if(fields.at(0).contains(QRegExp("[EN]"))) {
		//insert node into yAxisNodes list only if missing
		if(!yAxisNodes->contains(nodeTrio.frstNode.nodeIds.yId))
		{
			connections->setNodePositionsArray(++nodeCounter);
			yAxisNodes->push_back(nodeTrio.frstNode.nodeIds.yId);
			nodeTrio.frstNode.nodePos.yPos = yAxisNodes->indexOf(nodeTrio.frstNode.nodeIds.yId)+1;
			nodePos = osg::Vec3f(0.0f, (float)nodeTrio.frstNode.nodePos.yPos*separator, 0.0f);
			if(nodeTrio.frstNode.nodeNames.yName.contains('E'))
				matrixGraph->addNode( nodeTrio.frstNode.nodeIds.yId, nodeTrio.frstNode.nodeNames.yName, eNodeType, nodePos );
			if(nodeTrio.frstNode.nodeNames.yName.contains('N'))
				matrixGraph->addNode( nodeTrio.frstNode.nodeIds.yId, nodeTrio.frstNode.nodeNames.yName, nNodeType, nodePos );
			connections->setNodePositionsArrayField(0, nodeTrio.frstNode.nodePos.yPos, true);
		}
	}

	//=====================end node (E||N||I)=====================
	//------------x axis------------
	nodeTrio.trdNode.nodeNames.xName = fields.at(2);
	nodeTrio.trdNode.nodeNames.xName = nodeTrio.trdNode.nodeNames.xName.append('x');
	nodeTrio.trdNode.nodeIds.xId = qHash(nodeTrio.trdNode.nodeNames.xName);

	if(fields.at(2).contains(QRegExp("[EN]"))) {
		//insert node into xAxisNodes list only if missing
		if(!xAxisNodes->contains(nodeTrio.trdNode.nodeIds.xId))
		{
			connections->setNodePositionsArray(++nodeCounter);
			xAxisNodes->push_back(nodeTrio.trdNode.nodeIds.xId);
			nodeTrio.trdNode.nodePos.xPos = xAxisNodes->indexOf(nodeTrio.trdNode.nodeIds.xId)+1;
			nodePos = osg::Vec3f((float)nodeTrio.trdNode.nodePos.xPos*separator, 0.0f, 0.0f);
			if(nodeTrio.trdNode.nodeNames.xName.contains('E'))
				matrixGraph->addNode( nodeTrio.trdNode.nodeIds.xId, nodeTrio.trdNode.nodeNames.xName, eNodeType, nodePos );
			if(nodeTrio.trdNode.nodeNames.xName.contains('N'))
				matrixGraph->addNode( nodeTrio.trdNode.nodeIds.xId, nodeTrio.trdNode.nodeNames.xName, nNodeType, nodePos );
			connections->setNodePositionsArrayField(nodeTrio.trdNode.nodePos.xPos, 0, true);
		}		

		//------------x axis------------
		if(fields.at(0).contains('I')) {
			//insert node into iNodes list only if missing
			if(!iNodes->contains(nodeTrio.frstNode.nodeIds.xId))
			{
				connections->setNodePositionsArray(++nodeCounter);
				iNodes->push_back(nodeTrio.frstNode.nodeIds.xId);
				//getPosition
				nodeTrio.frstNode.nodePos.xPos = xAxisNodes->indexOf(nodeTrio.trdNode.nodeIds.xId)+1;
				nodeTrio.frstNode.nodePos.yPos = (nodeTrio.frstNode.nodePos.xPos) /3;
				nodeTrio.frstNode.nodePos = getAvailablePosition(connections, nodeTrio.frstNode.nodePos.xPos, nodeTrio.frstNode.nodePos.yPos);
				nodePos = osg::Vec3f((float)nodeTrio.frstNode.nodePos.xPos*separator, (float)nodeTrio.frstNode.nodePos.yPos*separator, 0.0f);
				matrixGraph->addNode( nodeTrio.frstNode.nodeIds.xId, nodeTrio.frstNode.nodeNames.xName, iHalfNodeType, nodePos );
				connections->setNodePositionsArrayField(nodeTrio.frstNode.nodePos.xPos, nodeTrio.frstNode.nodePos.yPos, nodeTrio.frstNode.nodeIds.xId);
			} //if there is check for iHalfNodeType
			//
			//
			//
		}
	}
	if(fields.at(2).contains('I')) {
		if(fields.at(0).contains(QRegExp("[EN]"))) {
			//insert node into iNodes list only if missing
			if(!iNodes->contains(nodeTrio.trdNode.nodeIds.xId))
			{
				connections->setNodePositionsArray(++nodeCounter);
				iNodes->push_back(nodeTrio.trdNode.nodeIds.xId);
				//getPosition
				nodeTrio.trdNode.nodePos.xPos = xAxisNodes->indexOf(nodeTrio.frstNode.nodeIds.xId)+1;
				nodeTrio.trdNode.nodePos.yPos = (nodeTrio.trdNode.nodePos.xPos) /3;
				nodeTrio.trdNode.nodePos = getAvailablePosition(connections, nodeTrio.trdNode.nodePos.xPos, nodeTrio.trdNode.nodePos.yPos);

				nodePos = osg::Vec3f((float)nodeTrio.trdNode.nodePos.xPos*separator, (float)nodeTrio.trdNode.nodePos.yPos*separator, 0.0f);
				matrixGraph->addNode( nodeTrio.trdNode.nodeIds.xId, nodeTrio.trdNode.nodeNames.xName, iHalfNodeType, nodePos );
				connections->setNodePositionsArrayField(nodeTrio.trdNode.nodePos.xPos, nodeTrio.trdNode.nodePos.yPos, nodeTrio.trdNode.nodeIds.xId);
			} //if there is check for iHalfNodeType
			//
			//
			//
		}
	}

	//------------y axis------------
	nodeTrio.trdNode.nodeNames.yName = fields.at(2);
	nodeTrio.trdNode.nodeNames.yName = nodeTrio.trdNode.nodeNames.yName.append('y');
	nodeTrio.trdNode.nodeIds.yId = qHash(nodeTrio.trdNode.nodeNames.yName);

	if(fields.at(2).contains(QRegExp("[EN]"))) {
		//insert node into yAxisNodes list only if missing
		if(!yAxisNodes->contains(nodeTrio.trdNode.nodeIds.yId))
		{
			connections->setNodePositionsArray(++nodeCounter);
			yAxisNodes->push_back(nodeTrio.trdNode.nodeIds.yId);
			nodeTrio.trdNode.nodePos.yPos = yAxisNodes->indexOf(nodeTrio.trdNode.nodeIds.yId)+1;
			nodePos = osg::Vec3f(0.0f, (float)nodeTrio.trdNode.nodePos.yPos*separator, 0.0f);
			if(nodeTrio.trdNode.nodeNames.yName.contains('E'))
				matrixGraph->addNode( nodeTrio.trdNode.nodeIds.yId, nodeTrio.trdNode.nodeNames.yName, eNodeType, nodePos );
			if(nodeTrio.trdNode.nodeNames.yName.contains('N'))
				matrixGraph->addNode( nodeTrio.trdNode.nodeIds.yId, nodeTrio.trdNode.nodeNames.yName, nNodeType, nodePos );
			connections->setNodePositionsArrayField(0, nodeTrio.trdNode.nodePos.yPos, true);
		}		

		//------------y axis------------
		if(fields.at(0).contains('I')) {
			//insert node into iNodes list only if missing
			if(!iNodes->contains(nodeTrio.frstNode.nodeIds.yId))
			{
				connections->setNodePositionsArray(++nodeCounter);
				iNodes->push_back(nodeTrio.frstNode.nodeIds.yId);
				//getPosition
				nodeTrio.frstNode.nodePos.yPos = yAxisNodes->indexOf(nodeTrio.trdNode.nodeIds.yId)+1;
				nodeTrio.frstNode.nodePos.xPos = (nodeTrio.frstNode.nodePos.yPos) /3;
				nodeTrio.frstNode.nodePos = getAvailablePosition(connections, nodeTrio.frstNode.nodePos.xPos, nodeTrio.frstNode.nodePos.yPos);

				nodePos = osg::Vec3f((float)nodeTrio.frstNode.nodePos.xPos*separator, (float)nodeTrio.frstNode.nodePos.yPos*separator, 0.0f);
				matrixGraph->addNode( nodeTrio.frstNode.nodeIds.yId, nodeTrio.frstNode.nodeNames.yName, iHalfNodeType, nodePos );
				connections->setNodePositionsArrayField(nodeTrio.frstNode.nodePos.xPos, nodeTrio.frstNode.nodePos.yPos, nodeTrio.frstNode.nodeIds.yId);
			} //if there is check for iHalfNodeType
			//
			//
			//
		}
	}
	if(fields.at(2).contains('I')) {
		if(fields.at(0).contains(QRegExp("[EN]"))) {
		//insert node into iNodes list only if missing
			if(!iNodes->contains(nodeTrio.trdNode.nodeIds.yId))
			{
				connections->setNodePositionsArray(++nodeCounter);
				iNodes->push_back(nodeTrio.trdNode.nodeIds.yId);
				//getPosition
				nodeTrio.trdNode.nodePos.yPos = yAxisNodes->indexOf(nodeTrio.frstNode.nodeIds.yId)+1;
				nodeTrio.trdNode.nodePos.xPos = (nodeTrio.trdNode.nodePos.yPos) /3;
				nodeTrio.trdNode.nodePos = getAvailablePosition(connections, nodeTrio.trdNode.nodePos.xPos, nodeTrio.trdNode.nodePos.yPos);
				nodePos = osg::Vec3f((float)nodeTrio.trdNode.nodePos.xPos*separator, (float)nodeTrio.trdNode.nodePos.yPos*separator, 0.0f);
				matrixGraph->addNode( nodeTrio.trdNode.nodeIds.yId, nodeTrio.trdNode.nodeNames.yName, iHalfNodeType, nodePos );
				connections->setNodePositionsArrayField(nodeTrio.trdNode.nodePos.xPos, nodeTrio.trdNode.nodePos.yPos, nodeTrio.trdNode.nodeIds.yId);
			} //if there is check for iHalfNodeType
			//
			//
			//
		}
	}

	//=====================middle node (I)=====================
	//------------x axis------------
	nodeTrio.secNode.nodeNames.xName = fields.at(1);
	nodeTrio.secNode.nodeNames.xName = nodeTrio.secNode.nodeNames.xName.append('x');
	nodeTrio.secNode.nodeIds.xId = qHash(nodeTrio.secNode.nodeNames.xName);

	//insert node into iNodes list only if missing
	if(!iNodes->contains(nodeTrio.secNode.nodeIds.xId))
	{
		//connections->setNodePositionsArray(++nodeCounter);
		iNodes->push_back(nodeTrio.secNode.nodeIds.xId);
		//getPosition
		if(fields.at(0).contains('I')) {
			nodeTrio.secNode.nodePos.xPos = xAxisNodes->indexOf(nodeTrio.trdNode.nodeIds.xId)+1;
			nodeTrio.secNode.nodePos.yPos = (yAxisNodes->indexOf(nodeTrio.trdNode.nodeIds.yId)+1) / 2;
		}
		if(fields.at(2).contains('I')) {
			nodeTrio.secNode.nodePos.xPos = xAxisNodes->indexOf(nodeTrio.frstNode.nodeIds.xId)+1;
			nodeTrio.secNode.nodePos.yPos = (yAxisNodes->indexOf(nodeTrio.frstNode.nodeIds.yId)+1) / 2;
		}
		nodeTrio.secNode.nodePos = getAvailablePosition(connections, nodeTrio.secNode.nodePos.xPos, nodeTrio.secNode.nodePos.yPos);
		nodePos = osg::Vec3f((float)nodeTrio.secNode.nodePos.xPos*separator, (float)nodeTrio.secNode.nodePos.yPos*separator, 0.0f);
		midNode = matrixGraph->addNode( nodeTrio.secNode.nodeIds.xId, nodeTrio.secNode.nodeNames.xName, iHalfNodeType, nodePos );
		connections->setNodePositionsArrayField(nodeTrio.secNode.nodePos.xPos, nodeTrio.secNode.nodePos.yPos, nodeTrio.secNode.nodeIds.xId);
	} //if there is check for iHalfNodeType
	//
	//
	//

	srcNode = matrixGraph->findNodeById(nodeTrio.frstNode.nodeIds.xId);
	desNode = matrixGraph->findNodeById(nodeTrio.trdNode.nodeIds.xId);

	edgeName = nodeTrio.secNode.nodeNames.xName;
	edgeName.append(nodeTrio.frstNode.nodeNames.xName);
	edgeId = qHash(edgeName);
	matrixGraph->addEdge(edgeId, edgeName, midNode, srcNode, iEdgeType, false);

	edgeName = nodeTrio.secNode.nodeNames.xName;
	edgeName.append(nodeTrio.trdNode.nodeNames.xName);
	edgeId = qHash(edgeName);
	matrixGraph->addEdge(edgeId, edgeName, midNode, desNode, iEdgeType, false);

	//------------y axis------------
	nodeTrio.secNode.nodeNames.yName = fields.at(1);
	nodeTrio.secNode.nodeNames.yName = nodeTrio.secNode.nodeNames.yName.append('y');
	nodeTrio.secNode.nodeIds.yId = qHash(nodeTrio.secNode.nodeNames.yName);

	//insert node into iNodes list only if missing
	if(!iNodes->contains(nodeTrio.secNode.nodeIds.yId))
	{
		//connections->setNodePositionsArray(++nodeCounter);
		iNodes->push_back(nodeTrio.secNode.nodeIds.yId);
		//getPosition
		if(fields.at(0).contains('I')) {
			nodeTrio.secNode.nodePos.xPos = (xAxisNodes->indexOf(nodeTrio.trdNode.nodeIds.xId)+1) / 2;
			nodeTrio.secNode.nodePos.yPos = yAxisNodes->indexOf(nodeTrio.trdNode.nodeIds.yId)+1;
		}
		if(fields.at(2).contains('I')) {
			nodeTrio.secNode.nodePos.xPos = (xAxisNodes->indexOf(nodeTrio.frstNode.nodeIds.xId)+1) / 2;
			nodeTrio.secNode.nodePos.yPos = yAxisNodes->indexOf(nodeTrio.frstNode.nodeIds.yId)+1;
		}
		nodeTrio.secNode.nodePos = getAvailablePosition(connections, nodeTrio.secNode.nodePos.xPos, nodeTrio.secNode.nodePos.yPos);
		nodePos = osg::Vec3f((float)nodeTrio.secNode.nodePos.xPos*separator, (float)nodeTrio.secNode.nodePos.yPos*separator, 0.0f);
		midNode = matrixGraph->addNode( nodeTrio.secNode.nodeIds.yId, nodeTrio.secNode.nodeNames.yName, iHalfNodeType, nodePos );
		connections->setNodePositionsArrayField(nodeTrio.secNode.nodePos.xPos, nodeTrio.secNode.nodePos.yPos, nodeTrio.secNode.nodeIds.yId);
	} //if there is check for iHalfNodeType
	//
	//
	//

	srcNode = matrixGraph->findNodeById(nodeTrio.frstNode.nodeIds.yId);
	desNode = matrixGraph->findNodeById(nodeTrio.trdNode.nodeIds.yId);

	edgeName = nodeTrio.secNode.nodeNames.yName;
	edgeName.append(nodeTrio.frstNode.nodeNames.yName);
	edgeId = qHash(edgeName);
	matrixGraph->addEdge(edgeId, edgeName, midNode, srcNode, iEdgeType, false);

	edgeName = nodeTrio.secNode.nodeNames.yName;
	edgeName.append(nodeTrio.trdNode.nodeNames.yName);
	edgeId = qHash(edgeName);
	matrixGraph->addEdge(edgeId, edgeName, midNode, desNode, iEdgeType, false);

	//qDebug() << "nodeCounter = " << nodeCounter;
	return matrixGraph;
}

qlonglong SpecialMatrix::FileParser::getHashIdFromNodeName( QString nodeName, char axis )
{

	/*yNodeName = fields.at(1);
	yNodeName = yNodeName.append('y');
	ySecNodeId = qHash(yNodeName);*/

	QString name = nodeName.append(axis);
	return qHash(name);
}

SpecialMatrix::FileParser::NodePos SpecialMatrix::FileParser::getAvailablePosition( SpecialMatrix::NodeConnections* connections, int row, int column )
{
	int newRow = row, newCol = column;
	int cornerRow = row, cornerCol = column;
	int distance = 1, phase = 0;
	size_t nodeCount = connections->getNodePositionsArray().size();
	NodePos nodePos;
	while(true) {
		//if the position is taken
		if(connections->getNodePositionsArrayField(newRow, newCol)) {
			switch (phase) {
			case 0:	//right
				cornerCol += distance;
				if(cornerCol > nodeCount) {
					++phase;
					break;
				}
				while(newCol != cornerCol) {
					++newCol;
					if(!connections->getNodePositionsArrayField(newRow, newCol))
						break;
				}
				++phase;
				break;
			case 1:	//down
				cornerRow += distance;
				if(cornerRow > nodeCount) {
					++distance;
					++phase;
					break;
				}
				while(newRow != cornerRow) {
					++newRow;
					if(!connections->getNodePositionsArrayField(newRow, newCol))
						break;
				}
				++distance;
				++phase;
				break;
			case 2:	//left
				cornerCol -= distance;
				if(cornerCol < 0) {
					++phase;
					break;
				}
				while(newCol != cornerCol) {
					--newCol;
					if(!connections->getNodePositionsArrayField(newRow, newCol))
						break;
				}
				++phase;
				break;
			case 3:	//up
				cornerRow -= distance;
				if(cornerRow < 0) {
					++distance;
					++phase;
					break;
				}
				while(newRow != cornerRow) {
					--newRow;
					if(!connections->getNodePositionsArrayField(newRow, newCol))
						break;
				}
				++distance;
				++phase;
				break;
			}
			phase %= 4;
		} else {
			//is available
			nodePos.xPos = newRow;
			nodePos.yPos = newCol;
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
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultEdgeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );
	iEdgeType = matrixGraph->addType( "iEdgeType", settings );
}

