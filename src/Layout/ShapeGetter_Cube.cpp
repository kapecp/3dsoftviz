#include "Layout/ShapeGetter_Cube.h"
//-----------------------------------------------------------------------------
#include "Layout/Shape_Cube.h"
#include "Data/Node.h"
//-----------------------------------------------------------------------------
#include <qset.h>

namespace Layout {

ShapeGetter_Cube::ShapeGetter_Cube (
		osg::ref_ptr<Data::Node> centerNode,
		osg::ref_ptr<Data::Node> surfaceNode
		) : centerNode_ (centerNode),
	surfaceNode_ (surfaceNode)
{
	// nothing
}

QSharedPointer<Shape> ShapeGetter_Cube::getShape (void) {
	return QSharedPointer<Shape> (
                new Shape_Cube (
					centerNode_->targetPosition (),
					(centerNode_->targetPosition () - surfaceNode_->targetPosition ()).length (),
                    Shape_Cube::RANDOM_DISTANCE_FROM_CENTER
					)
				);
}

QSet<Data::Node * > ShapeGetter_Cube::getNodesOfShape(){
	QSet<Data::Node * > nodes;
	nodes.insert (centerNode_.get());
	nodes.insert (surfaceNode_.get());
	return nodes;
}

} // namespace
