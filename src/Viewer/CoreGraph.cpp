#include "Viewer/CoreGraph.h"

#include "Viewer/CameraManipulator.h"
#include "Viewer/SkyBox.h"
#include "Viewer/EdgeGroup.h"
#include "Viewer/NodeGroup.h"
#include "Viewer/PerlinNoiseTextureGenerator.h"
#include "Viewer/SkyTransform.h"
#include "Viewer/TextureWrapper.h"
#include "Viewer/DataHelper.h"
#include "Leap/LeapLib/LeapCameraStream.h"

#include <QDebug>
#include <QMatrix4x4>

#include <osgManipulator/TranslateAxisDragger>

#include "Network/Server.h"
#include "Data/Graph.h"

#include "Clustering/Clusterer.h"
#include "Clustering/Figures/Cube.h"
#include "Clustering/Figures/Sphere.h"

#include "Util/ApplicationConfig.h"

#ifdef OPENCV_FOUND
#include "OpenCV/CameraStream.h"
#endif

#include <osgUtil/Optimizer>
#include <osg/Depth>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osgFX/Outline>
#include <osg/ValueObject>

#include <math.h>

#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowMap>
#include <osgShadow/SoftShadowMap>

#include <easylogging++.h>

namespace Vwr {

/*
* TODO prerobit - v sucastnosti je scena jeden velky plochy graf. toto sa da optimalizovat do stromovej strukutry. pri vytvarani grafu ho treba prechadzat ako graf
* a nie vsetko zaradom ako je to teraz
*/

osg::ref_ptr<osg::AutoTransform> getSphere( qlonglong id, osg::Vec3 position, float radius, osg::Vec4 color )
{
	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
	at->setPosition( position * 1 );
	//at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

	osg::ShapeDrawable* shape = new osg::ShapeDrawable;
	osg::Sphere* sphere = new osg::Sphere;
	sphere->setRadius( radius );
	shape->setShape( sphere );
	shape->setColor( color ); //osg::Vec4(0.9, 0.1, 0.3, 0.5));
	shape->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
	shape->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	osg::Geode* geode = new osg::Geode;
	geode->addDrawable( shape );

	geode->setUserValue( "id", QString::number( id ).toStdString() );

	at->addChild( geode );
	return at;
}

osg::ref_ptr<osg::AutoTransform> getCube( qlonglong id, osg::Vec3 position, float width, osg::Vec4 color )
{
	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
	at->setPosition( position * 1 );
	//at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

	osg::ShapeDrawable* shape = new osg::ShapeDrawable;
	osg::Box* cube = new osg::Box;
	cube->setHalfLengths( osg::Vec3( width,width,width ) );
	shape->setShape( cube );
	shape->setColor( color ); //osg::Vec4(0.9, 0.1, 0.3, 0.5));
	shape->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
	shape->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	osg::Geode* geode = new osg::Geode;
	geode->addDrawable( shape );

	geode->setUserValue( "id", QString::number( id ).toStdString() );

	at->addChild( geode );
	return at;
}

osg::ref_ptr<osg::AutoTransform> getCone( qlonglong id, osg::Vec3 position, float radius, osg::Vec4 color )
{
	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
	at->setPosition( position * 1 );
	//at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

	osg::ShapeDrawable* shape = new osg::ShapeDrawable;
	osg::Cone* cone = new osg::Cone;
	cone->setRadius( radius );
	cone->setHeight( radius*3 );
	shape->setShape( cone );
	shape->setColor( color ); //osg::Vec4(0.9, 0.1, 0.3, 0.5));
	shape->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
	shape->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	osg::Geode* geode = new osg::Geode;
	geode->addDrawable( shape );

	geode->setUserValue( "id", QString::number( id ).toStdString() );

	at->addChild( geode );

	return at;
}

osg::ref_ptr<osg::AutoTransform> getCylinder( qlonglong id, osg::Vec3 position, float radius, osg::Vec4 color )
{
	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
	at->setPosition( position * 1 );
	//at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

	osg::ShapeDrawable* shape = new osg::ShapeDrawable;
	osg::Cylinder* cylinder = new osg::Cylinder;
	cylinder->setRadius( radius );
	cylinder->setHeight( radius*2 );
	shape->setShape( cylinder );
	shape->setColor( color ); //osg::Vec4(0.9, 0.1, 0.3, 0.5));
	shape->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
	shape->getStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	osg::Geode* geode = new osg::Geode;
	geode->addDrawable( shape );

	geode->setUserValue( "id", QString::number( id ).toStdString() );

	at->addChild( geode );

	return at;
}

osg::Vec3f getMidPoint( QSet<Data::Node*> nodes )
{

	osg::Vec3 total = osg::Vec3( 0,0,0 );
	float count = 0;

	QSet<Data::Node*>::const_iterator i = nodes.constBegin();
	while ( i != nodes.constEnd() ) {

		Data::Node* v = *i;

		osg::Vec3f pos = v->getCurrentPosition();

		total += pos;

		++i;
		++count;
	}
	total.operator /=( count );

	return total;
}

double getRadius( QSet<Data::Node*> nodes, osg::Vec3f midPoint )
{

	double maxDistance = 0;

	QSet<Data::Node*>::const_iterator i = nodes.constBegin();
	while ( i != nodes.constEnd() ) {
		Data::Node* v = *i;
		osg::Vec3f pos = v->getCurrentPosition();

		double newDistance = sqrt( pow( static_cast<double>( pos.x()-midPoint.x() ),2 ) + pow( static_cast<double>( pos.y()-midPoint.y() ),2 ) + pow( static_cast<double>( pos.z()-midPoint.z() ),2 ) );

		if ( newDistance > maxDistance ) {
			maxDistance = newDistance;
		}
		++i;
	}
	return maxDistance;
}

osg::Geode* test()
{
	//cout << " test ...";
	osg::Geode* pyramidGeode = new osg::Geode();
	osg::Geometry* pyramidGeometry = new osg::Geometry();
	pyramidGeode->addDrawable( pyramidGeometry );

	osg::Vec3Array* pyramidVertices = new osg::Vec3Array;
	pyramidVertices->push_back( osg::Vec3( 0, 0, 0 ) ); // front left
	pyramidVertices->push_back( osg::Vec3( 100, 0, 0 ) ); // front right
	pyramidVertices->push_back( osg::Vec3( 100,100, 0 ) ); // back right
	pyramidVertices->push_back( osg::Vec3( 0,100, 0 ) ); // back left
	pyramidVertices->push_back( osg::Vec3( 50, 50,100 ) ); // peak

	pyramidGeometry->setVertexArray( pyramidVertices );

	osg::DrawElementsUInt* pyramidBase =
		new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
	pyramidBase->push_back( 3 );
	pyramidBase->push_back( 2 );
	pyramidBase->push_back( 1 );
	pyramidBase->push_back( 0 );
	pyramidGeometry->addPrimitiveSet( pyramidBase );

	osg::DrawElementsUInt* pyramidFaceOne =
		new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
	pyramidFaceOne->push_back( 0 );
	pyramidFaceOne->push_back( 1 );
	pyramidFaceOne->push_back( 4 );
	pyramidGeometry->addPrimitiveSet( pyramidFaceOne );

	osg::DrawElementsUInt* pyramidFaceTwo =
		new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
	pyramidFaceTwo->push_back( 1 );
	pyramidFaceTwo->push_back( 2 );
	pyramidFaceTwo->push_back( 4 );
	pyramidGeometry->addPrimitiveSet( pyramidFaceTwo );

	osg::DrawElementsUInt* pyramidFaceThree =
		new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
	pyramidFaceThree->push_back( 2 );
	pyramidFaceThree->push_back( 3 );
	pyramidFaceThree->push_back( 4 );
	pyramidGeometry->addPrimitiveSet( pyramidFaceThree );

	osg::DrawElementsUInt* pyramidFaceFour =
		new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
	pyramidFaceFour->push_back( 3 );
	pyramidFaceFour->push_back( 0 );
	pyramidFaceFour->push_back( 4 );
	pyramidGeometry->addPrimitiveSet( pyramidFaceFour );

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back( osg::Vec4( 1.0f, 0.0f, 0.0f, 1.0f ) ); //index 0 red
	colors->push_back( osg::Vec4( 0.0f, 1.0f, 0.0f, 1.0f ) ); //index 1 green
	colors->push_back( osg::Vec4( 0.0f, 0.0f, 1.0f, 1.0f ) ); //index 2 blue
	colors->push_back( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) ); //index 3 white
	colors->push_back( osg::Vec4( 1.0f, 0.0f, 0.0f, 1.0f ) ); //index 4 red

	pyramidGeometry->setColorArray( colors );
	pyramidGeometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

	return pyramidGeode;
}


