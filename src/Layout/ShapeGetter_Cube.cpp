#include "Layout/ShapeGetter_Cube.h"
//-----------------------------------------------------------------------------
#include "Layout/Shape_Cube.h"
#include "Data/Node.h"
//-----------------------------------------------------------------------------
#include <qset.h>

namespace Layout {

ShapeGetter_Cube::ShapeGetter_Cube(
	osg::ref_ptr<Data::Node> centerNode,
	osg::ref_ptr<Data::Node> surfaceNodeX,
	osg::ref_ptr<Data::Node> surfaceNodeY,
	osg::ref_ptr<Data::Node> surfaceNodeZ
) : centerNode_( centerNode ), surfaceNodeX_( surfaceNodeX ), surfaceNodeY_( surfaceNodeY ), surfaceNodeZ_( surfaceNodeZ )
{
	// nothing
}

QSharedPointer<Shape> ShapeGetter_Cube::getShape( void )
{
	return QSharedPointer<Shape> (
			   new Shape_Cube(
				   centerNode_->targetPosition(),
				   surfaceNodeX_->targetPosition(),
				   surfaceNodeY_->targetPosition(),
				   surfaceNodeZ_->targetPosition(),
				   Shape_Cube::RANDOM_DISTANCE_FROM_CENTER
			   )
		   );
}

QSet<Data::Node* > ShapeGetter_Cube::getNodesOfShape()
{
	QSet<Data::Node* > nodes;
	nodes.insert( centerNode_.get() );
	nodes.insert( surfaceNodeX_.get() );
	nodes.insert( surfaceNodeY_.get() );
	nodes.insert( surfaceNodeZ_.get() );
	return nodes;
}

void ShapeGetter_Cube::move( double lengthX, double lengthY, double lengthZ )
{

	surfaceNodeX_->setTargetPosition( surfaceNodeX_->getTargetPosition() + osg::Vec3d( lengthX, 0, 0 ) );
	surfaceNodeX_->getCurrentPosition( true );
	surfaceNodeY_->setTargetPosition( surfaceNodeY_->getTargetPosition() + osg::Vec3d( 0, lengthY, 0 ) );
	surfaceNodeY_->getCurrentPosition( true );
	surfaceNodeZ_->setTargetPosition( surfaceNodeZ_->getTargetPosition() + osg::Vec3d( 0, 0, lengthZ ) );
	surfaceNodeZ_->getCurrentPosition( true );

}

float ShapeGetter_Cube::getDistanceX()
{
	return ( surfaceNodeX_->getTargetPosition() - centerNode_->getTargetPosition() ).length();
}

float ShapeGetter_Cube::getDistanceY()
{
	return ( surfaceNodeY_->getTargetPosition() - centerNode_->getTargetPosition() ).length();
}

float ShapeGetter_Cube::getDistanceZ()
{
	return ( surfaceNodeZ_->getTargetPosition() - centerNode_->getTargetPosition() ).length();
}

} // namespace
