#include "Layout/RestrictionsDefinition.h"

namespace Layout {

void RestrictionsDefinition::setRestrictions (
	QSet<Data::Node *> nodes,
	QSharedPointer<RestrictionApplier> restrictionApplier
) {
	for (QSet<Data::Node *>::iterator it = nodes.begin (); it != nodes.end (); ++it) {
		if (restrictionApplier.isNull()) {
			restrictions_.remove (*it);
		} else {
			restrictions_[*it] = restrictionApplier;
		}
	}
}

QSharedPointer<RestrictionApplier> RestrictionsDefinition::getRestrictionApplier (
	Data::Node &node
) {
	RestrictionsMapType::iterator it = restrictions_.find (&node);
	if (it != restrictions_.end ()) {
		return it.value();
	} else {
		return QSharedPointer<RestrictionApplier> ();
	}
}

} // namespace
