#ifndef Layout_ShapeGetter_Cube_H
#define Layout_ShapeGetter_Cube_H

#include "Layout/ShapeGetter.h"

#include <osg/ref_ptr>

namespace Layout {

/**
 * \brief Returning a sphere.
 * Center is defined by the centerNode position.
 * Radius is defined by the surfaceNode position (the radius is computed in a way that
 * surfaceNode will be placed on the surface of the resulting sphere).
 */
class ShapeGetter_Cube : public ShapeGetter {

public:

    ShapeGetter_Cube (
			osg::ref_ptr<Data::Node> centerNode,
			osg::ref_ptr<Data::Node> surfaceNode
			);

	/***/
    virtual ~ShapeGetter_Cube (void) {}

	virtual QSharedPointer<Shape> getShape (void);

	virtual  QSet<Data::Node *> getNodesOfShape();


private:

	osg::ref_ptr<Data::Node> centerNode_;
	osg::ref_ptr<Data::Node> surfaceNode_;

}; // class

} // namespace

#endif // Layout_ShapeGetter_Cube_H
