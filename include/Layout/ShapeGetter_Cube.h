#ifndef Layout_ShapeGetter_Cube_H
#define Layout_ShapeGetter_Cube_H

#include "Layout/ShapeGetter.h"

#include <osg/ref_ptr>
#include <osg/Vec3d>

namespace Layout {

class ShapeGetter_Cube : public ShapeGetter
{

public:

	ShapeGetter_Cube(
		osg::ref_ptr<Data::Node> centerNode,
		osg::ref_ptr<Data::Node> surfaceNodeX,
		osg::ref_ptr<Data::Node> surfaceNodeY,
		osg::ref_ptr<Data::Node> surfaceNodeZ
	);

	/***/
	virtual ~ShapeGetter_Cube( void ) {}

	virtual QSharedPointer<Shape> getShape( void );

	virtual  QSet<Data::Node*> getNodesOfShape();

	void move( double lengthX, double lengthY, double lengthZ );
	float getDistanceX();
	float getDistanceY();
	float getDistanceZ();

	osg::ref_ptr<Data::Node> getCenterNode()
	{
		return centerNode_;
	}
	osg::ref_ptr<Data::Node> getSurfaceNodeX()
	{
		return surfaceNodeX_;
	}
	osg::ref_ptr<Data::Node> getSurfaceNodeY()
	{
		return surfaceNodeY_;
	}
	osg::ref_ptr<Data::Node> getSurfaceNodeZ()
	{
		return surfaceNodeZ_;
	}

	void setCenterNode( osg::ref_ptr<Data::Node> centerNode )
	{
		centerNode_ = centerNode;
	}
	void setSurfaceNodeX( osg::ref_ptr<Data::Node> surfaceNodeX )
	{
		surfaceNodeX_ = surfaceNodeX;
	}
	void setSurfaceNodeY( osg::ref_ptr<Data::Node> surfaceNodeY )
	{
		surfaceNodeY_ = surfaceNodeY;
	}
	void setSurfaceNodeZ( osg::ref_ptr<Data::Node> surfaceNodeZ )
	{
		surfaceNodeZ_ = surfaceNodeZ;
	}

private:

	osg::ref_ptr<Data::Node> centerNode_;
	osg::ref_ptr<Data::Node> surfaceNodeX_;
	osg::ref_ptr<Data::Node> surfaceNodeY_;
	osg::ref_ptr<Data::Node> surfaceNodeZ_;

	QSharedPointer<Shape> shape;

}; // class

} // namespace

#endif // Layout_ShapeGetter_Cube_H
