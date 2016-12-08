#ifndef Layout_ShapeGetter_Const_H
#define Layout_ShapeGetter_Const_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeGetter.h"
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * \brief Returning a constant shape (specified in the constructor).
 */
class ShapeGetter_Const : public ShapeGetter
{

public:

	explicit ShapeGetter_Const(
		QSharedPointer<Shape> shape
	);

	/***/
	virtual ~ShapeGetter_Const( void ) {}

	virtual QSharedPointer<Shape> getShape( void );

	virtual QSet<Data::Node*  > getNodesOfShape();

private:

	QSharedPointer<Shape> shape_;

}; // class

} // namespace

#endif // Layout_ShapeGetter_Const_H
