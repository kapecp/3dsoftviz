#include "Viewer/ShapeVisitor_VisualizerCreator.h"
//-----------------------------------------------------------------------------
#include "Util/ApplicationConfig.h"
//-----------------------------------------------------------------------------
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/BlendFunc>
//volovar pridanie wireframe modu pre radial layout
#include <osg/PolygonMode>
//volovar koniec
//-----------------------------------------------------------------------------

namespace Vwr {

osg::ref_ptr<osg::Node> ShapeVisitor_VisualizerCreator::getCreatedVisualizer( void )
{
	return createdVisualizer_;
}

void ShapeVisitor_VisualizerCreator::visit( Layout::Shape_Null /*& shape*/ )
{
	createdVisualizer_ = new osg::Group;
}

void ShapeVisitor_VisualizerCreator::visit( Layout::Shape_Composite& shape )
{
	osg::Group* group = new osg::Group;
	Layout::Shape_Composite::ShapesListType& shapes = shape.getShapes();
	for ( Layout::Shape_Composite::ShapesListType::iterator it = shapes.begin(); it != shapes.end(); ++it ) {
		( *it )->accept( *this );
		group->addChild( createdVisualizer_ );
	}

	createdVisualizer_ = group;
}


void ShapeVisitor_VisualizerCreator::visit( Layout::Shape_Intersection& shape )
{

	osg::Vec3 center;
	//osg::Vec3 normalVector;

	if ( shape.getCompositeType() == Layout::Shape_Composite::CompositeType::CIRCLE ) {
		float radius = 0;

		Layout::Shape_Composite::ShapesListType& shapes = shape.getShapes();
		Layout::Shape_Composite::ShapesListType::iterator it = shapes.begin();

		if ( QSharedPointer<Layout::Shape_Sphere> sphere = qSharedPointerCast<Layout::Shape_Sphere>( *it ) ) {
			radius = sphere->getRadius();
			center = sphere->getCenter();
		}
		else {
			// TODO: What if the first condition does not apply
		}
		++it;

		//if(QSharedPointer<Layout::Shape_Plane> plane = qSharedPointerCast<Layout::Shape_Plane>(*it)){
		//   normalVector = plane->getNormalVector();
		//}

		osg::Cylinder* cylinder = new osg::Cylinder;
		cylinder->setCenter( getScaledPosition( center ) );
		cylinder->setRadius( getScaledDistance( radius ) );
		cylinder->setHeight( 1 );

		osg::ShapeDrawable* sd = new osg::ShapeDrawable;
		sd->setShape( cylinder );

		//volovar zmena apha nie je konstanta 0.06f
		sd->setColor( osg::Vec4( 0.0f, 0.f, 1.0f, shape.getAlpha() ) );

		sd->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
		sd->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
		sd->getStateSet()->setMode( GL_DEPTH_TEST,osg::StateAttribute::OFF );
		sd->getStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
		sd->getStateSet()->setAttributeAndModes( new osg::BlendFunc, osg::StateAttribute::ON );
		sd->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
		sd->getStateSet()->setRenderBinDetails( 11, "RenderBin" );

		osg::Geode* geode = new osg::Geode;
		geode->addDrawable( sd );

		createdVisualizer_ = geode;

	}

}

void ShapeVisitor_VisualizerCreator::visit( Layout::Shape_Plane& shape )
{
	// OSG does not support InfinitePlane drawing at this time, maybe try to use this piece of code with the new version of OSG
	/*
	osg::InfinitePlane * plane = new osg::InfinitePlane;
	plane->set (shape.getNormalVector (), getScaledDistance (shape.getD ()));

	osg::ShapeDrawable * sd = new osg::ShapeDrawable;
	sd->setShape (plane);
	sd->setColor (osg::Vec4 (0, 0.5, 0.0, 0.5));

	now we use a temporary code instead - drawing a plane using box:
	if(shape.isInvisible()) {
	    createdVisualizer_ = new osg::Group;
	    return;
	}
	*/

	// position of center point
	osg::Vec3f center( 0, 0, ( -shape.getD() ) / shape.getNormalVector().z() ); // some point on the plane
	//qDebug() << "center: " << center.z();

	// size of grid
	unsigned int size = 1000;
	// number of lines on one side of grid
	unsigned int noOfLines = 10;

	// compute 4 corners of grid based on center point
	// osg::Vec3Array* gridVertices = new osg::Vec3Array;
	// gridVertices->push_back( center + osg::Vec3f( -size/2, size/2, 0) ); // top left
	// gridVertices->push_back( center + osg::Vec3f(size/2, size/2, 0) ); // top right
	// gridVertices->push_back( center + osg::Vec3f(-size/2,-size/2, 0) ); // bottom right
	// gridVertices->push_back( center + osg::Vec3f( size/2,-size/2, 0) ); // bottom left

	osg::Geometry* gridGeometry;
	gridGeometry = createGridGeometry( size, noOfLines, center );

	// creating new geode
	osg::Geode* geode = new osg::Geode;

	// assign geometry
	geode->addDrawable( gridGeometry );
	// turn off GL_LIGHTNING
	gridGeometry->getOrCreateStateSet()->setMode( GL_LIGHTING,osg::StateAttribute::OFF );
	// setting color of grid
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back( osg::Vec4( 0.0f, 0.0f, 1.0f, 0.6f ) ); // blue
	gridGeometry->setColorArray( colors );
	gridGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );

