#include "Layout/Shape_CylinderSurface.h"

#include "Layout/ShapeVisitor.h"
#include "QOSG/ViewerQT.h"

namespace Layout {

Shape_CylinderSurface::Shape_CylinderSurface( const QOSG::ViewerQT* viewerWidget, const QSpinBox& baseRadiusSpinBox ) :
	mViewerWidget( viewerWidget ),
	mBaseRadiusSpinBox( baseRadiusSpinBox )
{
}

void Shape_CylinderSurface::accept( ShapeVisitor& visitor )
{
	visitor.visit( *this );
}

osg::Vec3f Shape_CylinderSurface::firstBaseCenter() const
{
	osg::Vec3f eye;
	osg::Vec3f center;
	osg::Vec3f up;
	mViewerWidget->getCamera()->getViewMatrixAsLookAt( eye, center, up );

	return eye;
}

osg::Vec3f Shape_CylinderSurface::secondBaseCenter() const
{
	osg::Vec3f eye;
	osg::Vec3f center;
	osg::Vec3f up;
	mViewerWidget->getCamera()->getViewMatrixAsLookAt( eye, center, up );

	return center;
}

float Shape_CylinderSurface::baseRadius() const
{

	return ( static_cast<float>( mBaseRadiusSpinBox.value() ) / 100.0f ) * 250.f;
}

}
