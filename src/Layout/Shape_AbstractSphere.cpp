#include "Layout/Shape_AbstractSphere.h"

namespace Layout {

Shape_AbstractSphere::Shape_AbstractSphere (
		const osg::Vec3f center,
		const float radius
		) :	center_ (center),
	radius_ (radius)
{
	// nothing

}

osg::Vec3f & Shape_AbstractSphere::getCenter () {
	return center_;
}

float Shape_AbstractSphere::getRadius () {
	return radius_;
}

/*
//volovar zac
void Shape_AbstractSphere::setAlpha(float alpha)
{
    alpha_ = alpha;
}

float Shape_AbstractSphere::getAlpha()
{
    return alpha_;
}

void Shape_AbstractSphere::setRenderType(RenderType rt)
{
    renderType_ = rt;
}

Shape_AbstractSphere::RenderType Shape_AbstractSphere::getRenderType()
{
    return renderType_;
}

//volovar kon
*/
} // namespace