	createdVisualizer_ = geode;
}

osg::Geometry* ShapeVisitor_VisualizerCreator::createGridGeometry( unsigned int size, unsigned int noOfLines, osg::Vec3f center )
{

	osg::Geometry* geom = new osg::Geometry();
	osg::Vec3Array* positionsOfLines = new osg::Vec3Array;

	geom->setVertexArray( positionsOfLines );
	int sizeNegative = -1 * size;

	// position of borders
	osg::Vec3f dif1( static_cast<osg::Vec3f::value_type>( sizeNegative/2 ), static_cast<osg::Vec3f::value_type>( size/2 ), -10 ); // top left
	osg::Vec3f dif2( static_cast<osg::Vec3f::value_type>( sizeNegative/2 ), static_cast<osg::Vec3f::value_type>( sizeNegative/2 ), -10 );
	osg::Vec3f dif3( static_cast<osg::Vec3f::value_type>( size/2 ), static_cast<osg::Vec3f::value_type>( size/2 ), -10 );

	// increment for creating points of mesh at border
	osg::Vec3f incrementX( static_cast<osg::Vec3f::value_type>( size/noOfLines ), 0, 0 );
	osg::Vec3f incrementY( 0, static_cast<osg::Vec3f::value_type>( sizeNegative/noOfLines ), 0 );

	// computing and saving positions of points at border of mesh
	for ( int i=0; i<2; i++ ) {

		osg::Vec3f base1 = center + dif1;
		osg::Vec3f base2 = center + dif2;

		// computing positions at x axis
		for ( unsigned int j=0; j<noOfLines; j++ ) {
			positionsOfLines->push_back( base1 + incrementX );
			base1 += incrementX;
			positionsOfLines->push_back( base2 + incrementX + incrementY );
			base2 += incrementX;
		}

		base1 = center + dif1;
		base2 = center + dif3;

		// computing positions at y axis
		for ( unsigned int j=0; j<noOfLines; j++ ) {
			positionsOfLines->push_back( base1 + incrementY );
			base1 += incrementY;
			positionsOfLines->push_back( base2 + incrementY + incrementX );
			base2 += incrementY;
		}

		// connecting two corresponding points at border of mesh
		for ( unsigned int k=0; k<noOfLines*4; k=k+2 ) {
			osg::DrawElementsUInt* line = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
			line->push_back( k );
			line->push_back( k+1 );
			geom->addPrimitiveSet( line );
		}
	}

	return geom;
}

void ShapeVisitor_VisualizerCreator::visit( Layout::Shape_Sphere& shape )
{
	visualizeSphere( shape );
}

void ShapeVisitor_VisualizerCreator::visit( Layout::Shape_SphereSurface& shape )
{
	visualizeSphere( shape );
}

void ShapeVisitor_VisualizerCreator::visit( Layout::Shape_CylinderSurface& shape )
{
	visualizeCylinder( shape );
}

void ShapeVisitor_VisualizerCreator::visit( Layout::Shape_ConeSurface& shape )
{
	visualizeCone( shape );
}

void ShapeVisitor_VisualizerCreator::visit( Layout::Shape_Cube& shape )
{
	visualizeCube( shape );
}