double CoreGraph::computeOpacity( osg::Vec3 clusterPosition )
{
	double distance = ( cameraManipulator->getCameraPosition() - clusterPosition ).length();
	double opacity = 0.0;

	if ( distance > 1000 ) {
		opacity = 1.0;
	}
	else if ( distance > 900 ) {
		opacity = 0.9;
	}
	else if ( distance > 800 ) {
		opacity = 0.8;
	}
	else if ( distance > 700 ) {
		opacity = 0.7;
	}
	else if ( distance > 600 ) {
		opacity = 0.6;
	}
	else if ( distance > 500 ) {
		opacity = 0.5;
	}
	else if ( distance > 400 ) {
		opacity = 0.4;
	}
	else if ( distance > 300 ) {
		opacity = 0.3;
	}
	else if ( distance > 200 ) {
		opacity = 0.2;
	}
	else if ( distance > 100 ) {
		opacity = 0.1;
	}

//    double solidDistance = 1000.0;
//    if (distance > solidDistance) {
//        distance = solidDistance;
//    }

//    return distance / solidDistance;
	return opacity;
}
/*
osg::ref_ptr<osg::Group> CoreGraph::test2() {

//    if (testGroup != NULL) {
//        return testGroup;
//    }

//    qDebug() << "***** INIT test2 ";
    testGroup = new osg::Group;

    if (graph != NULL) {

//    Manager::GraphManager * manager = Manager::GraphManager::getInstance();
//    QMap<qlonglong, Data::Type*> * types = manager->getActiveGraph()->getTypes();
//    Data::Type * type = types->value(1);

    QMap<qlonglong, osg::ref_ptr<Data::Cluster> > clusters = Clustering::Clusterer::getInstance().getClusters();

    QMap<qlonglong, osg::ref_ptr<Data::Cluster> >::iterator i;
//int tempID = 0;
    for (i = clusters.begin(); i != clusters.end(); i++)
    {
        osg::ref_ptr<Data::Cluster> cluster = i.value();

    //    osg::ref_ptr<Data::Cluster> cluster = node->getCluster();

     //   osg::ref_ptr<Data::Cluster> cluster = new Data::Cluster(tempID++, "name", type, graph->getNodeScale(), graph, osg::Vec3f(0,0,0));

    //    qDebug() << "***** test2 cluster " << cluster->getId() << " count: " << cluster->getClusteredNodesCount();

    //    testGroup->addChild(getSphere(osg::Vec3( cluster->getId() * 10, cluster->getId() * 10, cluster->getId() * 10)));

        osg::Vec3f midPoint;
        float radius;

        // ak je na tomto clusteri zaregistrovany obmedzovac, vezmi jeho tvar
        if (cluster->getShapeGetter() != NULL) {
            midPoint = cluster->getShapeGetter()->getCenterNode()->getCurrentPosition(true);
            radius = (midPoint - cluster->getShapeGetter()->getSurfaceNode()->getCurrentPosition(true)).length();
        }
        // inak vypocitaj tvar podla zlucenych uzlov
        else {
            midPoint = getMidPoint(cluster->getALLClusteredNodes());
            radius = getRadius(cluster->getALLClusteredNodes(), midPoint);
        }

        int nodesCount = cluster->getClusteredNodesCount();

        osg::Vec4 color = cluster->getColor();
        if (clustersOpacityAutomatic) {
            color.w() = computeOpacity(midPoint);
        } else {
            color.w() = clustersOpacity;
        }

        // todo refactoring

//        if (nodesCount > clustersRangeMin && nodesCount <= clusters1Value) {
//            if (cameraInsideCube(midPoint, getRadius(cluster->getALLClusteredNodes(), midPoint))) {
//                color.w() = 1;
//            }
//            testGroup->addChild(getCube(cluster->getId(), midPoint, getRadius(cluster->getALLClusteredNodes(), midPoint), color));
//        } else if (nodesCount > clusters1Value && nodesCount <= clustersMiddleValue) {
            if (cameraInsideCube(midPoint, radius)) {
                color.w() = 1;
            }
            Cube * cube = new Cube(midPoint, radius, color);
            cube->getGeode()->setUserValue("id", QString::number(cluster->getId()).toStdString());

            cluster->setCube(cube);

            testGroup->addChild(cube->getAT());
//        } else if (nodesCount > clustersMiddleValue && nodesCount <= clusters2Value) {
//            if (cameraInsideSphere(midPoint, getRadius(cluster->getALLClusteredNodes(), midPoint))) {
//                color.w() = 1;
//            }
//            testGroup->addChild(dodecahedron(cluster->getId(), midPoint, getRadius(cluster->getALLClusteredNodes(), midPoint), color));
//        } else {
//            if (cameraInsideSphere(midPoint, getRadius(cluster->getALLClusteredNodes(), midPoint))) {
//                color.w() = 1;
//            }
//            testGroup->addChild(getSphere(cluster->getId(), midPoint, getRadius(cluster->getALLClusteredNodes(), midPoint), color));
//        }
    }

    }
    return testGroup;
}
*/
osg::ref_ptr<osg::AutoTransform> CoreGraph::dodecahedron( qlonglong id, osg::Vec3 position, double radius, osg::Vec4 color )
{
//    (±1, ±1, ±1)
//    (0, ±1/φ, ±φ)
//    (±1/φ, ±φ, 0)
//    (±φ, 0, ±1/φ)
//    where φ = (1 + √5) / 2 is the golden ratio (also written τ) ≈ 1.618.
//    The edge length is 2/φ = √5 – 1. The containing sphere has a radius of √3.

	osg::Vec3 midpoint = osg::Vec3( 0,0,0 );

	float x = midpoint.x();
	float y = midpoint.y();
	float z = midpoint.z();

	float i = 1;
	float fi = ( 1 + sqrtf( 5.0f ) ) / 2;
	float fi_rev = 1/fi;

	osg::Geode* dodecahedronGeode = new osg::Geode();
	osg::Geometry* dodecahedronGeometry = new osg::Geometry();
	dodecahedronGeode->addDrawable( dodecahedronGeometry );

	osg::Vec3Array* dodecahedronVertices = new osg::Vec3Array;
	// use http://en.wikipedia.org/wiki/File:Dodecahedron_vertices.png as reference
	// orange - cube
	dodecahedronVertices->push_back( osg::Vec3( x-i, y-i, z-i ) ); //front left bot  0
	dodecahedronVertices->push_back( osg::Vec3( x+i, y-i, z-i ) ); //front right bot 1
	dodecahedronVertices->push_back( osg::Vec3( x-i, y+i, z-i ) ); //front left top  2
	dodecahedronVertices->push_back( osg::Vec3( x+i, y+i, z-i ) ); //front right top 3

	dodecahedronVertices->push_back( osg::Vec3( x-i, y-i, z+i ) ); //back left bot   4
	dodecahedronVertices->push_back( osg::Vec3( x+i, y-i, z+i ) ); //back right bot  5
	dodecahedronVertices->push_back( osg::Vec3( x-i, y+i, z+i ) ); //back left top   6
	dodecahedronVertices->push_back( osg::Vec3( x+i, y+i, z+i ) ); //back right top  7

	// green - y-z plane
	dodecahedronVertices->push_back( osg::Vec3( x, y+fi_rev, z-fi ) ); //front top   8
	dodecahedronVertices->push_back( osg::Vec3( x, y-fi_rev, z-fi ) ); //front bot   9
	dodecahedronVertices->push_back( osg::Vec3( x, y+fi_rev, z+fi ) ); //back top    10
	dodecahedronVertices->push_back( osg::Vec3( x, y-fi_rev, z+fi ) ); //back bot    11

	// blue - x-y plane
	dodecahedronVertices->push_back( osg::Vec3( x-fi_rev, y-fi, z ) ); //bot left    12
	dodecahedronVertices->push_back( osg::Vec3( x+fi_rev, y-fi, z ) ); //bot right   13
	dodecahedronVertices->push_back( osg::Vec3( x-fi_rev, y+fi, z ) ); //top left    14
	dodecahedronVertices->push_back( osg::Vec3( x+fi_rev, y+fi, z ) ); //top right   15

	// pink - x-z plane
	dodecahedronVertices->push_back( osg::Vec3( x-fi, y, z-fi_rev ) ); //front left  16
	dodecahedronVertices->push_back( osg::Vec3( x+fi, y, z-fi_rev ) ); //front right 17
	dodecahedronVertices->push_back( osg::Vec3( x-fi, y, z+fi_rev ) ); //back left   18
	dodecahedronVertices->push_back( osg::Vec3( x+fi, y, z+fi_rev ) ); //back right  19

	dodecahedronGeometry->setVertexArray( dodecahedronVertices );

	osg::DrawElementsUInt* front =
		new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
	front->push_back( 8 );
	front->push_back( 3 );
	front->push_back( 17 );
	front->push_back( 1 );
	front->push_back( 9 );
	dodecahedronGeometry->addPrimitiveSet( front );

	osg::DrawElementsUInt* front2 =
		new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
	front2->push_back( 14 );
	front2->push_back( 15 );
	front2->push_back( 3 );
	front2->push_back( 8 );
	front2->push_back( 2 );
	dodecahedronGeometry->addPrimitiveSet( front2 );

	osg::DrawElementsUInt* front3 =
		new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
	front3->push_back( 16 );
	front3->push_back( 2 );
	front3->push_back( 8 );
	front3->push_back( 9 );
	front3->push_back( 0 );
	dodecahedronGeometry->addPrimitiveSet( front3 );

	osg::DrawElementsUInt* front4 =
		new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
	front4->push_back( 3 );
	front4->push_back( 15 );
	front4->push_back( 7 );
	front4->push_back( 19 );
	front4->push_back( 17 );
	dodecahedronGeometry->addPrimitiveSet( front4 );

	osg::DrawElementsUInt* front5 =
		new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
	front5->push_back( 17 );
	front5->push_back( 19 );
	front5->push_back( 5 );
	front5->push_back( 13 );
	front5->push_back( 1 );
	dodecahedronGeometry->addPrimitiveSet( front5 );

	osg::DrawElementsUInt* front6 =
		new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
	front6->push_back( 12 );
	front6->push_back( 13 );
	front6->push_back( 1 );
	front6->push_back( 9 );
	front6->push_back( 0 );
	dodecahedronGeometry->addPrimitiveSet( front6 );

	osg::DrawElementsUInt* back1 =
		new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
	back1->push_back( 6 );
	back1->push_back( 10 );
	back1->push_back( 11 );
	back1->push_back( 4 );
	back1->push_back( 18 );
	dodecahedronGeometry->addPrimitiveSet( back1 );

	osg::DrawElementsUInt* back2 =
		new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
	back2->push_back( 6 );
	back2->push_back( 10 );
	back2->push_back( 7 );
	back2->push_back( 15 );
	back2->push_back( 14 );
	dodecahedronGeometry->addPrimitiveSet( back2 );

	osg::DrawElementsUInt* back3 =
		new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
	back3->push_back( 11 );
	back3->push_back( 10 );
	back3->push_back( 7 );
	back3->push_back( 19 );
	back3->push_back( 5 );
	dodecahedronGeometry->addPrimitiveSet( back3 );

	osg::DrawElementsUInt* back4 =
		new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
	back4->push_back( 12 );
	back4->push_back( 4 );
	back4->push_back( 11 );
	back4->push_back( 5 );
	back4->push_back( 13 );
	dodecahedronGeometry->addPrimitiveSet( back4 );

	osg::DrawElementsUInt* back5 =
		new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
	back5->push_back( 16 );
	back5->push_back( 18 );
	back5->push_back( 4 );
	back5->push_back( 12 );
	back5->push_back( 0 );
	dodecahedronGeometry->addPrimitiveSet( back5 );

	osg::DrawElementsUInt* back6 =
		new osg::DrawElementsUInt( osg::PrimitiveSet::POLYGON, 0 );
	back6->push_back( 6 );
	back6->push_back( 18 );
	back6->push_back( 16 );
	back6->push_back( 2 );
	back6->push_back( 14 );
	dodecahedronGeometry->addPrimitiveSet( back6 );

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back( color );

	dodecahedronGeometry->setColorArray( colors );
	dodecahedronGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
//    dodecahedronGeometry->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE_SET);



	osg::ref_ptr<osg::StateSet> ss = dodecahedronGeometry->getOrCreateStateSet();
	/*
	// only wireframe (outline / contour)
	    osg::ref_ptr<osg::PolygonMode> pm = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	    ss->setAttributeAndModes(pm.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	// line width
	    osg::LineWidth* linewidth = new osg::LineWidth();
	    linewidth->setWidth(20.0f);
	    ss->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
	*/

// transparent

	ss->setMode( GL_BLEND, osg::StateAttribute::ON );
	ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

	osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform;
	at->setPosition( position * 1 );
	at->setScale( radius/15 * sqrt( 75 + 30*sqrt( 5.0 ) ) );
//    at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

	dodecahedronGeode->setUserValue( "id", QString::number( id ).toStdString() );

	at->addChild( dodecahedronGeode );

	return at;
}



