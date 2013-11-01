#ifndef UTIL_ELEMENT_SELECTOR
#define UTIL_ELEMENT_SELECTOR 1

#include "Viewer/NodeGroup.h"
#include "Viewer/EdgeGroup.h"
#include "Viewer/PickHandler.h"

namespace Util
{
	class ElementSelector
	{
	public:
		static void randomElementSelector(QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, QMap<qlonglong, osg::ref_ptr<Data::Edge> >* edges, int num, Vwr::PickHandler * pickhandler);
		static void weightedElementSelector(QMap<qlonglong, osg::ref_ptr<Data::Node> >* nodes, int num, Vwr::PickHandler * pickhandler);
	};
}

#endif