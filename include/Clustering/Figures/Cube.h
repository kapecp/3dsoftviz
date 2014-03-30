#ifndef CUBE_H
#define CUBE_H

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/AutoTransform>

class Cube
{

public:

    Cube(osg::Vec3d position, float radius);
    Cube(osg::Vec3d position, float radius, osg::Vec4d color);

    osg::AutoTransform * getAT()    { return at;        }
    osg::Geode     *getGeode()      { return geode;     }
    osg::Geometry  *getGeometry()   { return geometry;  }
    osg::Vec3Array *getVertices()   { return vertices;  }
    osg::Vec3Array *getNormals()    { return normals;   }
    osg::Vec2Array *getTexCoords()  { return texCoords; }

    void setColor(osg::Vec4d color) { this->color = color; }

    osg::Vec3d getMidpoint() { return midPoint; }
    float getRadius() { return radius; }

protected:

    osg::Vec3d        midPoint;
    float             radius;
    osg::Vec4d        color;

    osg::AutoTransform * at;
    osg::Geode         * geode;
    osg::Geometry      * geometry;
    osg::Vec3Array     * vertices;
    osg::Vec3Array     * normals;
    osg::Vec2Array     * texCoords;

    std::vector<osg::DrawElementsUInt *> * primitiveSets;

    void init(osg::Vec3d position, float radius);
    void computeGeode();
    void transform(osg::Vec3d position, float radius);

};

#endif  /*CUBE_H*/
