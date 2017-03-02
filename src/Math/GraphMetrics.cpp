#include "Math/GraphMetrics.h"

#include "Data/Graph.h"
#include "Math/DjikstraAlg.h"

#include "Util/ApplicationConfig.h"

namespace MathModule {

void GraphMetrics::computeGraphMetrics( Data::Graph* activeGraph )
{
	QMutableMapIterator<qlonglong, osg::ref_ptr<Data::Node> > i( *activeGraph->getNodes() );

	int numVertices = activeGraph->getNodes()->size();
	QVector<QLinkedList<osg::ref_ptr<Data::Node> > >**   paths;

	paths = new QVector<QLinkedList<osg::ref_ptr<Data::Node> > >* [numVertices]();

	int idx = 0;

	Util::ApplicationConfig* appConf = Util::ApplicationConfig::get();
	QString regexp = appConf->getValue( "Graph.Metrics.Regexp" );

	QRegExp re( regexp );
	re.setPatternSyntax( QRegExp::Wildcard );

	// compute metrics for all nodes in graph
	while ( i.hasNext() ) {
		i.next();

		// compute node degree

		float nodeDegree = static_cast<float>( i.value()->getEdges()->size() ) / static_cast<float>( numVertices - 1 );
		i.value()->setNodeDegree( nodeDegree );

		// get all shortest paths
		DjikstraAlg* alg = new DjikstraAlg( activeGraph );
		QMap<qlonglong, qlonglong> shortestPaths = alg->execute( i.value() );

		QMutableMapIterator<qlonglong, qlonglong> i2( shortestPaths );

		paths[idx] = new QVector<QLinkedList<osg::ref_ptr<Data::Node> > > [numVertices];

		// compute node closeness
		qlonglong pathSum = 0;
		while ( i2.hasNext() ) {
			i2.next();
			pathSum += i2.value();
		}

		if ( numVertices == 1 ) {
			numVertices = 2;
		}

		i.value()->setNodeCloseness( static_cast<float>( pathSum ) / static_cast<float>( numVertices - 1 ) );

		// check for regexp match
		if ( re.exactMatch( ( static_cast<Data::AbsNode*>( i.value() ) )->getName() ) ) {
			i.value()->setNodeMatched( true );
		}

		int idx2 = 0;

		// get shortest paths to all nodes for node betweenness
		QMutableMapIterator<qlonglong, osg::ref_ptr<Data::Node> > i3( *activeGraph->getNodes() );
		while ( i3.hasNext() ) {
			i3.next();
			paths[idx][idx2++] = alg->getAllPaths( i3.value() );
		}

		idx++;
	}

	i.toFront();
	float nodeDegreeWeigth = appConf->getValue( "Graph.Metrics.NodeDegreeWeigth" ).toFloat();
	float betweennessWeigth = appConf->getValue( "Graph.Metrics.BetweennessWeigth" ).toFloat();
	float closenessWeigth = appConf->getValue( "Graph.Metrics.ClosenessWeigth" ).toFloat();
	float regexpWeigth = appConf->getValue( "Graph.Metrics.RgexpWeigth" ).toFloat();

	// iterate through all nodes
	while ( i.hasNext() ) {

		i.next();
		float sum = 0;

		// compute node betweenness
		for ( int x = 0; x < numVertices; x++ ) {
			for ( int y = 0; y < numVertices; y++ ) {
				int cnt = 0;

				for ( int z = 0; z < paths[x][y].size(); z++ ) {
					if ( paths[x][y].at( z ).contains( i.value() ) ) {
						cnt++;
					}
				}

				if ( cnt != 0 ) {
					sum += static_cast<float>( cnt ) / static_cast<float>( paths[x][y].size() );
				}
			}
		}
		delete[] paths;
		i.value()->setNodeBetweeness( sum / ( static_cast<float>( numVertices - 1 ) * static_cast<float>( numVertices - 2 ) )	);

		float base = i.value()->isNodeMatched() ? regexpWeigth : 0;

		// compute overall node metric
		float overallWeight = nodeDegreeWeigth * i.value()->getNodeDegree() + closenessWeigth * i.value()->getNodeCloseness() + betweennessWeigth * i.value()->getNodeBetweeness() + base;

		i.value()->setOverallWeight( overallWeight );
	}

}

} // namespace MathModule
