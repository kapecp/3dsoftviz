#ifndef CUBE_H
#define CUBE_H

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/AutoTransform>

#include <vector>

namespace Clustering {

class Cube
{

public:

	Cube( osg::Vec3d position, float radius );
	Cube( osg::Vec3d position, float radius, osg::Vec4d color );

	osg::AutoTransform* getAT()
	{
		return at;
	}
	osg::Geode*     getGeode()
	{
		return geode;
	}
	osg::Geometry*  getGeometry()
	{
		return geometry;
	}
	osg::Vec3Array* getVertices()
	{
		return vertices;
	}
	osg::Vec3Array* getNormals()
	{
		return normals;
	}
	osg::Vec2Array* getTexCoords()
	{
		return texCoords;
	}

	osg::Vec3d getMidpoint()
	{
		return at->getPosition();
	}
	double getRadius()
	{
		return at->getScale().x();
	}

	void transform( osg::Vec3d position, osg::Vec3d scale, osg::Vec4d color );

protected:

	osg::AutoTransform* at;
	osg::Geode*          geode;
	osg::Geometry*       geometry;
	osg::Vec3Array*      vertices;
	osg::Vec3Array*      normals;
	osg::Vec2Array*      texCoords;

	std::vector<osg::DrawElementsUInt*>* primitiveSets;

	void computeGeode();

};

}

#endif  /*CUBE_H*/