void CoreGraph::setCameraManipulator( Vwr::CameraManipulator* cameraManipulator )
{
	this->cameraManipulator = cameraManipulator;
}

void CoreGraph::setClustersOpacityAutomatic( bool automatic )
{
	this->clustersOpacityAutomatic = automatic;
}

void CoreGraph::setClustersOpacitySelected( bool selected )
{
	this->clustersOpacitySelected = selected;
}

void CoreGraph::setClustersOpacity( double opacity )
{
	this->clustersOpacity = opacity;
}

void CoreGraph::setClustersShapeBoundary( int value )
{
	this->clustersShapeBoundary = value;
}

Vwr::CoreGraph::CoreGraph( Data::Graph* graph, osg::ref_ptr<osg::Camera> camera )
{
	this->graph = graph;
	this->camera = ( ( camera == 0 ) ? ( static_cast<osg::ref_ptr<osg::Camera> >( new osg::Camera ) ) : ( camera ) );

	this->in_nodes = NULL;
	this->in_edges = NULL;
	this->qmetaNodes = NULL;
	this->qmetaEdges = NULL;
	this->nodesFreezed = false;
	this->clustersOpacityAutomatic = false;

	this->edgesGroup = NULL;
	this->qmetaEdgesGroup = NULL;

	appConf = Util::ApplicationConfig::get();

	root = new osg::Group();
	hud = new Hud();
	graphRotTransf = new osg::MatrixTransform();
	graphGroup = new osg::Group();
	rotationMatrix = rotationMatrix.identity();



	//jurik
	//lighting
	osg::Light* pLight = new osg::Light;
	pLight->setLightNum( 1 );
	pLight->setDiffuse( osg::Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
	pLight->setPosition( osg::Vec4( 0,0,1,0 ) );		// w = 0 directional light
	// w = 1 point light (position)
	// light source
	osg::LightSource* pLightSource = new osg::LightSource;
	pLightSource->setLight( pLight );
	root->addChild( pLightSource );

	//shadow scene
	//http://trac.openscenegraph.org/projects/osg//wiki/Support/ProgrammingGuide/osgShadow
	shadowedScene = new osgShadow::ShadowedScene;
	shadowedScene->setReceivesShadowTraversalMask( 0x1 );
	shadowedScene->setCastsShadowTraversalMask( 0x2 );
	root->addChild( shadowedScene );


	//node and transform for axes
	axesGeode = new osg::Geode();
	axesTransform = new osg::MatrixTransform();

	root->addChild( axesTransform );
	axesTransform->addChild( axesGeode );
	drawAxes();

	//node and transform for bas
	baseGeode = new osg::Geode();
	baseTransform = new osg::MatrixTransform();

	graphRotTransf->addChild( graphGroup );
	shadowedScene->addChild( graphRotTransf );
	root->addChild( graphRotTransf );

	createBase();
	if ( !arucoRunning ) {
		updateBase( 2000 );
	}

	//******

	//leap ar hands group
	handsGroup = new osg::Group();
	root->addChild( handsGroup );
	//

	// backgroung this must be last Node in root !!!  ( because of ortho2d background)
	// Gloger: disabled skybox- using solid background (see setClearColor in ViewerQT)
	leapCameraStream = nullptr;
	root->addChild( createBackground() );
	backgroundPosition = 2;

	reload( graph );
}

int CoreGraph::updateBackground( int bgVal, Data::Graph* currentGraph )
{
	osg::Group* root = this->getScene();
	if ( root->removeChild( root->getNumChildren()-1 ) == true ) {
		if ( bgVal == 0 ) { // default skybox
			SkyBox* skyBox = new SkyBox;
			root->addChild( skyBox->createSkyBox( 0 ) );
		}
		else if ( bgVal == 1 ) { // noise skybox
			root->addChild( createSkyNoiseBox() );
		}
#ifdef OPENCV_FOUND
		else if ( bgVal == 2 ) {
			root->addChild( createTextureBackground() );
		}
        else if ( bgVal == 3 ) {
			root->addChild( createOrtho2dBackground() );
		}
#endif
        else if ( bgVal == 4 ) {
			root->addChild( createLeapBackground() ); // leap
		}
		else if ( bgVal == -1 ) {
			SkyBox* skyBox = new SkyBox;
			root->addChild( skyBox->createSkyBox( -1 ) ); // black skybox
		}
		else if ( bgVal == -2 ) {
			SkyBox* skyBox = new SkyBox;
			root->addChild( skyBox->createSkyBox( -2 ) ); // white skybox
		}

		reload( currentGraph );
		return 0;
	}

	return 1;
}


int CoreGraph::updateBackgroundStream( unsigned char* buffer )
{
//	LOG( INFO ) << "CoreGraph::updateBackgroundStream - updating background";
	if ( leapCameraStream != nullptr ) {
		leapCameraStream->dirty();
        leapCameraStream->updateBackgroundImage( buffer);
	}
	return 1;

    // TODO return success/fail
}


void CoreGraph::reload( Data::Graph* graph )
{
	cleanUp();

	unsigned int currentPos = 0;

	while ( graphGroup->getNumChildren() > 0 ) {
		graphGroup->removeChildren( 0, 1 );
	}

	this->graph = graph;

	if ( graph != NULL ) {
		this->in_nodes = graph->getNodes();
		this->in_edges = graph->getEdges();
		this->qmetaNodes = graph->getMetaNodes();
		this->qmetaEdges = graph->getMetaEdges();
	}
	else {
		this->in_nodes = new QMap<qlonglong, osg::ref_ptr<Data::Node> >;
		this->in_edges = new QMap<qlonglong, osg::ref_ptr<Data::Edge> >;
		this->qmetaNodes = new QMap<qlonglong, osg::ref_ptr<Data::Node> >;
		this->qmetaEdges = new QMap<qlonglong, osg::ref_ptr<Data::Edge> >;
	}

	QMapIterator<qlonglong, osg::ref_ptr<Data::Edge> > i( *in_edges );

	while ( i.hasNext() ) {
		i.next();
		i.value()->setCamera( camera );
	}

	this->nodesGroup = new Vwr::NodeGroup( in_nodes );
	graphGroup->addChild( nodesGroup->getGroup() );
	nodesPosition = currentPos++;

	this->edgesGroup = new Vwr::EdgeGroup( in_edges );
	//this->edgesGroup = new Vwr::EdgeGroup(in_edges, 10);
	graphGroup->addChild( edgesGroup->getGroup() );
	edgesPosition = currentPos++;

	this->qmetaNodesGroup = new Vwr::NodeGroup( qmetaNodes );
	graphGroup->addChild( qmetaNodesGroup->getGroup() );
	qmetaNodesPosition = currentPos++;

	this->qmetaEdgesGroup = new Vwr::EdgeGroup( qmetaEdges );
	//this->qmetaEdgesGroup = new Vwr::EdgeGroup(qmetaEdges, 10);
	graphGroup->addChild( qmetaEdgesGroup->getGroup() );
	qmetaEdgesPosition = currentPos++;

	graphGroup->addChild( initEdgeLabels() );
	labelsPosition = currentPos++;

	//zaciatok

	/*
	    osg::ref_ptr<osgManipulator::TranslateAxisDragger> dragger = new osgManipulator::TranslateAxisDragger();
	    dragger->setupDefaultGeometry();
	    graphGroup->addChild(dragger.get());

	    osg::ref_ptr<osg::MatrixTransform> geom1 = new osg::MatrixTransform(osg::Matrixd::scale(osg::Vec3f(1,1,1)));
	    geom1->addChild(graphGroup);

	    qDebug() << dragger->getMatrix().getTrans().x() << " " << dragger->getMatrix().getTrans().y();

	    float scale = geom1->getBound().radius() * 1.0f;
	    osg::Matrix mat = osg::Matrix::scale(scale, scale, scale) * osg::Matrix::translate(geom1->getBound().center());
	    dragger->setMatrix(mat);

	    dragger->setHandleEvents(true);
	    // konec
	*/
	this->restrictionVisualizationsGroup = QSharedPointer<Vwr::RestrictionVisualizationsGroup> ( new Vwr::RestrictionVisualizationsGroup );
	graphGroup->addChild( restrictionVisualizationsGroup->getGroup() );
	restrictionVisualizationsPosition = currentPos++;

	this->restrictionManipulatorsGroup = QSharedPointer<Vwr::RestrictionManipulatorsGroup> ( new Vwr::RestrictionManipulatorsGroup );
	graphGroup->addChild( restrictionManipulatorsGroup->getGroup() );
	restrictionManipulatorsPosition = currentPos++;

	this->browsersGroup = new Vwr::BrowserGroup();
	graphGroup->addChild( browsersGroup->getGroup() );
	browsersPosition = currentPos++;

	if ( this->graph != NULL ) {
		graph->getRestrictionsManager().setObservers( restrictionVisualizationsGroup, restrictionManipulatorsGroup );
	}

	clustersGroup = new osg::Group;
	graphGroup->addChild( clustersGroup );
	currentPos++;

	customNodesPosition = currentPos;

	osgUtil::Optimizer opt;
	opt.optimize( edgesGroup->getGroup(), osgUtil::Optimizer::CHECK_GEOMETRY );

	// Set browsers to be always on top
	this->browsersGroup->getGroup()->getOrCreateStateSet()->setMode( GL_DEPTH_TEST, osg::StateAttribute::OFF );
	this->browsersGroup->getGroup()->getOrCreateStateSet()->setRenderBinDetails( 100,"RenderBin" );
}

void CoreGraph::cleanUp()
{
	if ( this->graph == NULL ) {
		delete in_nodes;
		delete in_edges;
		delete qmetaNodes;
		delete qmetaEdges;
	}
	else {
		graph->getRestrictionsManager().resetObserver();
	}

	delete qmetaEdgesGroup;
	delete edgesGroup;
}
osg::ref_ptr<osg::Node> CoreGraph::createLeapBackground()
{

	osg::ref_ptr<osg::Geode> GeodeHUD = new osg::Geode();

	osg::ref_ptr<osg::Projection> ProjectionMatrixHUD = new osg::Projection;
	osg::ref_ptr<osg::MatrixTransform> ModelViewMatrixHUD = new osg::MatrixTransform;

	ModelViewMatrixHUD->setMatrix( osg::Matrix::identity() );
	ModelViewMatrixHUD->setReferenceFrame( osg::Transform::ABSOLUTE_RF );

	ProjectionMatrixHUD->setMatrix( osg::Matrix::ortho2D( 0,640,0,240 ) );
	ProjectionMatrixHUD->addChild( ModelViewMatrixHUD );
	ModelViewMatrixHUD->addChild( GeodeHUD );


	osg::Vec3Array* coordsHUD = new osg::Vec3Array;
	coordsHUD->push_back( osg::Vec3( 0,    0, -1 ) );
	coordsHUD->push_back( osg::Vec3( 640,    0, -1 ) );
	coordsHUD->push_back( osg::Vec3( 640,  240, -1 ) );
	coordsHUD->push_back( osg::Vec3( 0,  240, -1 ) );

	osg::Vec2Array* texCoords = new osg::Vec2Array( 4 );
	( *texCoords )[0].set( 0.0f, 1.0f );
	( *texCoords )[1].set( 1.0f, 1.0f );
	( *texCoords )[2].set( 1.0f, 0.0f );
	( *texCoords )[3].set( 0.0f, 0.0f );

	osg::Vec3Array* normalsHUD = new osg::Vec3Array;
	normalsHUD->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );


	osg::ref_ptr<osg::Geometry> GeomHUD = new osg::Geometry();
	GeomHUD->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POLYGON,0,4 ) );
	GeomHUD->setVertexArray( coordsHUD );
	GeomHUD->setNormalArray( normalsHUD );
	GeomHUD->setNormalBinding( osg::Geometry::BIND_OVERALL );
	GeomHUD->setTexCoordArray( 0,texCoords );


	leapCameraStream = new Leap::LeapCameraStream();
	leapCameraStream->setDataVariance( osg::Object::DYNAMIC );

	osg::ref_ptr<osg::Texture2D> textureHUD = new osg::Texture2D( leapCameraStream );
	textureHUD->setDataVariance( osg::Object::DYNAMIC );
	textureHUD->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
	textureHUD->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
	textureHUD->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
	textureHUD->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );
	textureHUD->setResizeNonPowerOfTwoHint( false );



	osg::ref_ptr<osg::StateSet> statesetHUD = new osg::StateSet();
	statesetHUD->setTextureAttributeAndModes( 0, textureHUD, osg::StateAttribute::ON );
	statesetHUD->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	statesetHUD->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
	statesetHUD->setMode( GL_BLEND,osg::StateAttribute::OFF );


	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction( osg::Depth::ALWAYS );
	depth->setRange( 1, 1 );
	statesetHUD->setAttributeAndModes( depth, osg::StateAttribute::ON );
	statesetHUD->setRenderBinDetails( -1, "RenderBin" );


	GeodeHUD->setStateSet( statesetHUD );
	GeodeHUD->addDrawable( GeomHUD );

	osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	clearNode->setRequiresClear( false );
	clearNode->addChild( ProjectionMatrixHUD );

	return clearNode;
}



