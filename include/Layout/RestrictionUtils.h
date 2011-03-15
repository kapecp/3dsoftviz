#ifndef Layout_RestrictionUtils_H
#define Layout_RestrictionUtils_H
//-----------------------------------------------------------------------------
#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Layout {

class RestrictionUtils {

public:

	static osg::Vec3f toSphere (
		const osg::Vec3f center,
		const float radius,
		const osg::Vec3f &point
	);

private:

	/***/
	RestrictionUtils (void) {};

	/***/
	virtual ~RestrictionUtils (void) {};

}; // class

} // namespace

#endif // Layout_RestrictionUtils_H
