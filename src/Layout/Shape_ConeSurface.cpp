#include "Layout/Shape_ConeSurface.h"

#include "Layout/ShapeVisitor.h"
#include "QOSG/ViewerQT.h"

#include "Viewer/CameraManipulator.h"

namespace Layout {

Shape_ConeSurface::Shape_ConeSurface( const QOSG::ViewerQT* viewerWidget, const QSpinBox& baseRadiusSpinBox ) :
	mViewerWidget( viewerWidget ),
	mBaseRadiusSpinBox( baseRadiusSpinBox )
{
}

void Shape_ConeSurface::accept( ShapeVisitor& visitor )
{
	visitor.visit( *this );
}

osg::Vec3f Shape_ConeSurface::baseCenter() const
{
	osg::Vec3f eye;
	osg::Vec3f center;
	osg::Vec3f up;
	mViewerWidget->getCamera()->getViewMatrixAsLookAt( eye, center, up );

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

	return ( static_cast<float>( mBaseRadiusSpinBox.value() ) / 100.0f ) * 250.f;
}

}
