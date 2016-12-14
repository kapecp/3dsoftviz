#ifndef SHAPEVISITOR_MANIPULATORCREATOR_H
#define SHAPEVISITOR_MANIPULATORCREATOR_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------
#include <osgManipulator/Dragger>
#include <osgManipulator/TrackballDragger>
#include <osgManipulator/Selection>
//-----------------------------------------------------------------------------


namespace Vwr {


class ShapeVisitor_ManipulatorCreator: public Layout::ShapeVisitor
{

public:

	class ManipulatorType
	{
	public:

		/**
			*  const int TRACKBALLDRAGGER
			*  \brief rotation
			*/
		static const int TRACKBALLDRAGGER = 0;


	};


	/***/
	virtual ~ShapeVisitor_ManipulatorCreator() {}

	osgManipulator::Dragger* getCreatedDragger()
	{
		return createdDragger;
	}
	osgManipulator::Selection* getCreatedSelection()
	{
		return createdSelection;
	}

	/**
	 * \brief Creates empty OSG group.
	 */

	virtual void visit( Layout::Shape_Null /*& shape*/ );

	/**
	 * \brief Creates OSG group containing  manipulator for each shape contained in the composite shape.
	 */
	virtual void visit( Layout::Shape_Composite& shape );

	/**
	 * \brief Creates OSG group containing  manipulator for intersection of shapes contained in the composite shape.
	 */
	virtual void visit( Layout::Shape_Intersection& shape );

	/**
	 * \brief Creates OSG manipulator for the plane.
	 */
	virtual void visit( Layout::Shape_Plane& shape );

	/**
	 * \brief Creates OSG manipulator for the sphere.
	 */
	virtual void visit( Layout::Shape_Sphere& shape );

	/**
	 * \brief Creates OSG manipulator for the sphere.
	 */
	virtual void visit( Layout::Shape_SphereSurface& shape );

	virtual void visit( Layout::Shape_CylinderSurface& shape );
	virtual void visit( Layout::Shape_ConeSurface& shape );

	virtual void visit( Layout::Shape_Cube& shape );

	void setType( int manipulatorType )
	{
		this->type = manipulatorType;
	}



private:

	osgManipulator::Dragger* createdDragger;
	osg::MatrixTransform* createdSelection;
	int type;

	void executeDefault();

	void createRotationManipulator( osg::Vec3 center,float trackBallRadius, int key );
};


}

#endif // SHAPEVISITOR_MANIPULATORCREATOR_H
