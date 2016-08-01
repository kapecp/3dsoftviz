#pragma once

#include <osg/Vec3d>
#include <osg/ref_ptr>
#include <QVector>
#include "Data/Node.h"

#include <list>

namespace MathModule {

class CameraMath
{
public:
	/*!
	 * \brief
	 * Computes position along curve in given
	 *
	 * \param time
	 * position time
	 *
	 * \param points
	 * control points
	 *
	 * \param weights
	 * control points weights
	 *
	 * \returns
	 * new point
	 */
	static osg::Vec3d getPointOnNextBezierCurve( double time, QVector<osg::Vec3d>* points, double weights[] );

	/*!
	 * \brief
	 * computes factorial
	 *
	 * \param n
	 * factorial of n
	 *
	 * \returns
	 * result
	 */
	static int factorial( int n );

	/*!
	 * \brief
	 * selects nodes in extremes of screen in t = 0.5
	 *
	 * \param camera
	 * camera
	 *
	 * \param selectedCluster
	 * selected nodes
	 *
	 * \returns
	 * nodes in extremes
	 */
	static QVector<osg::ref_ptr<Data::Node> >* getViewExtremes( osg::ref_ptr<osg::Camera> camera, std::list<osg::ref_ptr<Data::Node> > selectedCluster );

	/*!
	 * \brief
	 * returns screen coordinates of node
	 *
	 * \param camera
	 * camera
	 *
	 * \param point
	 * node coordinates
	 *
	 * \returns
	 * node screen coordinates
	 */
	static osg::Vec3d projectOnScreen( osg::ref_ptr<osg::Camera> camera, osg::Vec3d point );

	/*!
	 * \brief
	 * moves point along vector
	 *
	 * \param p1
	 * source point
	 *
	 * \param p2
	 * destination point
	 *
	 * \param distance
	 * distance along vector
	 *
	 * \returns
	 * point coordinates
	 */

	static osg::Vec3d getPointOnVector( osg::Vec3d p1, osg::Vec3d p2, double distance );

	/*!
	 * \brief
	 * Checks if point is in rectangle starting at 0,0
	 *
	 * \param point
	 * point position
	 *
	 * \param width
	 * rectangle width
	 *
	 * \param height
	 * rectangle height
	 *
	 * \param margin
	 * rectangle margin
	 *
	 * \returns
	 * true, if point is in rectangle
	 */
	static bool isInRect( osg::Vec3d point, double width, double height, double margin );


	/*!
	 * \brief
	 * checks if point is in camera FOV
	 *
	 * \param point
	 * point
	 *
	 * \param camera
	 * camera
	 *
	 * \returns
	 * true, if point is in FOV
	 */
	static bool isInFOV( osg::Vec3d point, osg::ref_ptr<osg::Camera> camera );

	/*!
	 * \brief
	 * Creates line between two points
	 *
	 * \param corner
	 * source point
	 *
	 * \param xdir
	 * target point
	 *
	 * \param color
	 * line color
	 *
	 * \returns
	 * line drawable
	 */
	static osg::Drawable* createAxis( const osg::Vec3& corner,const osg::Vec3& xdir, osg::Vec4 color = osg::Vec4d( 0.0f,0.0f,1.0f,0.0f ) );
};
}
