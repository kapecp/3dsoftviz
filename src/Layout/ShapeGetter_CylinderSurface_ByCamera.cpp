#include "Layout/ShapeGetter_CylinderSurface_ByCamera.h"

#include "Layout/Shape_CylinderSurface.h"


namespace Layout {

ShapeGetter_CylinderSurface_ByCamera::ShapeGetter_CylinderSurface_ByCamera(const QOSG::ViewerQT *viewerWidget,
																		   const QSlider &baseRadiusSlider) :
	mViewerWidget(viewerWidget),
	mBaseRadiusSlider(baseRadiusSlider)
{
}

QSharedPointer<Shape> ShapeGetter_CylinderSurface_ByCamera::getShape()
{
	return QSharedPointer<Shape>( new Shape_CylinderSurface(mViewerWidget, mBaseRadiusSlider) );
}
QSet<Data::Node *> ShapeGetter_CylinderSurface_ByCamera::getNodesOfShape()
{
	//implement
	QSet<Data::Node *  >  nodes;
	return nodes;
}
}
