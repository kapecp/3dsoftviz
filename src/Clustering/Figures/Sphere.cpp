#include "Clustering/Figures/Sphere.h"

#include <vector>
#include <algorithm>
#include <QDebug>
#include <osg/Depth>
#include <osg/BlendFunc>
#include <osg/Material>

/* If VS2010 compiler is used , we need to define math.h constants*/
#ifdef _MSC_VER
#if (_MSC_VER == 1600)
#define _USE_MATH_DEFINES
#endif
#endif

#include <math.h>

Clustering::Sphere::Sphere( osg::Vec3d position, double radius, osg::Vec4d color )
{
	init();
	computeGeode( 20, 20 );
	transform( position, osg::Vec3d( radius,radius,radius ), color );
	at->addChild( sphereGeode );
}

void Clustering::Sphere::init()
{
	midPoint = osg::Vec3d( 0,0,0 );
	sphereGeode = new osg::Geode();
	sphereGeometry = new osg::Geometry();
	sphereVertices = new osg::Vec3Array;
	sphereNormals = new osg::Vec3Array;
	sphereTexCoords = new osg::Vec2Array;
	spherePrimitiveSets = new std::vector<osg::DrawElementsUInt*>;
	at = new osg::AutoTransform;
}

void Clustering::Sphere::computeGeode( unsigned int rings, unsigned int sectors )
{
	float radius = 1;
	float const R = 1.0f/static_cast<float>( rings-1 );
	float const S = 1.0f/static_cast<float>( sectors-1 );
	unsigned int r, s;

	sphereGeode->addDrawable( sphereGeometry );

	// Establish texture coordinates, vertex list, and normals
	for ( r = 0; r < rings; r++ ) {
		for ( s = 0; s < sectors; s++ ) {
			float const y = static_cast<float>( sin( -sqrt( M_PI ) + M_PI * static_cast<double>( static_cast<float>( r ) * R ) ) );
			float const x = static_cast<float>( cos( 2*M_PI * static_cast<double>( static_cast<float>( s ) * S ) ) ) * static_cast<float>( sin( M_PI * static_cast<double>( static_cast<float>( r ) * R ) ) );
			float const z = static_cast<float>( sin( 2*M_PI * static_cast<double>( static_cast<float>( s ) * S ) ) ) * static_cast<float>( sin( M_PI * static_cast<double>( static_cast<float>( r ) * R ) ) );

			sphereTexCoords->push_back( osg::Vec2( static_cast<float>( s ) * R, static_cast<float>( r ) * R ) );
			sphereVertices->push_back( osg::Vec3( x * radius, y * radius, z * radius ) );
			sphereNormals->push_back( osg::Vec3( x, y, z ) );
		}
	}

	sphereGeometry->setNormalArray( sphereNormals );
	sphereGeometry->setVertexArray( sphereVertices );
	sphereGeometry->setTexCoordArray( 0, sphereTexCoords );

	// Generate quads for each face.
	for ( r = 0; r < rings-1; r++ ) {
		for ( s = 0; s < sectors-1; s++ ) {
			spherePrimitiveSets->push_back( new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 ) );

			osg::DrawElementsUInt* face = spherePrimitiveSets->back();

			// Corners of quads should be in CCW order.
			face->push_back( ( r + 0 ) * sectors + ( s + 0 ) );
			face->push_back( ( r + 0 ) * sectors + ( s + 1 ) );
			face->push_back( ( r + 1 ) * sectors + ( s + 1 ) );
			face->push_back( ( r + 1 ) * sectors + ( s + 0 ) );

			sphereGeometry->addPrimitiveSet( face );
		}
	}

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back( color );
	sphereGeometry->setColorArray( colors );
}

void Clustering::Sphere::transform( osg::Vec3d position, osg::Vec3d scale, osg::Vec4d color )
{
	osg::StateSet* ss = sphereGeometry->getOrCreateStateSet();

	osg::Material* material = new osg::Material();
	material->setAmbient( osg::Material::FRONT,color );
	material->setDiffuse( osg::Material::FRONT,color );
	material->setSpecular( osg::Material::FRONT,color );
	material->setAlpha( osg::Material::FRONT,static_cast<float>( color.a() ) );

	ss->setAttribute( material,osg::StateAttribute::OVERRIDE );
	ss->setMode( GL_DEPTH_TEST,osg::StateAttribute::ON );
	ss->setMode( GL_LIGHTING,osg::StateAttribute::OFF );
	ss->setMode( GL_BLEND,osg::StateAttribute::ON );
	ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask( false );
	ss->setAttributeAndModes( depth, osg::StateAttribute::ON );
	ss->setAttributeAndModes( new osg::BlendFunc, osg::StateAttribute::ON );

	at->setPosition( position * 1 );
	at->setScale( scale );
}
