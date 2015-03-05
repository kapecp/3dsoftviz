#include "Layout/ShapeGetter_CylinderSurface_ByCamera.h"

#include "Layout/Shape_CylinderSurface.h"


namespace Layout {

ShapeGetter_CylinderSurface_ByCamera::ShapeGetter_CylinderSurface_ByCamera( const QOSG::ViewerQT* viewerWidget,
		const QSpinBox& baseRadiusSpinBox ) :
	mViewerWidget( viewerWidget ),
	mBaseRadiusSpinBox( baseRadiusSpinBox )
{
}

QSharedPointer<Shape> ShapeGetter_CylinderSurface_ByCamera::getShape()
{
	return QSharedPointer<Shape>( new Shape_CylinderSurface( mViewerWidget, mBaseRadiusSpinBox ) );
}
QSet<Data::Node*> ShapeGetter_CylinderSurface_ByCamera::getNodesOfShape()
{
	//implement
	QSet<Data::Node*  >  nodes;
	return nodes;
}
}