#ifdef OPENCV_FOUND
osg::ref_ptr<osg::Node> CoreGraph::createTextureBackground()
{
	// rectangle
	// coordinates
	osg::Vec3Array* coords = new osg::Vec3Array( 4 );
	( *coords )[0].set( -1.0f, 1.0f, -1.0f );
	( *coords )[1].set( 1.0f, 1.0f, -1.0f );
	( *coords )[2].set( 1.0f, 1.0f,  1.0f );
	( *coords )[3].set( -1.0f, 1.0f,  1.0f );

	// normals
	osg::Vec3Array* normals = new osg::Vec3Array( 1 );
	( *normals )[0].set( 0.0f, 1.0f, 0.0f );

	// texture coordinates
	osg::Vec2Array* texCoords = new osg::Vec2Array( 4 );
	( *texCoords )[0].set( 0.0f, 1.0f );
	( *texCoords )[1].set( 1.0f, 1.0f );
	( *texCoords )[2].set( 1.0f, 0.0f );
	( *texCoords )[3].set( 0.0f, 0.0f );

	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
	geom->setDataVariance( osg::Object::DYNAMIC );
	geom->addPrimitiveSet( new osg::DrawArrays( GL_QUADS, 0, 4 ) );
	geom->setVertexArray( coords );
	geom->setNormalArray( normals );
	geom->setNormalBinding( osg::Geometry::BIND_OVERALL );
	geom->setTexCoordArray( 0,texCoords );


	// texture
	mCameraStream = new OpenCV::CameraStream( geom );
	mCameraStream->setDataVariance( osg::Object::DYNAMIC );

	osg::ref_ptr<osg::Texture2D> skymap = new osg::Texture2D( mCameraStream );
	skymap->setDataVariance( osg::Object::DYNAMIC );
	skymap->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
	skymap->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
	skymap->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
	skymap->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );
	skymap->setResizeNonPowerOfTwoHint( false );



	// stateset
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	stateset->setTextureAttributeAndModes( 0, skymap, osg::StateAttribute::ON );
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
	stateset->setRenderBinDetails( -1,"RenderBin" );

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction( osg::Depth::ALWAYS );
	depth->setRange( 1, 1 );
	stateset->setAttributeAndModes( depth, osg::StateAttribute::ON );



	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->setCullingActive( false );
	geode->setStateSet( stateset );
	geode->addDrawable( geom );

	osg::ref_ptr<osg::Transform> transform = new SkyTransform;

	transform->setCullingActive( false );
	transform->addChild( geode );

	osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	clearNode->setRequiresClear( false );
	clearNode->addChild( transform );

	return clearNode;

}

