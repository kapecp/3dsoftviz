#pragma once

#include <osg/Geode>

namespace Shapes {
class Cuboid : public osg::Geode
{
public:
	Cuboid( float width, float height, float depth, const osg::Vec3& offset = osg::Vec3() );
	explicit Cuboid( const osg::BoundingBox& boundingBox );
};
}
