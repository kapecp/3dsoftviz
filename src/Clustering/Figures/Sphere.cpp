#include "Clustering/Figures/Sphere.h"

#include <vector>
#include <QDebug>

Sphere::Sphere(float radius, unsigned int rings, unsigned int sectors) {
    init();
    computeGeode(radius, rings, sectors);
}

Sphere::Sphere(float radius, unsigned int rings, unsigned int sectors, osg::Vec4d color) {
    init();
    computeGeode(radius, rings, sectors);
    setColor(color);
}

void Sphere::init() {
    midPoint = osg::Vec3d(0,0,0);
    sphereGeode = new osg::Geode();
    sphereGeometry = new osg::Geometry();
    sphereVertices = new osg::Vec3Array;
    sphereNormals = new osg::Vec3Array;
    sphereTexCoords = new osg::Vec2Array;
    spherePrimitiveSets = new std::vector<osg::DrawElementsUInt *>;
}

void Sphere::computeGeode(float radius, unsigned int rings, unsigned int sectors) {
    float const R = 1./(float)(rings-1);
    float const S = 1./(float)(sectors-1);
    int r, s;

    sphereGeode->addDrawable( sphereGeometry );

    // Establish texture coordinates, vertex list, and normals
    for(r = 0; r < rings; r++) {
        for(s = 0; s < sectors; s++) {
            float const y = sin( -M_PI_2 + M_PI * r * R);
            float const x = cos(2*M_PI * s * S) * sin(M_PI * r * R);
            float const z = sin(2*M_PI * s * S) * sin(M_PI * r * R);

            sphereTexCoords->push_back(osg::Vec2(s * R, r * R));
            sphereVertices->push_back(osg::Vec3(x * radius, y * radius, z * radius));
            sphereNormals->push_back(osg::Vec3(x, y, z));
        }
    }

    sphereGeometry->setVertexArray  (sphereVertices);
    sphereGeometry->setTexCoordArray(0, sphereTexCoords);

    // Generate quads for each face.
    for(r = 0; r < rings-1; r++) {
        for(s = 0; s < sectors-1; s++) {
            spherePrimitiveSets->push_back(new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0));

            osg::DrawElementsUInt * face = spherePrimitiveSets->back();

            // Corners of quads should be in CCW order.
            face->push_back((r + 0) * sectors + (s + 0));
            face->push_back((r + 0) * sectors + (s + 1));
            face->push_back((r + 1) * sectors + (s + 1));
            face->push_back((r + 1) * sectors + (s + 0));

            sphereGeometry->addPrimitiveSet(face);
        }
    }

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(color);
    sphereGeometry->setColorArray(colors);
}
