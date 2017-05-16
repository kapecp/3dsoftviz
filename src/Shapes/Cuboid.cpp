#include "Shapes/Cuboid.h"
#include <osg/Vec3>
#include <osg/Geometry>
#include <QVector>

namespace Shapes {
Cuboid::Cuboid( float width, float height, float depth, const osg::Vec3& offset )
	: Cuboid( osg::BoundingBox( -width / 2 + offset.x(), -depth / 2 + offset.y(), -height / 2 + offset.z(), width / 2 + offset.x(), depth / 2 + offset.y(), height / 2 + offset.z() ) )
{
}

Cuboid::Cuboid( const osg::BoundingBox& boundingBox )
{
	const float right = boundingBox.xMax();
	const float left = boundingBox.xMin();
	const float top = boundingBox.zMax();
	const float bottom = boundingBox.zMin();
	const float farr = boundingBox.yMax();
	const float nearr = boundingBox.yMin();

	const osg::Vec3 vs[] = {
		osg::Vec3( left, nearr, bottom ),		// 0
		osg::Vec3( right, nearr, bottom ),	// 1
		osg::Vec3( right, nearr, top ),		// 2
		osg::Vec3( left, nearr, top ),		// 3
		osg::Vec3( left, farr, bottom ),		// 4
		osg::Vec3( right, farr, bottom ),		// 5
		osg::Vec3( right, farr, top ),		// 6
		osg::Vec3( left, farr, top )			// 7
	};

	const osg::Vec3 ns[] = {
		osg::Vec3( 0, -1, 0 ),
		osg::Vec3( 1, 0, 0 ),
		osg::Vec3( 0, 1, 0 ),
		osg::Vec3( -1, 0, 0 ),
		osg::Vec3( 0, 0, 1 ),
		osg::Vec3( 0, 0, -1 )
	};

	const osg::Vec2 cs[] = {
		osg::Vec2( 0, 0 ),
		osg::Vec2( 1, 0 ),
		osg::Vec2( 1, 1 ),
		osg::Vec2( 0, 1 )
	};

	const GLuint fs[] = {
		0, 1, 2, 3,	// near
		1, 5, 6, 2,	// right
		5, 4, 7, 6,	// far
		4, 0, 3, 7,	// left
		3, 2, 6, 7,	// top
		4, 5, 1, 0	// bottom
	};

	QVector<osg::Vec3> vertices;
	QVector<osg::Vec3> normals;
	QVector<osg::Vec2> coords;
	QVector<GLuint> quads;
	for ( GLuint i = 0; i < sizeof( fs ) / sizeof( GLuint ); ++i ) {
		vertices << vs[fs[i]];
		normals << ns[i / 4];
		coords << cs[i % 4];
		quads << i;
	}

	auto geom = new osg::Geometry;
	geom->setVertexArray( new osg::Vec3Array( static_cast<unsigned int>( vertices.count() ), vertices.data() ) );
	geom->setNormalArray( new osg::Vec3Array( static_cast<unsigned int>( normals.count() ), normals.data() ) );
	geom->setTexCoordArray( 0, new osg::Vec2Array( static_cast<unsigned int>( coords.count() ), coords.data() ) );
	geom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
	for ( int i = 0; i < quads.count(); i += 4 ) {
		geom->addPrimitiveSet( new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 4, quads.data() + i ) );
	}
	Geode::addDrawable( geom );
}
}
