/**
 * Created by Patrik Berger on 13.11.2016.
 */
#ifndef HANDNODE_H
#define HANDNODE_H

#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>

namespace Leap {

class HandNode: public osg::MatrixTransform
{
public:
	HandNode();
	/**
	 * @brief setColor -  set color of the drawable
	 * @param colorSwitch 1 - green, 2 - blue
	 * @param handDrawable -  drawable to change
	 */
	void setColor( int colorSwitch, osg::ref_ptr<osg::ShapeDrawable> handDrawable );
	/**
	 * @brief generateGeometry - interface method for generating all necessary objects (hierarchy) to display node in screen
	 * @param radius - radius of the node
	 * @param colorSwitch - color of the node, 1- green, 2- blue
	 */
	virtual void generateGeometry( float radius, int colorSwitch );
	/**
	 * @brief initStructure  - interface method for generating child elements of the node
	 */
	virtual void initStructure();
};
}
#endif // HANDNODE_H
