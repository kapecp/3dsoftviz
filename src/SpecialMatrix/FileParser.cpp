#include "SpecialMatrix/FileParser.h"

SpecialMatrix::FileParser::FileParser( QString fileName, Data::Graph* matrixGraph, SpecialMatrix::NodeConnections* connections )
{
	createSettings(matrixGraph);
	parseFile(matrixGraph, fileName, connections);
}

void SpecialMatrix::FileParser::parseFile( Data::Graph* matrixGraph, QString fileName, SpecialMatrix::NodeConnections* connections )
{
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(0, "error", file.errorString());
	}

	QTextStream in(&file);

	while(!in.atEnd()) {
		QString line = in.readLine();
		QStringList fields = line.split('\t');

		if(fields.at(0).contains('E') && fields.at(1).contains('I') && fields.at(2).contains('N'))
			matrixGraph = addEINNodesToGraph(matrixGraph, fields, connections);
		//qDebug() << "line: "<< fields.at(0) << ", " << fields.at(1) << ", " << fields.at(2);
	}
	file.close();
}

Data::Graph* SpecialMatrix::FileParser::addEINNodesToGraph( Data::Graph* matrixGraph, QStringList fields, SpecialMatrix::NodeConnections* connections )
{	
	int separator = Util::ApplicationConfig::get()->getValue( "Viewer.Display.MatrixNodeSeparator" ).toInt();
	QString xNodeName, yNodeName;
	qlonglong xAxisPosition, yAxisPosition;
	qlonglong xFrstNodeId, yFrstNodeId, xSecNodeId, ySecNodeId, xTrdNodeId, yTrdNodeId;
	osg::Vec3f nodePos;

	QList<qlonglong>* xAxisNodes = connections->getXAxisNodes();
	QList<qlonglong>* yAxisNodes = connections->getYAxisNodes();
	QList<qlonglong>* iNodes = connections->getINodes();
	QMap<qlonglong, QList<qlonglong>* >* connectedNodes = connections->getConnectedNodes();
	QList<qlonglong>* tempList;

	//-----start node (E)
	//x axis
	xNodeName = fields.at(0);
	xNodeName = xNodeName.append('x');
	xFrstNodeId = qHash(xNodeName);

	//insert node into xAxisNodes list only if missing
	if(!xAxisNodes->contains(xFrstNodeId))
	{		
		xAxisNodes->push_back(xFrstNodeId);
		xAxisPosition = xAxisNodes->indexOf(xFrstNodeId)+1;
		nodePos = osg::Vec3f((float)xAxisPosition*separator, 0.0f, 0.0f);
		matrixGraph->addNode( xFrstNodeId, xNodeName, eNodeType, nodePos );
	}
	//get list from Qmap where key is xFrstNodeId and insert I node into connectedNodes list
	tempList = connectedNodes->value(xFrstNodeId);
	if(tempList == nullptr)
		tempList = new QList<qlonglong>;
	tempList->push_back(getHashIdFromNodeName(fields.at(1), 'x'));
	connectedNodes->insert(xFrstNodeId, tempList);


	//y axis
	yNodeName = fields.at(0);
	yNodeName = yNodeName.append('y');
	yFrstNodeId = qHash(yNodeName);

	//insert node into yAxisNodes list only if missing
	if(!yAxisNodes->contains(yFrstNodeId))
	{
		yAxisNodes->push_back(yFrstNodeId);
		yAxisPosition = yAxisNodes->indexOf(yFrstNodeId)+1;
		nodePos = osg::Vec3f(0.0f, (float)yAxisPosition*separator, 0.0f);
		matrixGraph->addNode( yFrstNodeId, yNodeName, eNodeType, nodePos );
	}
	//get list from Qmap where key is yFrstNodeId and insert I node into connectedNodes list
	tempList = connectedNodes->value(yFrstNodeId);
	if(tempList == nullptr)
		tempList = new QList<qlonglong>;
	tempList->push_back(getHashIdFromNodeName(fields.at(1), 'y'));
	connectedNodes->insert(yFrstNodeId, tempList);

	//-----end node (N)
	//x axis
	xNodeName = fields.at(2);
	xNodeName = xNodeName.append('x');
	xTrdNodeId = qHash(xNodeName);

	//insert node into xAxisNodes list only if missing
	if(!xAxisNodes->contains(xTrdNodeId))
	{		
		xAxisNodes->push_back(xTrdNodeId);
		xAxisPosition = xAxisNodes->indexOf(xTrdNodeId)+1;
		nodePos = osg::Vec3f((float)xAxisPosition*separator, 0.0f, 0.0f);
		matrixGraph->addNode( xTrdNodeId, xNodeName, nNodeType, nodePos );
	}
	//get list from Qmap where key is xTrdNodeId and insert I node into connectedNodes list
	tempList = connectedNodes->value(xTrdNodeId);
	if(tempList == nullptr)
		tempList = new QList<qlonglong>;
	tempList->push_back(getHashIdFromNodeName(fields.at(1), 'x'));
	connectedNodes->insert(xTrdNodeId, tempList);

	//y axis
	yNodeName = fields.at(2);
	yNodeName = yNodeName.append('y');
	yTrdNodeId = qHash(yNodeName);

	//insert node into yAxisNodes list only if missing
	if(!yAxisNodes->contains(yTrdNodeId))
	{		
		yAxisNodes->push_back(yTrdNodeId);
		yAxisPosition = yAxisNodes->indexOf(yTrdNodeId)+1;
		nodePos = osg::Vec3f(0.0f, (float)yAxisPosition*separator, 0.0f);
		matrixGraph->addNode( yTrdNodeId, yNodeName, nNodeType, nodePos );
	}
	//get list from Qmap where key is yTrdNodeId and insert I node into connectedNodes list
	tempList = connectedNodes->value(yTrdNodeId);
	if(tempList == nullptr)
		tempList = new QList<qlonglong>;
	tempList->push_back(getHashIdFromNodeName(fields.at(1), 'y'));
	connectedNodes->insert(yTrdNodeId, tempList);

	//-----middle node (I)
	//x axis
	xNodeName = fields.at(1);
	xNodeName = xNodeName.append('x');
	xSecNodeId = qHash(xNodeName);

	//insert node into iNodes list only if missing
	if(!iNodes->contains(xSecNodeId))
	{
		iNodes->push_back(xSecNodeId);
		xAxisPosition = xAxisNodes->indexOf(xFrstNodeId)+1;
		yAxisPosition = yAxisNodes->indexOf(yTrdNodeId)+1;
		nodePos = osg::Vec3f((float)xAxisPosition*separator, (float)yAxisPosition*separator, 0.0f);
		matrixGraph->addNode( xSecNodeId, xNodeName, iNodeType, nodePos );
	}

	//y axis
	yNodeName = fields.at(1);
	yNodeName = yNodeName.append('y');
	ySecNodeId = qHash(yNodeName);

	//insert node into iNodes list only if missing
	if(!iNodes->contains(ySecNodeId))
	{
		iNodes->push_back(ySecNodeId);
		xAxisPosition = xAxisNodes->indexOf(xTrdNodeId)+1;
		yAxisPosition = yAxisNodes->indexOf(yFrstNodeId)+1;
		nodePos = osg::Vec3f((float)xAxisPosition*separator, (float)yAxisPosition*separator, 0.0f);
		matrixGraph->addNode( ySecNodeId, yNodeName, iNodeType, nodePos );
	}

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

void SpecialMatrix::FileParser::createSettings( Data::Graph* matrixGraph )
{
	QMap<QString, QString>* settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "0" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.SquareNode" ) );
	eNodeType = matrixGraph->addType( "eNode", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.SquareNode" ) );
	iNodeType = matrixGraph->addType( "iNode", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "0" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.SquareNode" ) );
	nNodeType = matrixGraph->addType( "nNode", settings );
}

