#ifndef Layout_Shape_Intersection_H
#define Layout_Shape_Intersection_H
//-----------------------------------------------------------------------------
#include "Layout/Shape_Composite.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * Shape which is intersection of more shapes of various types.
 */
class Shape_Intersection : public Shape_Composite {

	/**
	 * [visitor pattern]
	 */
	virtual void accept (
			ShapeVisitor & visitor
			);
public:
    //volovar_zac
    Shape_Intersection() {setAlpha(0.06f);} //shapu bola pridaná vlastnosť alphy, kvoli manipulácii s aplhou pri radial layoute
    //volovar_kon

}; // class

} // namespace

#endif // Layout_Shape_Intersection_H
