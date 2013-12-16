#ifndef Layout_ShapeGetter_Sphere_ByTwoNodes_H
#define Layout_ShapeGetter_Sphere_ByTwoNodes_H

#include "Layout/ShapeGetter.h"


namespace Layout {

/**
 * \brief Returning a sphere.
 * Center is defined by the centerNode position.
 * Radius is defined by the surfaceNode position (the radius is computed in a way that
 * surfaceNode will be placed on the surface of the resulting sphere).
 */
class ShapeGetter_Sphere_ByTwoNodes : public ShapeGetter {

public:

	ShapeGetter_Sphere_ByTwoNodes (
			osg::ref_ptr<Data::Node> centerNode,
			osg::ref_ptr<Data::Node> surfaceNode
			);

	/***/
	virtual ~ShapeGetter_Sphere_ByTwoNodes (void) {}

	virtual QSharedPointer<Shape> getShape (void);

	virtual  QSet<Data::Node *> getNodesOfShape();


private:

	osg::ref_ptr<Data::Node> centerNode_;
	osg::ref_ptr<Data::Node> surfaceNode_;

}; // class

} // namespace

#endif // Layout_ShapeGetter_Sphere_ByTwoNodes_H
