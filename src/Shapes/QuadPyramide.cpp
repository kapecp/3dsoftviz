#include "Shapes//QuadPyramide.h"
#include <osg/Vec3>
#include <osg/Geometry>
#include <QVector>

namespace Shapes {
osg::Vec3 normalize( const osg::Vec3 v )
{
	auto v2 = v;
	v2.normalize();
	return v2;
}

QuadPyramide::QuadPyramide( float baseWidth, float baseDepth, float height, const osg::Vec3& offset )
{
	const float right = baseWidth / 2 + offset.x();
	const float left = -baseWidth / 2 + offset.x();
	const float top = height + offset.z();
	const float bottom = 0 + offset.z();
	const float farr = baseDepth / 2 + offset.y();
	const float nearr = -baseDepth / 2 + offset.y();

	const osg::Vec3 vs[] = {
		osg::Vec3( left, nearr, bottom ),		// 0
		osg::Vec3( right, nearr, bottom ),	// 1
		osg::Vec3( right, farr, bottom ),		// 2
		osg::Vec3( left, farr, bottom ),		// 3
		osg::Vec3( 0, 0, top )				// 4
	};

	const osg::Vec3 ns[] = {
		normalize( ( vs[1] - vs[0] ) ^ ( vs[4] - vs[0] ) ),	// near
		normalize( ( vs[2] - vs[1] ) ^ ( vs[4] - vs[1] ) ),	// right
		normalize( ( vs[3] - vs[2] ) ^ ( vs[4] - vs[2] ) ),	// far
		normalize( ( vs[0] - vs[3] ) ^ ( vs[4] - vs[3] ) ),	// left
		osg::Vec3( 0, 0, -1 )
	};

	const GLuint ts[] = {
		0, 1, 4,	// near
		1, 2, 4,	// right
		2, 3, 4,	// far
		3, 0, 4		// left
	};

	const GLuint qs[] = {
		0, 1, 2, 3	// bottom
	};

	QVector<osg::Vec3> vertices;
	QVector<osg::Vec3> normals;
	QVector<GLuint> triangles;
	QVector<GLuint> quads;
	for ( GLuint i = 0; i < sizeof( ts ) / sizeof( GLuint ); ++i ) {
		vertices << vs[ts[i]];
		normals << ns[i / 3];
		triangles << i;
	}
	for ( GLuint i = 0; i < sizeof( qs ) / sizeof( GLuint ); ++i ) {
		vertices << vs[qs[i]];
		normals << ns[4];
		quads << static_cast<unsigned int>( triangles.count() ) + i ;
	}

	auto geom = new osg::Geometry;
	geom->setVertexArray( new osg::Vec3Array( static_cast<unsigned int>( vertices.count() ), vertices.data() ) );
	geom->setNormalArray( new osg::Vec3Array( static_cast<unsigned int>( normals.count() ), normals.data() ) );
	geom->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );
	for ( int i = 0; i < triangles.count(); i += 3 ) {
		geom->addPrimitiveSet( new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 3, triangles.data() + i ) );
	}
	for ( int i = 0; i < quads.count(); i += 4 ) {
		geom->addPrimitiveSet( new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 4, quads.data() + i ) );
	}
	Geode::addDrawable( geom );
}
}
