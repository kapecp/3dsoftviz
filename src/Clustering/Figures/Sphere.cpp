#include "Clustering/Figures/Sphere.h"

#include <vector>
#include <QDebug>

Sphere::Sphere(osg::Vec3d position, float radius, osg::Vec4d color) {
    init();
    computeGeode(20, 20);
    transform(position, radius, color);
    at->addChild(sphereGeode);
}

void Sphere::init() {
    midPoint = osg::Vec3d(0,0,0);
    sphereGeode = new osg::Geode();
    sphereGeometry = new osg::Geometry();
    sphereVertices = new osg::Vec3Array;
    sphereNormals = new osg::Vec3Array;
    sphereTexCoords = new osg::Vec2Array;
    spherePrimitiveSets = new std::vector<osg::DrawElementsUInt *>;
    at = new osg::AutoTransform;
}

void Sphere::computeGeode(unsigned int rings, unsigned int sectors) {
    float radius = 1;
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

    sphereGeometry->setNormalArray(sphereNormals);
    sphereGeometry->setVertexArray(sphereVertices);
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

void Sphere::transform(osg::Vec3d position, float radius, osg::Vec4d color) {
    osg::StateSet * ss = sphereGeometry->getOrCreateStateSet();

    osg::Material* material = new osg::Material();
    material->setAmbient(osg::Material::FRONT,color);
    material->setDiffuse(osg::Material::FRONT,color);
    material->setSpecular(osg::Material::FRONT,color);
    material->setAlpha(osg::Material::FRONT,color.a());

    ss->setAttribute(material,osg::StateAttribute::OVERRIDE);
    ss->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
    ss->setMode(GL_LIGHTING,osg::StateAttribute::ON);
    ss->setMode(GL_BLEND,osg::StateAttribute::ON);
    ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    at->setPosition(position * 1);
    at->setScale(radius);
}
