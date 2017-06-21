#ifndef ARSELECTIONLAYOUTSTRATEGY_H
#define ARSELECTIONLAYOUTSTRATEGY_H

#include <osg/Vec3f>
#include "Data/Node.h"
#include "Core/Core.h"

namespace Data {
class Graph;
}

namespace ArucoModul {


class ArSelectionLayoutStrategy: public QObject
{
	Q_OBJECT

public:
	ArSelectionLayoutStrategy();
	void setSelectionLayout( osg::ref_ptr<Data::Node> node );
	void resetSelectionLayout( osg::ref_ptr<Data::Node> node, bool isReturnNode = true );
private:
	virtual void setSelectedNodeLayoutOptions( osg::ref_ptr<Data::Node> node ) = 0;
	virtual void resetSelectedNodeLayoutOptions( osg::ref_ptr<Data::Node> node, bool isReturnNode = true ) = 0;
};




class ArSelectionLayoutStrategyNodeOnly: public ArSelectionLayoutStrategy
{
public:
	ArSelectionLayoutStrategyNodeOnly(): ArSelectionLayoutStrategy() {}
private:
	osg::Vec3f original_position;
	void setSelectedNodeLayoutOptions( osg::ref_ptr<Data::Node> node );
	void resetSelectedNodeLayoutOptions( osg::ref_ptr<Data::Node> node, bool isReturnNode = true );
};

class ArSelectionLayoutStrategyNodeCluster: public ArSelectionLayoutStrategy
{
public:
	ArSelectionLayoutStrategyNodeCluster(): ArSelectionLayoutStrategy() {}
private:
	void setSelectedNodeLayoutOptions( osg::ref_ptr<Data::Node> node );
	void resetSelectedNodeLayoutOptions( osg::ref_ptr<Data::Node> node, bool isReturnNode = true );
};

}


#endif // ARSELECTIONLAYOUTSTRATEGY_H