osg::ref_ptr<osg::Node> CoreGraph::createOrtho2dBackground()
{

	osg::ref_ptr<osg::Geode> GeodeHUD = new osg::Geode();

	osg::ref_ptr<osg::Projection> ProjectionMatrixHUD = new osg::Projection;
	osg::ref_ptr<osg::MatrixTransform> ModelViewMatrixHUD = new osg::MatrixTransform;

	ModelViewMatrixHUD->setMatrix( osg::Matrix::identity() );
	ModelViewMatrixHUD->setReferenceFrame( osg::Transform::ABSOLUTE_RF );

	ProjectionMatrixHUD->setMatrix( osg::Matrix::ortho2D( 0,640,0,480 ) );
	ProjectionMatrixHUD->addChild( ModelViewMatrixHUD );
	ModelViewMatrixHUD->addChild( GeodeHUD );


	osg::Vec3Array* coordsHUD = new osg::Vec3Array;
	coordsHUD->push_back( osg::Vec3( 0,    0, -1 ) );
	coordsHUD->push_back( osg::Vec3( 640,    0, -1 ) );
	coordsHUD->push_back( osg::Vec3( 640,  480, -1 ) );
	coordsHUD->push_back( osg::Vec3( 0,  480, -1 ) );

	osg::Vec2Array* texCoords = new osg::Vec2Array( 4 );
	( *texCoords )[0].set( 0.0f, 1.0f );
	( *texCoords )[1].set( 1.0f, 1.0f );
	( *texCoords )[2].set( 1.0f, 0.0f );
	( *texCoords )[3].set( 0.0f, 0.0f );

	osg::Vec3Array* normalsHUD = new osg::Vec3Array;
	normalsHUD->push_back( osg::Vec3( 0.0f, 0.0f, 1.0f ) );


	osg::ref_ptr<osg::Geometry> GeomHUD = new osg::Geometry();
	GeomHUD->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::POLYGON,0,4 ) );
	GeomHUD->setVertexArray( coordsHUD );
	GeomHUD->setNormalArray( normalsHUD );
	GeomHUD->setNormalBinding( osg::Geometry::BIND_OVERALL );
	GeomHUD->setTexCoordArray( 0,texCoords );


	mCameraStream = new OpenCV::CameraStream();
	mCameraStream->setDataVariance( osg::Object::DYNAMIC );

	osg::ref_ptr<osg::Texture2D> textureHUD = new osg::Texture2D( mCameraStream );
	textureHUD->setDataVariance( osg::Object::DYNAMIC );
	textureHUD->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
	textureHUD->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
	textureHUD->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
	textureHUD->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );
	textureHUD->setResizeNonPowerOfTwoHint( false );



	osg::ref_ptr<osg::StateSet> statesetHUD = new osg::StateSet();
	statesetHUD->setTextureAttributeAndModes( 0, textureHUD, osg::StateAttribute::ON );
	statesetHUD->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	statesetHUD->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
	statesetHUD->setMode( GL_BLEND,osg::StateAttribute::OFF );


	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction( osg::Depth::ALWAYS );
	depth->setRange( 1, 1 );
	statesetHUD->setAttributeAndModes( depth, osg::StateAttribute::ON );
	statesetHUD->setRenderBinDetails( -1, "RenderBin" );


	GeodeHUD->setStateSet( statesetHUD );
	GeodeHUD->addDrawable( GeomHUD );

	osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	clearNode->setRequiresClear( false );
	clearNode->addChild( ProjectionMatrixHUD );

	return clearNode;

}
#endif

osg::ref_ptr<osg::Node> CoreGraph::createSkyNoiseBox()
{
	unsigned char red = static_cast<unsigned char>( appConf->getValue( "Viewer.Display.BackGround.R" ).toInt() );
	unsigned char green = static_cast<unsigned char>( appConf->getValue( "Viewer.Display.BackGround.G" ).toInt() );
	unsigned char blue = static_cast<unsigned char>( appConf->getValue( "Viewer.Display.BackGround.B" ).toInt() );
	osg::ref_ptr<osg::Texture2D> skymap =
		PerlinNoiseTextureGenerator::getCoudTexture( 2048, 1024,
				red,
				green,
				blue,
				255 );

	skymap->setDataVariance( osg::Object::DYNAMIC );
	skymap->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
	skymap->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
	skymap->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
	skymap->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );

	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();
	stateset->setTextureAttributeAndModes( 0, skymap, osg::StateAttribute::ON );
	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );
	stateset->setRenderBinDetails( -1,"RenderBin" );

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setFunction( osg::Depth::ALWAYS );
	depth->setRange( 1, 1 );
	stateset->setAttributeAndModes( depth, osg::StateAttribute::ON );

	osg::ref_ptr<osg::Drawable> drawable = new osg::ShapeDrawable( new osg::Sphere( osg::Vec3( 0.0f,0.0f,0.0f ), 1 ) );
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	geode->setCullingActive( false );
	geode->setStateSet( stateset );
	geode->addDrawable( drawable );

	osg::ref_ptr<osg::Transform> transform = new SkyTransform;
	transform->setCullingActive( false );
	transform->addChild( geode );

	osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
	clearNode->setRequiresClear( false );
	clearNode->addChild( transform );

	return clearNode;
}

osg::ref_ptr<osg::Node> CoreGraph::createBackground()
{

	// skybox
	int background = appConf->getValue( "Viewer.SkyBox.Noise" ).toInt();


	if ( background == 0 ) {
		SkyBox* skyBox = new SkyBox;
		return skyBox->createSkyBox( 0 );
	}

	// skynoise
	if ( background == 1 ) {
		return createSkyNoiseBox();
	}

#ifdef OPENCV_FOUND
	// video backgroung as 3d rectangle
	if ( background == 2 ) {
		return createTextureBackground();
	}

    if ( background == 3 ) {
        return createOrtho2dBackground();
    }

    if ( background == 4 ) {
		return createLeapBackground();
	}

#endif

	return NULL;
}


osg::ref_ptr<osg::Group> CoreGraph::initEdgeLabels()
{
	osg::ref_ptr<osg::Group> labels = new osg::Group;

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator i = in_edges->begin();

	while ( i != in_edges->end() ) {
		labels->addChild( i.value()->createLabel( i.value()->Data::AbsEdge::getName() ) );
		++i;
	}

	labels->setNodeMask( 0 );

	return labels;
}

osg::ref_ptr<osg::Group> CoreGraph::initCustomNodes()
{
	osg::ref_ptr<osg::Group> customNodes = new osg::Group;

	QLinkedList<osg::ref_ptr<osg::Node> >::const_iterator i = customNodeList.constBegin();
	while ( i != customNodeList.constEnd() ) {
		customNodes->addChild( *i );
		++i;
	}
	return customNodes;
}

void CoreGraph::createClusterGroup( QMap<qlonglong, osg::ref_ptr<Data::Cluster> > clusters )
{
	clustersGroup->removeChildren( 0,clustersGroup->getNumChildren() );
	QMap<qlonglong, osg::ref_ptr<Data::Cluster> >::iterator i;
	for ( i = clusters.begin(); i != clusters.end(); ++i ) {
		osg::ref_ptr<Data::Cluster> cluster = i.value();

		osg::Vec3f midPoint = getMidPoint( cluster->getALLClusteredNodes() );
		double radius = getRadius( cluster->getALLClusteredNodes(), midPoint );

		Clustering::Cube* cube = new Clustering::Cube( midPoint, radius, osg::Vec4d( 1,1,1,0.5 ) );
		cube->getGeode()->setUserValue( "id", QString::number( cluster->getId() ).toStdString() );
		cluster->setCube( cube );

		Clustering::Sphere* sphere = new Clustering::Sphere( midPoint, radius, osg::Vec4d( 1,1,1,0.5 ) );
		sphere->getGeode()->setUserValue( "id", QString::number( cluster->getId() ).toStdString() );
		cluster->setSphere( sphere );

		clustersGroup->addChild( cluster->getSphere()->getAT() );
		clustersGroup->addChild( cluster->getCube()->getAT() );
	}
}

