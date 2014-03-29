#ifndef SPHERE_H
#define SPHERE_H

#include <osg/Geode>
#include <osg/Geometry>

class Sphere
{

public:

    Sphere(float radius, unsigned int rings, unsigned int sectors);
    Sphere(float radius, unsigned int rings, unsigned int sectors, osg::Vec4d color);

    osg::Geode     *getGeode()      { return sphereGeode;     }
    osg::Geometry  *getGeometry()   { return sphereGeometry;  }
    osg::Vec3Array *getVertices()   { return sphereVertices;  }
    osg::Vec3Array *getNormals()    { return sphereNormals;   }
    osg::Vec2Array *getTexCoords()  { return sphereTexCoords; }

    void setColor(osg::Vec4d color) { this->color = color; }

protected:

    osg::Vec3d        midPoint;
    osg::Vec4d        color;

    osg::Geode      * sphereGeode;
    osg::Geometry   * sphereGeometry;
    osg::Vec3Array  * sphereVertices;
    osg::Vec3Array  * sphereNormals;
    osg::Vec2Array  * sphereTexCoords;

    std::vector<osg::DrawElementsUInt *> * spherePrimitiveSets;

    void init();
    void computeGeode(float radius, unsigned int rings, unsigned int sectors);

};

#endif  /*SPHERE_H*/
