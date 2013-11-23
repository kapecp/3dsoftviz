#ifndef Layout_ShapeGetter_H
#define Layout_ShapeGetter_H
//-----------------------------------------------------------------------------
#include "Layout/Shape.h"
//#include "Data/Node.h"
//-----------------------------------------------------------------------------
#include <QSharedPointer>
#include <osg/ref_ptr>
//-----------------------------------------------------------------------------

namespace Data{
	class Node;
}

namespace Layout {

/**
 * \brief Implementations return current restriction shape.
 * [abstract class]
 * The purpose was to create the possibility to define dynamic shapes - the implementation
 * can return different shape after each call of getShape.
 */
class ShapeGetter {

public:

		virtual QSharedPointer<Shape> getShape (void)=0;

		virtual QSet<Data::Node *> getNodesOfShape(void)=0;

		/***/
		virtual ~ShapeGetter (void) {}


		void allowRestriction();

		void setInvisible(bool invisible){this->invisible = invisible;}

		bool isInvisible(){return invisible;}

protected:
		bool invisible;

}; // class

//PK: Hack for Qt v4.6.2, because Qt compilation fails on qmap.h `operator <`
//TODO: test if this operator implementation works...
#if (QT_VERSION < QT_VERSION_CHECK(4, 7, 0))
	typedef QSharedPointer<Layout::ShapeGetter> foo;
	bool operator<( foo const & one, foo const & other );
#endif

} // namespace

#endif // Layout_ShapeGetter_H
