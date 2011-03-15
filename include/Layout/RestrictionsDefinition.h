#ifndef Layout_RestrictionsDefinition_H
#define Layout_RestrictionsDefinition_H
//-----------------------------------------------------------------------------
#include "Data/Node.h"
#include "Layout/RestrictionApplier.h"
//-----------------------------------------------------------------------------
#include <QMap>
//-----------------------------------------------------------------------------

namespace Layout {

class RestrictionsDefinition {

public:

	void setRestrictions (
		QSet<osg::ref_ptr<Data::Node> > nodes,
		QSharedPointer<RestrictionApplier> restrictionApplier
	);

	QSharedPointer<RestrictionApplier> getRestrictionApplier (
		const osg::ref_ptr<Data::Node> node
	);

	/***/
	virtual ~RestrictionsDefinition (void) {};

private:

	typedef QMap<osg::ref_ptr<Data::Node>, QSharedPointer<RestrictionApplier> > RestrictionsMapType;

	RestrictionsMapType restrictions_;

}; // class

} // namespace

#endif // Layout_RestrictionsDefinition_H
