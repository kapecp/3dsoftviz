#include "Layout/ShapeGetter_Const.h"
#include "Data/Node.h"

#include <qset.h>

namespace Layout {

ShapeGetter_Const::ShapeGetter_Const(
	QSharedPointer<Shape> shape
) : shape_( shape )
{
	// nothing
}

QSharedPointer<Shape> ShapeGetter_Const::getShape( void )
{
	return shape_;
}

QSet<Data::Node* >  ShapeGetter_Const::getNodesOfShape()
{
	QSet<Data::Node* > nodes;
	return nodes;
}

} // namespace
