#include "Math/CameraMath.h"

#include "Util/ApplicationConfig.h"

#include <osg/Camera>
#include <osg/LineWidth>
#include <osg/Geometry>

#include <cmath>
#include <list>

namespace MathModule {

osg::Vec3d CameraMath::getPointOnNextBezierCurve( double time, QVector<osg::Vec3d>* points, double weights[] )
{
	int n = points->size() - 1;
	double denominator = 0;
	double x, y, z;

	x = y = z = 0;

	for ( int i = 0; i <= n; i++ ) {
		double x_0 = factorial( n )/( factorial( i ) * factorial( n - i ) );
		double x_1 = pow( time, i );
		double x_2 = pow( 1 - time, n - i ) * weights[i];

		double numerator = x_0 * x_1 * x_2;

		x += numerator * points->at( i ).x();
		y += numerator * points->at( i ).y();
		z += numerator * points->at( i ).z();
	}

	for ( int i = 0; i <= n; i++ ) {
		double x_0 = factorial( n )/( factorial( i ) * factorial( n - i ) );
		double x_1 = pow( time, i );
		double x_2 = pow( 1 - time, n - i ) * weights[i];

		denominator += x_0 * x_1 * x_2;
	}

	x /= denominator;
	y /= denominator;
	z /= denominator;

	return osg::Vec3d( x, y, z );
}

int CameraMath::factorial( int n )
{
	if ( n <= 1 ) {  // base case
		return 1;
	}
	else {
		return n * factorial( n - 1 );
	}
}

QVector<osg::ref_ptr<Data::Node> >* CameraMath::getViewExtremes( osg::ref_ptr<osg::Camera> camera, std::list<osg::ref_ptr<Data::Node> > selectedCluster )
{
	osg::Matrixd mv = camera->getViewMatrix();
	osg::Matrixd mp = camera->getProjectionMatrix();
	osg::Matrixd mw = camera->getViewport()->computeWindowMatrix();

	QVector<osg::ref_ptr<Data::Node> >* extremes = new QVector<osg::ref_ptr<Data::Node> >;

	osg::Vec3d leftPosition, rightPosition, topPosition, bottomPosition;

	for ( int x = 0; x < 4; x++ ) {
		extremes->push_back( selectedCluster.front() );
	}

	float scale = Util::ApplicationConfig::get()->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();

	osg::Vec3d p = selectedCluster.front()->getCurrentPosition() * scale;

	// get initial onscreen position
	leftPosition = rightPosition = topPosition = bottomPosition = p * mv * mp * mw;

	std::list<osg::ref_ptr<Data::Node> >::iterator i;

	// find onscreen extremes in cluster
	for ( i = selectedCluster.begin(); i != selectedCluster.end(); ++i ) {
		osg::Vec3d position = ( *i )->getCurrentPosition() * scale * mv * mp * mw;

		if ( position.x() < leftPosition.x() ) {
			extremes->replace( 0, *i );
			leftPosition = position;
		}

		if ( position.x() > rightPosition.x() ) {
			extremes->replace( 1, *i );
			rightPosition = position;
		}

		if ( position.y() < topPosition.y() ) {
			extremes->replace( 2, *i );
			topPosition = position;
		}

		if ( position.y() > bottomPosition.y() ) {
			extremes->replace( 3, *i );
			bottomPosition = position;
		}
	}


	return extremes;
}

osg::Vec3d CameraMath::projectOnScreen( osg::ref_ptr<osg::Camera> camera, osg::Vec3d point )
{
	osg::Matrixd mv = camera->getViewMatrix();
	osg::Matrixd mp = camera->getProjectionMatrix();
	osg::Matrixd mw = camera->getViewport()->computeWindowMatrix();

	osg::Vec3d result = point * mv;
	result = result * mp;
	result = result * mw;

	return result;
}


osg::Vec3d CameraMath::getPointOnVector( osg::Vec3d p1, osg::Vec3d p2, double distance )
{
	//x = x1 + (x2-x1)*(d/D) = x1(1-(d/D)) + x2(d/D)
	osg::Vec3d directionVec =  p1 - p2;
	double distanceRatio = distance / directionVec.length();

	return p1 * ( 1 - distanceRatio ) + p2 * distanceRatio;
}

bool CameraMath::isInRect( osg::Vec3d point, double width, double height, double margin )
{
	if ( point.x() < margin || point.x() >  width - margin || point.y() < margin || point.y() > height - margin ) {
		return false;
	}
	else {
		return true;
	}
}

bool CameraMath::isInFOV( osg::Vec3d point, osg::ref_ptr<osg::Camera> camera )
{
	double left, right, bottom, top, zNear, zFar;
	camera->getProjectionMatrixAsFrustum( left, right, bottom, top, zNear, zFar );

	osg::Matrixd& mv = camera->getViewMatrix();
	osg::Matrixd& mp = camera->getProjectionMatrix();
	osg::Vec3d pView = point * mv * mp;


	if ( pView.z() < 0.0 ) {
		return false;
	}

	return true;
}

osg::Drawable* CameraMath::createAxis( const osg::Vec3& corner,const osg::Vec3& dir, osg::Vec4 color )
{
	// set up the Geometry.
	osg::Geometry* geom = new osg::Geometry;

	osg::Vec3Array* coords = new osg::Vec3Array( 2 );
	( *coords )[0] = corner;
	( *coords )[1] = dir;

	geom->setVertexArray( coords );

	// set colors
	osg::Vec4Array* clr = new osg::Vec4Array( 2 );
	( *clr )[0] = color;
	( *clr )[1] = color;

	geom->setColorArray( clr );

#ifdef BIND_PER_PRIMITIVE
	geom->setColorBinding( osg::Geometry::BIND_PER_PRIMITIVE );
#else
	geom->setColorBinding( osg::Geometry::BIND_PER_PRIMITIVE_SET );
#endif

	geom->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::LINES,0,2 ) );

	// set line parameters
	osg::StateSet* stateset = new osg::StateSet;
	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth( 4.0f );
	stateset->setAttributeAndModes( linewidth,osg::StateAttribute::ON );
	stateset->setMode( GL_LIGHTING,osg::StateAttribute::OFF );
	geom->setStateSet( stateset );

	return geom;
}

} // namespace MathModule
