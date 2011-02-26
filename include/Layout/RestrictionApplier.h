#ifndef Layout_RestrictionApplier_H
#define Layout_RestrictionApplier_H
//-----------------------------------------------------------------------------
#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Layout {

class RestrictionApplier {

public:

	/***/
	virtual ~RestrictionApplier (void) {};

	virtual osg::Vec3f applyRestriction (
		const osg::Vec3f &point
	) = 0;

}; // class

} // namespace

#endif // Layout_RestrictionApplier_H
