#ifndef Layout_RestrictionsDefinition_H
#define Layout_RestrictionsDefinition_H
//-----------------------------------------------------------------------------
#include "Data/Node.h"
#include "Layout/RestrictionApplier.h"
//-----------------------------------------------------------------------------
#include <QMap>
#include <QSet>
#include <QSharedPointer>
//-----------------------------------------------------------------------------

namespace Layout {

class RestrictionsDefinition {

public:

	void setRestrictions (
		QSet<Data::Node *> nodes,
		QSharedPointer<RestrictionApplier> restrictionApplier
	);

	QSharedPointer<RestrictionApplier> getRestrictionApplier (
		Data::Node &node
	);

	/***/
	virtual ~RestrictionsDefinition (void) {};

private:

	typedef QMap<Data::Node *, QSharedPointer<RestrictionApplier> > RestrictionsMapType;

	RestrictionsMapType restrictions_;

}; // class

} // namespace

#endif // Layout_RestrictionsDefinition_H
