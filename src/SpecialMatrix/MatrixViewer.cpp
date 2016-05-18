#include "SpecialMatrix/MatrixViewer.h"

#include "osg/LineWidth"
#include "Importer/GraphOperations.h"

#include <QDebug>

SpecialMatrix::MatrixViewer::MatrixViewer( Data::Graph* matrixGraph, QString fileName )
{
	this->matrixGraph = matrixGraph;
	this->connections = new SpecialMatrix::NodeConnections();

	SpecialMatrix::FileParser( fileName, matrixGraph, connections );	

	createAxis();

	adjustPositions();

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
	Data::Type* axisNodeType = matrixGraph->addType( "axisNode", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "0" );
	settings->insert( "color.G", "0" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultEdgeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );
	Data::Type* axisEdgeType = matrixGraph->addType( "axisEdge", settings );

	osg::ref_ptr<Data::Node> center = matrixGraph->addNode( qHash("Center"), "Center", axisNodeType, osg::Vec3f(0.0f, 0.0f, 0.0f) );
	osg::ref_ptr<Data::Node> xAxis = matrixGraph->addNode( qHash("xAxis"), "xAxis", axisNodeType, osg::Vec3f((float)xNodesCount*separator, 0.0f, 0.0f) );
	osg::ref_ptr<Data::Node> yAxis = matrixGraph->addNode( qHash("yAxis"), "yAxis", axisNodeType, osg::Vec3f(0.0f, (float)yNodesCount*separator, 0.0f) );

	matrixGraph->addEdge( qHash("xAxisEdge"), "xAxisEdge", center, xAxis, axisEdgeType, false );
	matrixGraph->addEdge( qHash("yAxisEdge"), "yAxisEdge", center, yAxis, axisEdgeType, false );

}

void SpecialMatrix::MatrixViewer::adjustPositions()
{
	QMap<qlonglong, osg::ref_ptr<Data::Node> >* allNodes = matrixGraph->getNodes();

	QMapIterator<qlonglong, osg::ref_ptr<Data::Node> > i(*allNodes);

	while ( i.hasNext() ) {
		i.next();
		osg::Vec3f targetPos = i.value()->getTargetPosition();

		//[NOTE] by default:
		//addNode
		//this->mTargetPosition = position;
		//this->currentPosition = position * Util::ApplicationConfig::get()->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();

		i.value()->setRestrictedTargetPosition(targetPos);
	}
}
