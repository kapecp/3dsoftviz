#ifndef Layout_RestrictionRemovalHandler_H
#define Layout_RestrictionRemovalHandler_H
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * \brief Performs a cleanup after a restriction is removed.
 */
class RestrictionRemovalHandler {

public:

	/**
	 * \brief Performs a cleanup (e.g. removes parts added to the scene which have
	 * been associated with the restriction).
	 * Called from RestrictionsManager after the restriction is removed.
	 */
	virtual void afterRestrictionRemoved (void) = 0;

	/***/
	virtual ~RestrictionRemovalHandler (void) {};

}; // class

} // namespace

#endif // Layout_RestrictionRemovalHandler_H
