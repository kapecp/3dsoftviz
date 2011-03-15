#ifndef Layout_RestrictionApplier_Plane_H
#define Layout_RestrictionApplier_Plane_H
//-----------------------------------------------------------------------------
#include "Layout/RestrictionApplier.h"
//-----------------------------------------------------------------------------
#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Layout {

class RestrictionApplier_Plane : public RestrictionApplier {

public:
	// normal vector = [a, b, c]
	// ax + by + cz = d
	RestrictionApplier_Plane (
		const osg::Vec3f normalVector,
		const float d
	);

	/***/
	virtual ~RestrictionApplier_Plane (void) {};

	virtual osg::Vec3f applyRestriction (
		const osg::Vec3f &point
	);

private:

	osg::Vec3f normalVector_;
	float d_;

}; // class

} // namespace

#endif // Layout_RestrictionApplier_Plane_H
