#ifndef Layout_ShapeVisitor_RestrictedPositionGetter_H
#define Layout_ShapeVisitor_RestrictedPositionGetter_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------
#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Layout {

class ShapeVisitor_RestrictedPositionGetter : public ShapeVisitor {

public:

	/***/
	virtual ~ShapeVisitor_RestrictedPositionGetter (void) {};

	void setOriginalPosition (osg::Vec3f originalPosition);
	osg::Vec3f getRestrictedPosition (void);

	virtual void visit (Shape_Composite & shape);
	virtual void visit (Shape_Plane & shape);
	virtual void visit (Shape_Sphere & shape);
	virtual void visit (Shape_SphereSurface & shape);

private: // private methods

	osg::Vec3f toSphere (
		const osg::Vec3f center,
		const float radius,
		const osg::Vec3f &point
	);

private: // visitor context

	osg::Vec3f originalPosition_;
	osg::Vec3f restrictedPosition_;

}; // class

} // namespace

#endif // Layout_ShapeVisitor_H
