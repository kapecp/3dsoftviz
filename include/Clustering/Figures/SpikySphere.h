#ifndef SPIKYSPHERE_H
#define SPIKYSPHERE_H

#include "Clustering/Figures/Sphere.h"

class SpikySphere : public Sphere
{

public:

    SpikySphere(osg::Vec3d position, float radius, osg::Vec4d color);

private:

    void addSpikes();

};

#endif  /*SPIKYSPHERE_H*/
