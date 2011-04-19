#ifndef Layout_ShapeGetter_SphereSurface_ByTwoNodes_H
#define Layout_ShapeGetter_SphereSurface_ByTwoNodes_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeGetter.h"
//-----------------------------------------------------------------------------
#include "Data/Node.h"
//-----------------------------------------------------------------------------

namespace Layout {

class ShapeGetter_SphereSurface_ByTwoNodes : public ShapeGetter {

public:

	ShapeGetter_SphereSurface_ByTwoNodes (
		osg::ref_ptr<Data::Node> centerNode,
		osg::ref_ptr<Data::Node> surfaceNode
	);

	/***/
	virtual ~ShapeGetter_SphereSurface_ByTwoNodes (void) {};

	virtual QSharedPointer<Shape> getShape (void);

private:

	osg::ref_ptr<Data::Node> centerNode_;
	osg::ref_ptr<Data::Node> surfaceNode_;

}; // class

} // namespace

#endif // Layout_ShapeGetter_SphereSurface_ByTwoNodes_H
