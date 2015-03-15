#ifndef LAYOUT_SHAPE_CONESURFACE_H
#define LAYOUT_SHAPE_CONESURFACE_H

#include "Layout/Shape.h"

#include <QSlider>
#include <QSpinBox>
#include <osg/Vec3f>

namespace QOSG {
class ViewerQT;
}

namespace Layout {

/**
 * \brief Represents a cone surface.
 * [immutable]
 */
class Shape_ConeSurface : public Shape
{
public:
	Shape_ConeSurface( const QOSG::ViewerQT* viewerWidget, const QSpinBox& baseRadiusSpinBox );
	virtual ~Shape_ConeSurface() {}

	/**
	 * [visitor pattern]
	 */
	virtual void accept( ShapeVisitor& visitor );

	osg::Vec3f baseCenter() const;
	osg::Vec3f spike() const;
	float baseRadius() const;

private:
	/**
	 * A pointer to viewer widget from which we can get base center point and
	 * spike point of a cone.
	 */
	const QOSG::ViewerQT* mViewerWidget;

	const QSpinBox& mBaseRadiusSpinBox;
};

} // namespace

#endif // LAYOUT_SHAPE_CONESURFACE_H
