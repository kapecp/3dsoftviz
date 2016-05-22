#pragma once

#include <osg/Geode>

namespace Clustering {
class QuadPyramide : public osg::Geode
{
public:
	QuadPyramide( float baseWidth, float baseDepth, float height, const osg::Vec3& offset = osg::Vec3() );
};
}
