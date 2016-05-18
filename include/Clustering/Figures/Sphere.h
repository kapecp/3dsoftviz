#ifndef SPHERE_H
#define SPHERE_H

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/AutoTransform>

#include <vector>

namespace Clustering {

class Sphere
{

public:

	Sphere( osg::Vec3d position, float radius, osg::Vec4d color );

	osg::AutoTransform* getAT()
	{
		return at;
	}
	osg::Geode*     getGeode()
	{
		return sphereGeode;
	}
	osg::Geometry*  getGeometry()
	{
		return sphereGeometry;
	}
	osg::Vec3Array* getVertices()
	{
		return sphereVertices;
	}
	osg::Vec3Array* getNormals()
	{
		return sphereNormals;
	}
	osg::Vec2Array* getTexCoords()
	{
		return sphereTexCoords;
	}

	void setColor( osg::Vec4d color )
	{
		this->color = color;
	}

	void transform( osg::Vec3d position, osg::Vec3d scale, osg::Vec4d color );

protected:

	osg::Vec3d        midPoint;
	osg::Vec4d        color;

	osg::AutoTransform*   at;
	osg::Geode*           sphereGeode;
	osg::Geometry*        sphereGeometry;
	osg::Vec3Array*       sphereVertices;
	osg::Vec3Array*       sphereNormals;
	osg::Vec2Array*       sphereTexCoords;

	std::vector<osg::DrawElementsUInt*>* spherePrimitiveSets;

	void init();
	void computeGeode( unsigned int rings, unsigned int sectors );

};

}

#endif  /*SPHERE_H*/
