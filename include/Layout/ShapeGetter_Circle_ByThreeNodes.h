#ifndef Layout_ShapeGetter_Circle_ByThreeNodes_H
#define Layout_ShapeGetter_Circle_ByThreeNodes_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeGetter.h"
#include "Layout/ShapeGetter_SphereSurface_ByTwoNodes.h"
#include "Layout/ShapeGetter_Plane_ByThreeNodes.h"

//-----------------------------------------------------------------------------

namespace Layout {

/**
 * \brief Returning a circle on plane defined by three points (specified by current positions of three nodes
 * provided in the constructor).
 */
class ShapeGetter_Circle_ByThreeNodes : public ShapeGetter {

public:

	ShapeGetter_Circle_ByThreeNodes (
			osg::ref_ptr<Data::Node> node1,
			osg::ref_ptr<Data::Node> node2,
			osg::ref_ptr<Data::Node> node3
			);

	/***/
	virtual ~ShapeGetter_Circle_ByThreeNodes (void) {};

	virtual QSharedPointer<Shape> getShape (void);

	virtual  QSet<Data::Node * > getNodesOfShape();

private:

	osg::ref_ptr<Data::Node> node1_;
	osg::ref_ptr<Data::Node> node2_;
	osg::ref_ptr<Data::Node> node3_;

    //volovar_zac
    QSharedPointer<Shape_Intersection> intersection_;
    //volovar_kon
}; // class

} // namespace

#endif // Layout_ShapeGetter_Plane_ByThreeNodes_H
