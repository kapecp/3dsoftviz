#ifndef Viewer_ShapeVisitor_VisualizerCreator_H
#define Viewer_ShapeVisitor_VisualizerCreator_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
#include "Layout/Shape.h"
//-----------------------------------------------------------------------------
#include <osg/Node>
//-----------------------------------------------------------------------------

namespace Vwr {

class ShapeVisitor_VisualizerCreator : public Layout::ShapeVisitor {

public:

	/***/
	virtual ~ShapeVisitor_VisualizerCreator (void) {};

	osg::Node * getCreatedVisualizer (void);

	virtual void visit (Layout::Shape_Composite & shape);
	virtual void visit (Layout::Shape_Plane & shape);
	virtual void visit (Layout::Shape_Sphere & shape);
	virtual void visit (Layout::Shape_SphereSurface & shape);

private: // visitor context

	osg::Node * createdVisualizer_;

}; // class

} // namespace

#endif // Viewer_ShapeVisitor_VisualizerCreator_H