void ShapeVisitor_VisualizerCreator::visualizeSphere( Layout::Shape_AbstractSphere& abstractSphere )
{
	osg::Sphere* sphere = new osg::Sphere;
	sphere->setRadius( getScaledDistance( abstractSphere.getRadius() ) );
	sphere->setCenter( getScaledPosition( abstractSphere.getCenter() ) );

	osg::ShapeDrawable* sd = new osg::ShapeDrawable;
	sd->setShape( sphere );
	//volovar zmena
	osg::PolygonMode* polygonMode = new osg::PolygonMode;  //podla http://snipplr.com/view/30978/osg-wireframe-display/

	sd->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
	switch ( abstractSphere.getRenderType() ) { //prepinanie medzi vykreslovacimi modmi shapu
		case Layout::Shape_AbstractSphere::SOLID:

			sd->setColor( osg::Vec4( 0.f, 0.f, 1.0f, abstractSphere.getAlpha() ) );
			sd->getStateSet()->setAttributeAndModes( new osg::BlendFunc, osg::StateAttribute::ON );
			sd->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
			break;
		case Layout::Shape_AbstractSphere::WIREFRAME:
			polygonMode->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE );
			sd->setColor( osg::Vec4( 0.f, 0.f, 1.0f, abstractSphere.getAlpha() ) );
			sd->getStateSet()->setAttributeAndModes( polygonMode,
					osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON );
			sd->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
			break;
		case Layout::Shape_AbstractSphere::CIRCLE:
			break;
		default:
			return;
	}
	//volovar koniec zmeny


	//transparency of sphere

	sd->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	sd->getStateSet()->setMode( GL_DEPTH_TEST,osg::StateAttribute::OFF );
	sd->getStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

	sd->getStateSet()->setRenderBinDetails( 11, "RenderBin" );
	//

	osg::Geode* geode = new osg::Geode;
	geode->addDrawable( sd );

	createdVisualizer_ = geode;
}

void ShapeVisitor_VisualizerCreator::visualizeCylinder( Layout::Shape_CylinderSurface& cylinder )
{
	osg::Vec3f center = cylinder.secondBaseCenter();

	osg::Cylinder* osgCylinder = new osg::Cylinder;
	osgCylinder->setRadius( getScaledDistance( 100 ) );
	osgCylinder->setCenter( getScaledPosition( center ) );

	osg::ShapeDrawable* sd = new osg::ShapeDrawable;
	sd->setShape( osgCylinder );
	sd->setColor( osg::Vec4( 0, 0, 1.0, /*0.06*/0.0 ) );

	//transparency of cylinder
	sd->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
	sd->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	sd->getStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
	sd->getStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	sd->getStateSet()->setAttributeAndModes( new osg::BlendFunc, osg::StateAttribute::ON );
	sd->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	sd->getStateSet()->setRenderBinDetails( 11, "RenderBin" );
	//

	osg::Geode* geode = new osg::Geode;
	geode->addDrawable( sd );

	createdVisualizer_ = geode;
}

void ShapeVisitor_VisualizerCreator::visualizeCone( Layout::Shape_ConeSurface& cone )
{
	osg::Vec3f center = cone.spike();

	osg::Cone* osgCone = new osg::Cone;
	osgCone->setRadius( getScaledDistance( 100 ) );
	osgCone->setCenter( getScaledPosition( center ) );

	osg::ShapeDrawable* sd = new osg::ShapeDrawable;
	sd->setShape( osgCone );
	sd->setColor( osg::Vec4( 0, 0, 1.0, /*0.06*/0.0 ) );

	//transparency of cone
	sd->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
	sd->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	sd->getStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
	sd->getStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	sd->getStateSet()->setAttributeAndModes( new osg::BlendFunc, osg::StateAttribute::ON );
	sd->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	sd->getStateSet()->setRenderBinDetails( 11, "RenderBin" );
	//

	osg::Geode* geode = new osg::Geode;
	geode->addDrawable( sd );

	createdVisualizer_ = geode;
}

void ShapeVisitor_VisualizerCreator::visualizeCube( Layout::Shape_AbstractCube& cube )
{
//    if(cube.isInvisible()) {
	createdVisualizer_ = new osg::Group;
//        return;
//    }
//    osg::Box *osgBox = new osg::Box(getScaledPosition(cube.getCenter()), getScaledDistance(cube.getRadius() * 2));

//    osg::ShapeDrawable *sd = new osg::ShapeDrawable;
//    sd->setShape(osgBox);
//    sd->setColor(osg::Vec4 (0, 0, 1.0, 0.06));

//    //transparency of cube
//    sd->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
//    sd->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//    sd->getStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
//    sd->getStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
//    sd->getStateSet()->setAttributeAndModes(new osg::BlendFunc, osg::StateAttribute::ON);
//    sd->getStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
//    sd->getStateSet()->setRenderBinDetails(11, "RenderBin");
//    //

//    osg::Geode *geode = new osg::Geode;
//    geode->addDrawable(sd);

//    createdVisualizer_ = geode;
}

float ShapeVisitor_VisualizerCreator::getScaledDistance(
	const float& distance
)
{
	return distance * getScale();
}

osg::Vec3f ShapeVisitor_VisualizerCreator::getScaledPosition(
	const osg::Vec3f& position
)
{
	return position * getScale();
}

float ShapeVisitor_VisualizerCreator::getScale()
{
	return Util::ApplicationConfig::get()->getValue( "Viewer.Display.NodeDistanceScale" ).toFloat();
}

} // namespace