void CoreGraph::updateClustersCoords()
{
	QMap<qlonglong, osg::ref_ptr<Data::Cluster> > clusters = Clustering::Clusterer::getInstance().getClusters();
	QMap<qlonglong, osg::ref_ptr<Data::Cluster> >::iterator i;
	for ( i = clusters.begin(); i != clusters.end(); ++i ) {
		osg::ref_ptr<Data::Cluster> cluster = i.value();

		osg::Vec3f midPoint;
		float radius;		// BUG: nie je inicializovany, nizsie sa moze nenastaveny pouzit
		osg::Vec3f scale;

		osg::Vec3f lowerPoint;
		osg::Vec3f upperPoint;

		// ak je na tomto clusteri zaregistrovany obmedzovac, vezmi jeho tvar
		if ( cluster->getShapeGetter() != NULL ) {
			Layout::ShapeGetter_Cube* shapeGetter = cluster->getShapeGetter();
			midPoint = shapeGetter->getCenterNode()->getCurrentPosition();
			float distanceX = ( shapeGetter->getSurfaceNodeX()->getCurrentPosition() - midPoint ).length();
			float distanceY = ( shapeGetter->getSurfaceNodeY()->getCurrentPosition() - midPoint ).length();
			float distanceZ = ( shapeGetter->getSurfaceNodeZ()->getCurrentPosition() - midPoint ).length();
			scale = osg::Vec3f( distanceX, distanceY, distanceZ );

			lowerPoint = osg::Vec3f( midPoint.x() - distanceX, midPoint.y() - distanceY, midPoint.z() - distanceZ );
			upperPoint = osg::Vec3f( midPoint.x() + distanceX, midPoint.y() + distanceY, midPoint.z() + distanceZ );
		}
		// inak vypocitaj tvar podla zlucenych uzlov
		else {
			midPoint = getMidPoint( cluster->getALLClusteredNodes() );
			radius = static_cast<float>( getRadius( cluster->getALLClusteredNodes(), midPoint ) );
			scale = osg::Vec3f( radius,radius,radius );

			lowerPoint = osg::Vec3f( midPoint.x() - radius, midPoint.y() - radius, midPoint.z() - radius );
			upperPoint = osg::Vec3f( midPoint.x() + radius, midPoint.y() + radius, midPoint.z() + radius );
		}

		osg::Vec4 color = cluster->getColor();
		if ( clustersOpacityAutomatic ) {
			color.w() = static_cast<float>( computeOpacity( midPoint ) );
		}
		else if ( ( clustersOpacitySelected && cluster->isSelected() ) || !clustersOpacitySelected ) {
			color.w() = static_cast<float>( clustersOpacity );
		}

		if ( cluster->getALLClusteredNodes().count() <= clustersShapeBoundary ) {
			if ( cameraInsideCube( lowerPoint, upperPoint ) ) {
				//color.w() = 1;
				color = osg::Vec4d( 1,1,1,1 );
			}
			cluster->getCube()->transform( midPoint, scale, color );
			color.w() = 0;
			cluster->getSphere()->transform( midPoint, scale, color );
		}
		else {
			if ( cameraInsideSphere( midPoint, radius ) ) {	// BUG: radius moze byt nenastaveny!
				//color.w() = 1;
				color = osg::Vec4d( 1,1,1,1 );
			}
			cluster->getSphere()->transform( midPoint, scale, color );
			color.w() = 0;
			cluster->getCube()->transform( midPoint, scale, color );
		}
	}
}

/*!
 *
 *
 * Zaktualizuje pozicie uzlov a hran na obrazovke.
 *
 */
void CoreGraph::update()
{
	graphGroup->removeChildren( customNodesPosition, 1u );

	synchronize();

	if ( !this->nodesFreezed ) {
		float interpolationSpeed = appConf->getValue( "Viewer.Display.InterpolationSpeed" ).toFloat();
		nodesGroup->updateNodeCoordinates( interpolationSpeed );
		qmetaNodesGroup->updateNodeCoordinates( interpolationSpeed );
	}
	else if ( !this->interpolationDenied ) {
		nodesGroup->updateNodeCoordinates( 1 );
		qmetaNodesGroup->updateNodeCoordinates( 1 );
	}

	if ( !this->nodesFreezed || !this->interpolationDenied ) {
		edgesGroup->updateEdgeCoords();
		qmetaEdgesGroup->updateEdgeCoords();
		browsersGroup->updateBrowsers();

		updateClustersCoords();
	}

	graphGroup->addChild( initCustomNodes() );

	//posli layout ostatnym klientom (ak nejaki su)
	Network::Server* server = Network::Server::getInstance();
	int time = QTime::currentTime().msec();
	if ( abs( time - prevTime ) > 100 ) {
		server -> sendLayout();
		prevTime = time;
	}
}

void CoreGraph::synchronize()
{
	nodesGroup->synchronizeNodes();
	edgesGroup->synchronizeEdges();
	qmetaNodesGroup->synchronizeNodes();
	qmetaEdgesGroup->synchronizeEdges();
}

void CoreGraph::setEdgeLabelsVisible( bool visible )
{
	graphGroup->getChild( labelsPosition )->setNodeMask( visible );
}

void CoreGraph::setNodeLabelsVisible( bool visible )
{
	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = in_nodes->constBegin();

	while ( i != in_nodes->constEnd() ) {
		( *i )->showLabel( visible, labelsForResidenceShowed );
		++i;
	}
}

void CoreGraph::showLabelsForResidence( bool state )
{
	this->labelsForResidenceShowed = state;
}

bool CoreGraph::isHudDisplayed() const
{
	return root->containsNode( hud );
}

void CoreGraph::showHud( bool state )
{
	if ( state && !isHudDisplayed() ) {
		root->addChild( hud );
	}
	else if ( !state && isHudDisplayed() ) {
		root->removeChild( hud );
	}
}

Hud* CoreGraph::getHud()
{
	return hud;
}

void CoreGraph::reloadConfig()
{
	root->setChild( backgroundPosition, createBackground() );
	root->setChild( 2, shadowedScene );


	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = in_nodes->constBegin();

	while ( i != in_nodes->constEnd() ) {
		( *i )->reloadConfig();
		++i;
	}

	graphGroup->setChild( labelsPosition, initEdgeLabels() );
	Vwr::TextureWrapper::reloadTextures();
}

CoreGraph::~CoreGraph( void )
{
	cleanUp();
}

void CoreGraph::onResized( int width, int height )
{
	hud->setWindowSize( QSize( width, height ) );
}

void CoreGraph::setNodesFreezed( bool val )
{
	this->nodesFreezed = val;
	nodesGroup->freezeNodePositions();
	qmetaNodesGroup->freezeNodePositions();
}


void CoreGraph::updateGraphRotByAruco( const osg::Quat quat )
{
	mRotAruco = quat;
	computeGraphRotTransf();
}


void CoreGraph::updateGraphPosAndRotByAruco( const osg::Quat quat, osg::Vec3d pos )
{
	mRotAruco = quat;
	//computeGraphRotTransf();
	addTranslateToGraphRotTransf( pos );
}

void CoreGraph::translateGraph( osg::Vec3d pos )
{
	osg::Matrixd matrix = graphRotTransf->getMatrix();

	qDebug() << "pos x,y,z " << pos.x() << "," << pos.y() << "," << pos.z();

	matrix.preMultTranslate( pos );

	graphRotTransf->setMatrix( matrix );
}

void CoreGraph::updateGraphRotByMouse( const osg::Quat quat )
{
	mRotMouse = mRotMouse * quat;
	computeGraphRotTransf();
}

void CoreGraph::updateGraphRotByFaceDet( const osg::Quat quat )
{
	mRotFaceDet = quat;
	computeGraphRotTransf();
}

void CoreGraph::computeGraphRotTransf()
{
	osg::Matrixd graphTransfMat( mRotMouse * mRotAruco * mRotFaceDet );
	graphRotTransf->setMatrix( graphTransfMat );
}

void CoreGraph::setNodeVisual( unsigned int index )
{
	QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator iNode = in_nodes->begin();

	while ( iNode != in_nodes->end() ) {
		iNode.value()->setVisual( index );
		++iNode;
	}

	QMap<qlonglong, osg::ref_ptr<Data::Node> >::iterator iMetaNode = qmetaNodes->begin();

	while ( iMetaNode != qmetaNodes->end() ) {
		iMetaNode.value()->setVisual( index );
		++iMetaNode;
	}

	graph->setNodeVisual( index );
}

void CoreGraph::setEdgeVisual( int index )
{
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator iEdge = in_edges->begin();

	while ( iEdge != in_edges->end() ) {
		iEdge.value()->setVisual( index );
		++iEdge;
	}

	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator iMetaEdge = qmetaEdges->begin();

	while ( iMetaEdge != qmetaEdges->end() ) {
		iMetaEdge.value()->setVisual( index );
		++iMetaEdge;
	}

	graph->setEdgeVisual( index );
}

void CoreGraph::setEdgeVisualForType( int index, QString edgeTypeName )
{
	QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator iEdge = in_edges->begin();

	while ( iEdge != in_edges->end() ) {
		if ( !QString::compare( iEdge.value()->getType()->getName(), edgeTypeName, Qt::CaseInsensitive ) ) {
			iEdge.value()->setVisual( index );
		}
		++iEdge;
	}

	/*QMap<qlonglong, osg::ref_ptr<Data::Edge> >::iterator iMetaEdge = qmetaEdges->begin();

	while ( iMetaEdge != qmetaEdges->end() ) {
		if(iEdge.value()->getType()->getName() == edgeTypeName) {
			iMetaEdge.value()->setVisual( index );
			++iMetaEdge;
		}
	}*/

	graph->setEdgeVisual( index );
}

