#ifndef Viewer_ShapeVisitor_VisualizerCreator_H
#define Viewer_ShapeVisitor_VisualizerCreator_H
//-----------------------------------------------------------------------------
#include "Layout/ShapeVisitor.h"
//-----------------------------------------------------------------------------
#include <osg/Node>
#include <osg/Vec3f>
//-----------------------------------------------------------------------------

namespace Vwr {

/**
 * \brief Creates visualizations of provided shapes.
 * Usage:
 * 1. Accept the visitor by the shape definition to visualize.
 * 2. Call getCreatedVisualizer to get created OSG objects which visualize the restriction.
 * [visitor pattern]
 */
class ShapeVisitor_VisualizerCreator : public Layout::ShapeVisitor
{

public:

	/***/
	virtual ~ShapeVisitor_VisualizerCreator( void ) {}

	osg::ref_ptr<osg::Node> getCreatedVisualizer( void );

	/**
	 * \brief Creates empty OSG group.
	 */
	virtual void visit( Layout::Shape_Null /*& shape*/ );

	/**
	 * \brief Creates OSG group containing visualizer for each shape contained in the composite shape.
	 */
	virtual void visit( Layout::Shape_Composite& shape );

	/**
	     * \brief Creates OSG group containing visualizer for intersection of shapes contained in the composite shape.
	     */
	virtual void visit( Layout::Shape_Intersection& shape );

	/**
	 * \brief Creates OSG geode visualizing the plane which conforms the plane position and rotation.
	 */
	virtual void visit( Layout::Shape_Plane& shape );

	/**
	 * \brief Creates OSG geode visualizing the sphere which conforms the sphere center and radius.
	 */
	virtual void visit( Layout::Shape_Sphere& shape );

	/**
	 * \brief Creates OSG geode visualizing the sphere which conforms the sphere surface center and radius.
	 */
	virtual void visit( Layout::Shape_SphereSurface& shape );

	/**
	 * \brief Creates OSG geode visualizing the cylinder which conforms the cylinder surface center and radius.
	 */
	virtual void visit( Layout::Shape_CylinderSurface& shape );

	/**
	 * \brief Creates OSG geode visualizing the cone which conforms the cone surface center and radius.
	 */
	virtual void visit( Layout::Shape_ConeSurface& shape );

	virtual void visit( Layout::Shape_Cube& shape );

private: // visitor context

	osg::ref_ptr<osg::Node> createdVisualizer_;

private: // common visualizers

	/**
	 * \brief Creates sphere visualizer (used to visualize Sphere and SphereSurface).
	 */
	void visualizeSphere( Layout::Shape_AbstractSphere& abstractSphere );

	/**
	 * \brief Creates cylinder visualizer (used to visualize CylinderSurface).
	 */
	void visualizeCylinder( Layout::Shape_CylinderSurface& cylinder );

	/**
	 * \brief Creates cone visualizer (used to visualize ConeSurface).
	 */
	void visualizeCone( Layout::Shape_ConeSurface& cone );

	void visualizeCube( Layout::Shape_AbstractCube& abstractCube );

	/**
	 * \brief Creating geomtetry of grid
	 */
	osg::Geometry* createGridGeometry( unsigned int size, unsigned int noOfLines, osg::Vec3f center );

private: // positions and distances

	/**
	 * \brief Multiplies the distance by the current graph scale.
	 */
	float getScaledDistance(
		const float& distance
	);

	/**
	 * \brief Multiplies the position by the current graph scale.
	 */
	osg::Vec3f getScaledPosition(
		const osg::Vec3f& position
	);

	/**
	 * \brief Gets current graph scale as set in the application configuration.
	 */
	float getScale();

}; // class

} // namespace

#endif // Viewer_ShapeVisitor_VisualizerCreator_H
