#include "Layout/ShapeGetter.h"
#include "Data/Node.h"
//-----------------------------------------------------------------------------

namespace Layout {

void ShapeGetter::allowRestriction(){
	QSet<Data::Node * > nodes = getNodesOfShape();
	QSet<Data::Node * > ::iterator nodeIt;
	for (nodeIt=nodes.begin(); nodeIt!=nodes.end(); nodeIt++){
		(*nodeIt)->setIgnored (false);
		(*nodeIt)->setPositionCanBeRestricted (true);
	}
}

}
