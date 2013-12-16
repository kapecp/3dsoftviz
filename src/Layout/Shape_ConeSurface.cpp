#include "Layout/Shape_ConeSurface.h"

#include "Layout/ShapeVisitor.h"
#include "QOSG/ViewerQT.h"

namespace Layout {

Shape_ConeSurface::Shape_ConeSurface(const QOSG::ViewerQT *viewerWidget, const QSlider &baseRadiusSlider) :
	mViewerWidget(viewerWidget),
	mBaseRadiusSlider(baseRadiusSlider)
{
}

void Shape_ConeSurface::accept(ShapeVisitor &visitor)
{
	visitor.visit(*this);
}

osg::Vec3f Shape_ConeSurface::baseCenter() const
{
	osg::Vec3f eye;
	osg::Vec3f center;
	osg::Vec3f up;
	mViewerWidget->getCamera()->getViewMatrixAsLookAt(eye, center, up);

	return eye;
}

osg::Vec3f Shape_ConeSurface::spike() const
{
	osg::Vec3f eye;
	osg::Vec3f center;
	osg::Vec3f up;
	center = mViewerWidget->getCameraManipulator()->getCenter();

	return center;
}

float Shape_ConeSurface::baseRadius() const
{
	return (mBaseRadiusSlider.value() / 100.0) * 250;
}

}
