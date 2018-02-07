/**
 * Created by Patrik Berger on 13.11.2016.
 */
#ifndef HANDNODE_H
#define HANDNODE_H

#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>

#include <QDataStream>

namespace Leap {

enum HandColors {
	NONE = 0,
	RIGHT = 1,
	LEFT = 2
};

class HandNode: public osg::MatrixTransform
{
public:
	HandNode();
	/**
	 * @brief setColor - set color of the drawable
	 * @param colorSwitch - specify color of element
	 * @param handDrawable - drawable to change
	 */
	void setColor( HandColors colorSwitch, osg::ref_ptr<osg::ShapeDrawable> handDrawable );
	/**
	 * @brief generateGeometry - interface method for generating all necessary objects (hierarchy) to display node in screen
	 * @param radius - radius of the node
	 * @param colorSwitch - color of the node
	 */
	virtual void generateGeometry( float radius, HandColors colorSwitch );
	/**
	 * @brief initStructure - interface method for generating child elements of the node
	 */
	virtual void initStructure();

	virtual void addToStream( QDataStream *stream );
	virtual void setFromStream( QDataStream *stream );
};
}
#endif // HANDNODE_H
