#ifndef LAYOUT_SHAPE_CYLINDERSURFACE_H
#define LAYOUT_SHAPE_CYLINDERSURFACE_H

#include "Layout/Shape.h"

#include <QSlider>
#include <osg/Vec3f>

namespace QOSG { class ViewerQT; }

namespace Layout {

/**
 * \brief Represents a cylinder surface.
 * [immutable]
 */
class Shape_CylinderSurface : public Shape
{
public:
    Shape_CylinderSurface(const QOSG::ViewerQT *viewerWidget, const QSlider &baseRadiusSlider);
    virtual ~Shape_CylinderSurface() {}

    /**
     * [visitor pattern]
     */
    virtual void accept(ShapeVisitor &visitor);

    osg::Vec3f firstBaseCenter() const;
    osg::Vec3f secondBaseCenter() const;
    float baseRadius() const;

private:
    /**
     * A pointer to viewer widget from which we can get first and second base
     * center points of a cylinder.
     */
    const QOSG::ViewerQT *mViewerWidget;

    const QSlider &mBaseRadiusSlider;
};

} // namespace

#endif // LAYOUT_SHAPE_CYLINDERSURFACE_H
