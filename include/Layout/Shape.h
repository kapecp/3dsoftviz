#ifndef LAYOUT_SHAPE_H
#define LAYOUT_SHAPE_H

namespace Layout {

class ShapeVisitor;

/**
 * Base class of all shapes.
 */
class Shape
{
public:
    virtual ~Shape() {}

	/**
	 * [visitor pattern]
	 */
    virtual void accept(ShapeVisitor &visitor) = 0;

};

}

#endif // LAYOUT_SHAPE_H
