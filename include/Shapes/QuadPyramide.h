#pragma once

#include <osg/Geode>

namespace Shapes {
class QuadPyramide : public osg::Geode
{
public:
	QuadPyramide( float baseWidth, float baseDepth, float height, const osg::Vec3& offset = osg::Vec3() );
};

osg::Vec3 normalize( const osg::Vec3 v );

}
