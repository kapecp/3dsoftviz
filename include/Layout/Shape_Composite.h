#ifndef Layout_Shape_Composite_H
#define Layout_Shape_Composite_H
//-----------------------------------------------------------------------------
#include "Layout/Shape.h"
//-----------------------------------------------------------------------------
#include <list>
#include <QSharedPointer>
//-----------------------------------------------------------------------------

namespace Layout {

/**
 * Shape which can be composed using more shapes of various types.
 */
class Shape_Composite : public Shape
{

public:
//    Shape_Composite ( ShapesListType shapes_, int compositeType ){
//       this->shapes_ = shapes_;
//       this->compositeType = compositeType;
//    }

	class CompositeType
	{
	public:

		/**
			*  const int CIRCLE
			*  \brief intersection of plane and sphere surface
			*/
		static const int CIRCLE = 1;
	};

	typedef std::list<QSharedPointer<Shape> > ShapesListType;

	/***/
	virtual ~Shape_Composite( void ) {}

	void addShape(
		QSharedPointer<Shape> shape
	);

	ShapesListType& getShapes( void );

	/**
	 * [visitor pattern]
	 */
	virtual void accept(
		ShapeVisitor& visitor
	);

	void setCompositeType( int compositeType )
	{
		this->compositeType = compositeType;
	}

	int getCompositeType()
	{
		return this->compositeType;
	}

private:

	ShapesListType shapes_;

	int compositeType;



}; // class

} // namespace

#endif // Layout_Shape_Composite_H
