#include "Clustering/Figures/SpikySphere.h"

Clustering::SpikySphere::SpikySphere( osg::Vec3d position, float radius, osg::Vec4d color ) : Clustering::Sphere( position, radius, color )
{
	addSpikes();
}

void Clustering::SpikySphere::addSpikes()
{
	for ( osg::Vec3Array::iterator i = sphereVertices->begin(); i != sphereVertices->end(); ++i ) {
		osg::Vec3 sp( i->x(),i->y(),i->z() );
		osg::Vec3 ep = ( sp - midPoint ) * 1.5;
		osg::ref_ptr<osg::Geometry> lineGeometry( new osg::Geometry );
		osg::ref_ptr<osg::Vec3Array> points = new osg::Vec3Array;
		points->push_back( sp );
		points->push_back( ep );
		osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
		colorArray->push_back( color );
		lineGeometry->setVertexArray( points.get() );
		lineGeometry->setColorArray( colorArray );
		lineGeometry->setColorBinding( osg::Geometry::BIND_OVERALL );
		lineGeometry->addPrimitiveSet( new osg::DrawArrays( GL_LINES,0,2 ) );

//        osg::Geode* lineGeode = new osg::Geode();
//        lineGeode->addDrawable(lineGeometry);
		sphereGeode->addDrawable( lineGeometry );
//        at->addChild(lineGeode);
	}
}
