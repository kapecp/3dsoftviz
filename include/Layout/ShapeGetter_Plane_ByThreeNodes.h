#ifndef Layout_ShapeGetter_Plane_ByThreeNodes_H
#define Layout_ShapeGetter_Plane_ByThreeNodes_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeGetter.h"
//-----------------------------------------------------------------------------
#include "Data/Node.h"
//-----------------------------------------------------------------------------

namespace Layout {

class ShapeGetter_Plane_ByThreeNodes : public ShapeGetter {

public:

	ShapeGetter_Plane_ByThreeNodes (
		osg::ref_ptr<Data::Node> node1,
		osg::ref_ptr<Data::Node> node2,
		osg::ref_ptr<Data::Node> node3
	);

	/***/
	virtual ~ShapeGetter_Plane_ByThreeNodes (void) {};

	virtual QSharedPointer<Shape> getShape (void);

private:

	osg::ref_ptr<Data::Node> node1_;
	osg::ref_ptr<Data::Node> node2_;
	osg::ref_ptr<Data::Node> node3_;

}; // class

} // namespace

#endif // Layout_ShapeGetter_Plane_ByThreeNodes_H
