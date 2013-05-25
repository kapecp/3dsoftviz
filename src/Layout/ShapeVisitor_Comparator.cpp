#include "Layout/ShapeVisitor_Comparator.h"
//-----------------------------------------------------------------------------

namespace Layout {

void ShapeVisitor_Comparator::setOtherShape (QSharedPointer<Shape> otherShape) {
	otherShape_ = otherShape;
}

bool ShapeVisitor_Comparator::getComparisonResult (void) {
	return comparisonResult_;
}

void ShapeVisitor_Comparator::visit (Shape_Null & shape) {
	QSharedPointer<Shape_Null> otherShape = otherShape_.dynamicCast<Shape_Null> ();

	comparisonResult_ = (! otherShape.isNull ());
}

void ShapeVisitor_Comparator::visit (Shape_Composite & shape) {
	QSharedPointer<Shape_Composite> otherShape = otherShape_.dynamicCast<Shape_Composite> ();
	if (! otherShape.isNull ()) {
		Shape_Composite::ShapesListType & list1 = shape.getShapes ();
		Shape_Composite::ShapesListType & list2 = otherShape->getShapes ();
		if (list1.size () == list2.size()) {
			Shape_Composite::ShapesListType::iterator it1 = list1.begin ();
			Shape_Composite::ShapesListType::iterator it2 = list2.begin ();
			for (; it1 != list1.end (); ++it1, ++it2) {
				otherShape_ = *it2;
				(*it1)->accept (*this);
				if (!comparisonResult_) {
					return;
				}
			}
		} else {
			comparisonResult_ = false;
		}
	} else {
		comparisonResult_ = false;
	}
}

void ShapeVisitor_Comparator::visit (Shape_Plane & shape) {
	QSharedPointer<Shape_Plane> otherShape = otherShape_.dynamicCast<Shape_Plane> ();
	if (! otherShape.isNull ()) {
		comparisonResult_ =
			(shape.getNormalVector () == otherShape->getNormalVector ())
			&&
			(shape.getD () == otherShape->getD ())
		;
	} else {
		comparisonResult_ = false;
	}
}

void ShapeVisitor_Comparator::visit (Shape_Sphere & shape) {
	QSharedPointer<Shape_Sphere> otherShape = otherShape_.dynamicCast<Shape_Sphere> ();
	if (! otherShape.isNull ()) {
		compareSpheres (shape, *otherShape);
	} else {
		comparisonResult_ = false;
	}
}

void ShapeVisitor_Comparator::visit (Shape_SphereSurface & shape) {
	QSharedPointer<Shape_SphereSurface> otherShape = otherShape_.dynamicCast<Shape_SphereSurface> ();
	if (! otherShape.isNull ()) {
        compareSpheres(shape, *otherShape);
	} else {
		comparisonResult_ = false;
    }
}

void ShapeVisitor_Comparator::visit(Shape_CylinderSurface &shape)
{
    QSharedPointer<Shape_CylinderSurface> otherShape = otherShape_.dynamicCast<Shape_CylinderSurface>();
    if (! otherShape.isNull ()) {
        compareCylinders(shape, *otherShape);
    } else {
        comparisonResult_ = false;
    }
}

void ShapeVisitor_Comparator::visit(Shape_ConeSurface &shape)
{
    QSharedPointer<Shape_ConeSurface> otherShape = otherShape_.dynamicCast<Shape_ConeSurface>();
    if (! otherShape.isNull ()) {
        compareCones(shape, *otherShape);
    } else {
        comparisonResult_ = false;
    }
}

void ShapeVisitor_Comparator::compareSpheres (Shape_AbstractSphere & shape, Shape_AbstractSphere & otherShape) {
	comparisonResult_ =
		(shape.getCenter () == otherShape.getCenter ())
		&&
		(shape.getRadius () == otherShape.getRadius ())
            ;
}

void ShapeVisitor_Comparator::compareCylinders(Shape_CylinderSurface &shape, Shape_CylinderSurface &otherShape)
{
    // TODO: Change made by Peter Sivak, I don't know how does this work for now - so return true for now
    comparisonResult_ = true;
    //comparisonResult_ = (shape.getCenter() == otherShape.getCenter()) && (shape.getRadius() == otherShape.getRadius());
}

void ShapeVisitor_Comparator::compareCones(Shape_ConeSurface &shape, Shape_ConeSurface &otherShape)
{
    // TODO: Change made by Peter Sivak, I don't know how does this work for now - so return true for now
    comparisonResult_ = true;
    //comparisonResult_ = (shape.getCenter() == otherShape.getCenter()) && (shape.getRadius() == otherShape.getRadius());
}

} // namespace
