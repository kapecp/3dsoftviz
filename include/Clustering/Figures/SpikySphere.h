#ifndef SPIKYSPHERE_H
#define SPIKYSPHERE_H

#include "Clustering/Figures/Sphere.h"

class SpikySphere : public Sphere
{

public:

    SpikySphere(float radius, unsigned int rings, unsigned int sectors);
    SpikySphere(float radius, unsigned int rings, unsigned int sectors, osg::Vec4d color);

private:

    void addSpikes();

};

#endif  /*SPIKYSPHERE_H*/
