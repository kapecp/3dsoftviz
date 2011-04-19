#ifndef Layout_RestrictionRemovalHandler_H
#define Layout_RestrictionRemovalHandler_H
//-----------------------------------------------------------------------------

namespace Layout {

class RestrictionRemovalHandler {

public:

	virtual void afterRestrictionRemoved (void) = 0;

	/***/
	virtual ~RestrictionRemovalHandler (void) {};

}; // class

} // namespace

#endif // Layout_RestrictionRemovalHandler_H
