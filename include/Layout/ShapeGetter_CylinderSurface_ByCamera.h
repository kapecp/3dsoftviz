#ifndef Layout_ShapeGetter_CylinderSurface_ByCamera_H
#define Layout_ShapeGetter_CylinderSurface_ByCamera_H

#include "Layout/ShapeGetter.h"

#include <QSpinBox>

#include "Data/Node.h"
#include "QOSG/ViewerQT.h"

namespace Layout {

/**
 * \brief Returning a cylinder surface.
 * Center is defined by the centerNode position.
 * Radius is defined by the surfaceNode position (the radius is computed in a way that
 * surfaceNode will be placed on the surface of the resulting sphere surface).
 */
class ShapeGetter_CylinderSurface_ByCamera : public ShapeGetter
{
public:
	ShapeGetter_CylinderSurface_ByCamera( const QOSG::ViewerQT* viewerWidget, const QSpinBox& baseRadiusSpinBox );

	virtual ~ShapeGetter_CylinderSurface_ByCamera() {}

	virtual QSharedPointer<Shape> getShape();
	virtual QSet<Data::Node*> getNodesOfShape();

private:
	const QOSG::ViewerQT* mViewerWidget;
	const QSpinBox& mBaseRadiusSpinBox;
};

} // namespace

#endif // Layout_ShapeGetter_CylinderSurface_ByCamera_H