#ifdef OPENCV_FOUND
OpenCV::CameraStream* CoreGraph::getCameraStream() const
{
	return mCameraStream;
}
#endif
Leap::LeapCameraStream* CoreGraph::getLeapCameraStream() const
{
	return leapCameraStream;
}

bool CoreGraph::cameraInsideCube( osg::Vec3d lowerPoint, osg::Vec3d upperPoint )
{
	return ( new osg::BoundingBox(
				 static_cast<float>( lowerPoint.x() ), static_cast<float>( lowerPoint.y() ), static_cast<float>( lowerPoint.z() ),
				 static_cast<float>( upperPoint.x() ), static_cast<float>( upperPoint.y() ), static_cast<float>( upperPoint.z() ) )
		   )->contains( cameraManipulator->getCameraPosition() );
}

bool CoreGraph::cameraInsideSphere( osg::Vec3d midPoint, float radius )
{
	return ( new osg::BoundingSphere( midPoint, radius ) )->contains( cameraManipulator->getCameraPosition() );
}

void CoreGraph::addTranslateToGraphRotTransf( osg::Vec3d pos )
{
	osg::Matrixd matrix = graphRotTransf->getMatrix();
	osg::Vec3d vypis;
	osg::Vec3f eye, center, up;
	double fovy, ar, zNear, zFar;
	double ViewportWidth, ViewPortHeight;
	int debug = 0;

	camera->getProjectionMatrixAsPerspective( fovy, ar, zNear, zFar );
	camera->getViewMatrixAsLookAt( eye, center, up );

	double x, y, z;
	x = -pos.x() * 1000;
	y = -pos.z() * 1000;
	z = -pos.y() * 1000;

	if ( debug ) {
		vypis.x() = x;
		vypis.y() = y;
		vypis.z() = z;
	}
	osg::ref_ptr<osg::Vec3Array> coordinates = new osg::Vec3Array;

	QMap<qlonglong, osg::ref_ptr<Data::Node> >::const_iterator i = in_nodes->constBegin();

	while ( i != in_nodes->constEnd() ) {
		coordinates->push_back( ( *i )->targetPositionConstRef() );
		++i;
	}

	osg::Vec3 massCenter = Vwr::DataHelper::getMassCenter( coordinates );

	double distance = fabs( static_cast<double>( center.y() - massCenter.z() ) );

	ViewPortHeight = tan( ( fovy/2 )*3.14159265 / 180.0 )*fabs( distance );
	ViewportWidth = ViewPortHeight * ar;

	x = x * fabs( ViewportWidth/( 950+y ) );
	z = z * fabs( ViewPortHeight/( 450+y ) );

	if ( debug ) {
		qDebug() << "pos x,y,z " << x << "(" << vypis.x() << ")," << y << "(" << vypis.y() << ")," << z << "(" << vypis.z() << "), " << ViewportWidth << " x " << ViewPortHeight;
	}

	osg::Vec3d result;
	result.x() = x;
	result.y() = y;
	result.z() = z;

	matrix.makeTranslate( result );

	graphRotTransf->setMatrix( matrix );
}

//jurik
void CoreGraph::turnOnShadows()
{
	osg::ref_ptr<osgShadow::SoftShadowMap> sm = new osgShadow::SoftShadowMap;
	//sm->setBias(0.1);
	//sm->setSoftnessWidth(0.1);
	shadowedScene->setShadowTechnique( sm.get() );
}

void CoreGraph::turnOffShadows()
{
	shadowedScene->setShadowTechnique( NULL );
}

void CoreGraph::turnOnBase()
{
	baseGeode->setNodeMask( 0x1 );
}

void CoreGraph::turnOffBase()
{
	baseGeode->setNodeMask( 0x0 );
}

void CoreGraph::turnAxes( bool turnOn )
{
	if ( turnOn ) {
		axesGeode->setNodeMask( 0x1 );
	}
	else {
		axesGeode->setNodeMask( 0x0 );
	}
}

void CoreGraph::createBase()
{
	osg::Geometry* baseGeometry = new osg::Geometry();

	baseGeode->addDrawable( baseGeometry );
	//invisible untill checkbox clicked
	baseGeode->setNodeMask( 0x0 );
	osg::Material* material = new osg::Material();
	material->setDiffuse( osg::Material::FRONT,  osg::Vec4( 0.8f, 0.8f, 0.8f, 0.2f ) );
	// material->setEmission(osg::Material::FRONT, osg::Vec4(0, 0, 0, 1));
	baseGeode->getOrCreateStateSet()->setAttribute( material );

	baseTransform->addChild( baseGeode );
	shadowedScene->addChild( baseTransform );

	//base
	osg::Vec3Array* vertices = new osg::Vec3Array;
	vertices->push_back( osg::Vec3( -1, -1, 0 ) ); // lb
	vertices->push_back( osg::Vec3( 1, -1, 0 ) ); // rb
	vertices->push_back( osg::Vec3( 1,  1, 0 ) ); // rt
	vertices->push_back( osg::Vec3( -1,  1, 0 ) ); // lt
	baseGeometry->setVertexArray( vertices );

	osg::DrawElementsUInt* base = new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
	base->push_back( 3 );
	base->push_back( 2 );
	base->push_back( 1 );
	base->push_back( 0 );

	baseGeometry->addPrimitiveSet( base );

	baseGeode->getOrCreateStateSet()->setMode( GL_BLEND, osg::StateAttribute::ON );
	baseGeode->getOrCreateStateSet()->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
	baseGeode->getOrCreateStateSet()->setRenderBinDetails( 1, "DepthSortedBin" );
}

//set aruco modelView matrix
void CoreGraph::recievedMVMatrix( QMatrix4x4 modelViewMatrix )
{
	osg::Matrixd arucoMVM( modelViewMatrix.operator()( 0,0 ),modelViewMatrix.operator()( 0,1 ),modelViewMatrix.operator()( 0,2 ),modelViewMatrix.operator()( 0,3 ),
						   modelViewMatrix.operator()( 1,0 ),modelViewMatrix.operator()( 1,1 ),modelViewMatrix.operator()( 1,2 ),modelViewMatrix.operator()( 1,3 ),
						   modelViewMatrix.operator()( 2,0 ),modelViewMatrix.operator()( 2,1 ),modelViewMatrix.operator()( 2,2 ),modelViewMatrix.operator()( 2,3 ),
						   modelViewMatrix.operator()( 3,0 ),modelViewMatrix.operator()( 3,1 ),modelViewMatrix.operator()( 3,2 ),modelViewMatrix.operator()( 3,3 ) );

	camera->setViewMatrix( arucoMVM );
	//update base size
	baseTransform->setMatrix( osg::Matrixd::identity() );
	updateBase( baseSize );
}

//set aruco projection matrix
void CoreGraph::recievedPMatrix( QMatrix4x4 projectionMatrix )
{
	osg::Matrixd arucoPM( projectionMatrix.operator()( 0,0 ),projectionMatrix.operator()( 0,1 ),projectionMatrix.operator()( 0,2 ),projectionMatrix.operator()( 0,3 ),
						  projectionMatrix.operator()( 1,0 ),projectionMatrix.operator()( 1,1 ),projectionMatrix.operator()( 1,2 ),projectionMatrix.operator()( 1,3 ),
						  projectionMatrix.operator()( 2,0 ),projectionMatrix.operator()( 2,1 ),projectionMatrix.operator()( 2,2 ),projectionMatrix.operator()( 2,3 ),
						  projectionMatrix.operator()( 3,0 ),projectionMatrix.operator()( 3,1 ),projectionMatrix.operator()( 3,2 ),projectionMatrix.operator()( 3,3 ) );

	camera->setProjectionMatrix( arucoPM );
}

//scale base to comfort graph size
void CoreGraph::updateBase( double size )
{
	osg::Matrixd originalMatrix = baseTransform->getMatrix();
	osg::Matrixd scaleMatrix( size,0,0,0,
							  0,size,0,0,
							  0,0,1,0,
							  0,0,0,1 );

	baseTransform->setMatrix( originalMatrix * scaleMatrix );

}

