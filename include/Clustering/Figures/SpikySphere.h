#ifndef SPIKYSPHERE_H
#define SPIKYSPHERE_H

#include "Clustering/Figures/Sphere.h"

namespace Clustering {

class SpikySphere : public Clustering::Sphere
{

public:

	SpikySphere( osg::Vec3d position, float radius, osg::Vec4d color );

private:

	void addSpikes();

};

}

#endif  /*SPIKYSPHERE_H*/
