#include "Layout/ShapeGetter_ConeSurface_ByCamera.h"

#include "Layout/Shape_ConeSurface.h"


namespace Layout {

ShapeGetter_ConeSurface_ByCamera::ShapeGetter_ConeSurface_ByCamera(const QOSG::ViewerQT *viewerWidget,
																   const QSlider &baseRadiusSpinBox) :
	mViewerWidget(viewerWidget),
	mBaseRadiusSlider(baseRadiusSpinBox)
{
}

QSharedPointer<Shape> ShapeGetter_ConeSurface_ByCamera::getShape()
{
	return QSharedPointer<Shape>( new Shape_ConeSurface(mViewerWidget, mBaseRadiusSlider) );
}
QSet<Data::Node *> ShapeGetter_ConeSurface_ByCamera::getNodesOfShape()
{
	//implement
	QSet<Data::Node *  >  nodes;
	return nodes;
}


}