//finds out graph size, position it over marker center and scale base
void CoreGraph::scaleGraphToBase()
{

	//if aruco mode, we need to scale graph
	//otherwise we need just to position base
	if ( arucoRunning ) {
		osg::Vec3f maxPosition( 0,0,0 );
		osg::Vec3f minPosition( 1000,1000,1000 );

		QMapIterator<qlonglong, osg::ref_ptr<Data::Node> > it( *in_nodes );

		//get maximum and minimum of each axis
		while ( it.hasNext() ) {
			it.next();

			if ( it.value()->getCurrentPosition().x() > maxPosition.x() ) {
				maxPosition.x() = it.value()->getCurrentPosition().x();
			}
			if ( it.value()->getCurrentPosition().x() < minPosition.x() ) {
				minPosition.x() = it.value()->getCurrentPosition().x();
			}

			if ( it.value()->getCurrentPosition().y() > maxPosition.y() ) {
				maxPosition.y() = it.value()->getCurrentPosition().y();
			}
			if ( it.value()->getCurrentPosition().y() < minPosition.y() ) {
				minPosition.y() = it.value()->getCurrentPosition().y();
			}

			// z minimum is to get graph over base
			if ( it.value()->getCurrentPosition().z() < minPosition.z() ) {
				minPosition.z() = it.value()->getCurrentPosition().z();
			}
		}

		// calculate translation to aruco marker center
		// mid X and Y axis and minimal z axis value to get graph over base
		osg::Vec3f centerGraph(
			( maxPosition.x() - ( ( maxPosition.x() + ( minPosition.x() * -1 ) ) / 2 ) ) * -1,
			( maxPosition.y() - ( ( maxPosition.y() + ( minPosition.y() * -1 ) ) / 2 ) ) * -1,
			( minPosition.z() * -1 ) + 50 );

		//translate to aruco center graph
		osg::Matrixd positionMatrix = graphRotTransf->getMatrix();
		positionMatrix.setTrans( centerGraph );
		graphRotTransf->setMatrix( positionMatrix );

		//scale aruco base
		baseSize = static_cast<double>( getFurthestPosition( maxPosition,minPosition ) );
	}
	else {
		osg::Vec3f minPosition( 1000,1000,1000 );

		QMapIterator<qlonglong, osg::ref_ptr<Data::Node> > it( *in_nodes );

		//get maximum and minimum of each axis
		while ( it.hasNext() ) {
			it.next();
			// z minimum is to get graph over base
			if ( it.value()->getCurrentPosition().z() < minPosition.z() ) {
				minPosition.z() = it.value()->getCurrentPosition().z();
			}
		}

		osg::Vec3f moveBase( 0,0,minPosition.z() - 50 );

		//translate to aruco center graph
		osg::Matrixd moveBaseMatrix = baseTransform->getMatrix();
		moveBaseMatrix.setTrans( moveBase );
		baseTransform->setMatrix( moveBaseMatrix );
	}

}

void CoreGraph::scaleGraph( int scale )
{
	osg::Matrixd scaleMatrix = graphRotTransf->getMatrix();
	//outputMatrix( scaleMatrix);
	switch ( scale ) {
		case 1: {
			graphRotTransf->setMatrix( scaleMatrix * scaleMatrix.scale( 0.5,0.5,0.5 ) );
			break;
		}
		case 2: {
			graphRotTransf->setMatrix( scaleMatrix * scaleMatrix.scale( 2,2,2 ) );
			break;
		}
		default:
			//scale wont have any other value
			qDebug() << "Unhandled scale value";
	}
}

//rotate aruco graph (NOT CAMERA) by Z axis (occasionaly Y)
void CoreGraph::rotateGraph( int direction )
{
	osg::Matrixd transfGraph = graphRotTransf->getMatrix();
	osg::Vec3f translate = transfGraph.getTrans();
	osg::Matrixd inverted = inverted.inverse( transfGraph );
	transfGraph.setTrans( inverted.getTrans() );

	switch ( direction ) {
		case 1: {
			rotationMatrix = rotationMatrix.rotate( 0.025,0,0,1 );
			break;
		}
		case -1: {
			rotationMatrix = rotationMatrix.rotate( -0.025,0,0,1 );
			break;
		}
		case 2: {
			rotationMatrix = rotationMatrix.rotate( 0.025,0,1,0 );
			break;
		}
		case -2: {
			rotationMatrix = rotationMatrix.rotate( -0.025,0,1,0 );
			break;
		}
		default:
			//direction wont have any other value
			qDebug() << "Unhandled direction value";
	}

	transfGraph = transfGraph * rotationMatrix;
	transfGraph.setTrans( translate );
	graphRotTransf->setMatrix( transfGraph );

}

void CoreGraph::outputMatrix( osg::Matrixd matrix )
{
	qDebug() << matrix.operator()( 0,0 ) << matrix.operator()( 0,1 ) << matrix.operator()( 0,2 ) << matrix.operator()( 0,3 );
	qDebug() << matrix.operator()( 1,0 ) << matrix.operator()( 1,1 ) << matrix.operator()( 1,2 ) << matrix.operator()( 1,3 );
	qDebug() << matrix.operator()( 2,0 ) << matrix.operator()( 2,1 ) << matrix.operator()( 2,2 ) << matrix.operator()( 2,3 );
	qDebug() << matrix.operator()( 3,0 ) << matrix.operator()( 3,1 ) << matrix.operator()( 3,2 ) << matrix.operator()( 3,3 );
}

void CoreGraph::ratata( double initialX,double actualX,double initialY, double actualY )
{
	osg::Matrixd transfGraph = graphRotTransf->getMatrix();
	if ( actualX > initialX +5 ) {
		rotationMatrix = rotationMatrix.rotate( 0.05,0,0,1 );
	}

	if ( actualX < initialX -5 ) {
		rotationMatrix = rotationMatrix.rotate( -0.025,0,0,1 );
	}

	/*if(actualY > initialY +5){
	    rotationMatrix = rotationMatrix * rotationMatrix.rotate(-0.05,osg::Vec3f(1,0,0));
	}
	if(actualY < initialY -5){
	    rotationMatrix = rotationMatrix * rotationMatrix.rotate(0.05,osg::Vec3f(1,0,0));
	}*/

	graphRotTransf->setMatrix( transfGraph * rotationMatrix );

}

//scale nodeGroup nodes
void CoreGraph::scaleNodes( bool scaleUp )
{
	QMapIterator<qlonglong, osg::ref_ptr<Data::Node> > it( *in_nodes );

	while ( it.hasNext() ) {
		it.next();

		float actualScale = it.value()->getScale();

		if ( scaleUp ) {
			if ( actualScale < 100 ) {
				it.value()->setScale( actualScale * 1.2f );
			}
			else {
				it.value()->setScale( actualScale );
			}
		}
		else {
			if ( actualScale > 1 ) {
				it.value()->setScale( actualScale * 0.8f );
			}
			else {
				it.value()->setScale( actualScale );
			}

		}
		//it.value()->reloadConfig();
		//reload(graph);
	}
}

//get furthest position in graph
float CoreGraph::getFurthestPosition( osg::Vec3f max,osg::Vec3f min )
{
	float x;
	float y;

	if ( max.x() > ( min.x() * -1 ) ) {
		x = max.x();
	}
	else {
		x = min.x() * -1;
	}

	if ( max.y() > ( min.y() * -1 ) ) {
		y = max.y();
	}
	else {
		y = min.y() * -1;
	}

	if ( x > y ) {
		return x;
	}
	else {
		return y;
	}
}

void CoreGraph::setArucoRunning( bool isRunning )
{
	this->arucoRunning = isRunning;
}

void CoreGraph::drawAxes()
{

	osg::Geometry* axesGeometry = new osg::Geometry();
	axesGeode->addDrawable( axesGeometry );
	axesGeode->setNodeMask( 0x0 );

	osg::StateSet* stateset = new osg::StateSet;
	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth( 3.0f );
	stateset->setAttributeAndModes( linewidth,osg::StateAttribute::ON );
	stateset->setMode( GL_LIGHTING,osg::StateAttribute::OFF );
	axesGeometry->setStateSet( stateset );

	//base
	osg::Vec3Array* vertices = new osg::Vec3Array;
	vertices->push_back( osg::Vec3( 0, 0, 0 ) ); // base
	vertices->push_back( osg::Vec3( 250, 0, 0 ) ); // x
	vertices->push_back( osg::Vec3( 0, 0, 0 ) ); // base
	vertices->push_back( osg::Vec3( 0, 250, 0 ) ); // y
	vertices->push_back( osg::Vec3( 0, 0, 0 ) ); // base
	vertices->push_back( osg::Vec3( 0, 0, 250 ) ); // z
	axesGeometry->setVertexArray( vertices );

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back( osg::Vec4( 1.0f, 0.0f, 0.0f, 1.0f ) ); //index 0 red
	colors->push_back( osg::Vec4( 1.0f, 0.0f, 0.0f, 1.0f ) ); //index 0 red
	colors->push_back( osg::Vec4( 0.0f, 1.0f, 0.0f, 1.0f ) ); //index 1 green
	colors->push_back( osg::Vec4( 0.0f, 1.0f, 0.0f, 1.0f ) ); //index 0 red
	colors->push_back( osg::Vec4( 0.0f, 0.0f, 1.0f, 1.0f ) ); //index 2 blue
	colors->push_back( osg::Vec4( 0.0f, 0.0f, 1.0f, 1.0f ) ); //index 3 white
	axesGeometry->setColorArray( colors );
	axesGeometry->setColorBinding( osg::Geometry::BIND_PER_VERTEX );

	osg::DrawElementsUInt* axisX = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
	axisX->push_back( 0 );
	axisX->push_back( 1 );

	osg::DrawElementsUInt* axisY = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES,0 );
	axisY->push_back( 2 );
	axisY->push_back( 3 );

	osg::DrawElementsUInt* axisZ = new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
	axisZ->push_back( 4 );
	axisZ->push_back( 5 );

	axesGeometry->addPrimitiveSet( axisX );
	axesGeometry->addPrimitiveSet( axisY );
	axesGeometry->addPrimitiveSet( axisZ );

}

//JMA
osg::Vec3f CoreGraph::getGrafRotTransVec()
{
	return graphRotTransf->getMatrix().getTrans();
}

//*****
}
