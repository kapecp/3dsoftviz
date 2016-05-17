#include "Layout/ShapeGetter.h"
#include "Data/Node.h"
//-----------------------------------------------------------------------------
#include <qset.h>

namespace Layout {

#if (QT_VERSION < QT_VERSION_CHECK(4, 7, 0))
bool operator<( foo const& one, foo const& other )
{
	return one != other;
}
#endif


void ShapeGetter::allowRestriction()
{
	QSet<Data::Node* > nodes = getNodesOfShape();
	QSet<Data::Node* > ::iterator nodeIt;
	for ( nodeIt=nodes.begin(); nodeIt!=nodes.end(); ++nodeIt ) {
		( *nodeIt )->setIgnored( false );
		( *nodeIt )->setPositionCanBeRestricted( true );
	}
}

}
