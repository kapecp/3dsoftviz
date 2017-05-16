/**
 * Created by Patrik Berger on 13.11.2016.
 */
#ifndef HANDNODE_H
#define HANDNODE_H


#include <osg/ref_ptr>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>

namespace osg {
class ShapeDrawable;
}

namespace Leap {

class HandNode: public osg::MatrixTransform
{
public:
	HandNode();
	void setColor( int colorSwitch, osg::ref_ptr<osg::ShapeDrawable> handDrawable );
	virtual void generateGeometry( float radius, int colorSwitch );
	virtual void initStructure();
};
}
#endif // HANDNODE_H
