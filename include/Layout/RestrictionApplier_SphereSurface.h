#ifndef Layout_RestrictionApplier_SphereSurface_H
#define Layout_RestrictionApplier_SphereSurface_H
//-----------------------------------------------------------------------------
#include "Layout/RestrictionApplier.h"
//-----------------------------------------------------------------------------
#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Layout {

class RestrictionApplier_SphereSurface : public RestrictionApplier {

public:

	RestrictionApplier_SphereSurface (
		const osg::Vec3f center,
		const float radius
	);

	/***/
	virtual ~RestrictionApplier_SphereSurface (void) {};

	virtual osg::Vec3f applyRestriction (
		const osg::Vec3f &point
	);

private:

	osg::Vec3f center_;

	float radius_;

}; // class

} // namespace

#endif // Layout_RestrictionApplier_SphereSurface_H
