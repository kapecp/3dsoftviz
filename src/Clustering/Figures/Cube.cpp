#include "Clustering/Figures/Cube.h"

#include <vector>
#include <algorithm>
#include <QDebug>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/Material>
#include <osg/Depth>
#include <osg/BlendFunc>

Clustering::Cube::Cube( osg::Vec3d position, float radius, osg::Vec4d color = osg::Vec4d( 0,0,0,1 ) )
{
	geode = new osg::Geode();
	geometry = new osg::Geometry();
	vertices = new osg::Vec3Array;
	normals = new osg::Vec3Array;
	texCoords = new osg::Vec2Array;
	primitiveSets = new std::vector<osg::DrawElementsUInt*>;
	at = new osg::AutoTransform;

	computeGeode();
	transform( position, osg::Vec3d( radius,radius,radius ), color );
	at->addChild( geode );
}

//void Cube::computeGeode() {
//    geode->addDrawable( geometry );

//    // bottom front left
//    vertices->push_back(osg::Vec3(-1, -1, -1));
//    vertices->push_back(osg::Vec3(-1, -1, -1));
//    vertices->push_back(osg::Vec3(-1, -1, -1));
//    // bottom front right
//    vertices->push_back(osg::Vec3(+1, -1, -1));
//    vertices->push_back(osg::Vec3(+1, -1, -1));
//    vertices->push_back(osg::Vec3(+1, -1, -1));
//    // bottom back right
//    vertices->push_back(osg::Vec3(+1, +1, -1));
//    vertices->push_back(osg::Vec3(+1, +1, -1));
//    vertices->push_back(osg::Vec3(+1, +1, -1));
//    // bottom back left
//    vertices->push_back(osg::Vec3(-1, +1, -1));
//    vertices->push_back(osg::Vec3(-1, +1, -1));
//    vertices->push_back(osg::Vec3(-1, +1, -1));

//    // top front left
//    vertices->push_back(osg::Vec3(-1, -1,  1));
//    vertices->push_back(osg::Vec3(-1, -1,  1));
//    vertices->push_back(osg::Vec3(-1, -1,  1));
//    // top front right
//    vertices->push_back(osg::Vec3(+1, -1,  1));
//    vertices->push_back(osg::Vec3(+1, -1,  1));
//    vertices->push_back(osg::Vec3(+1, -1,  1));
//    // top back right
//    vertices->push_back(osg::Vec3(+1, +1,  1));
//    vertices->push_back(osg::Vec3(+1, +1,  1));
//    vertices->push_back(osg::Vec3(+1, +1,  1));
//    // top back left
//    vertices->push_back(osg::Vec3(-1, +1,  1));
//    vertices->push_back(osg::Vec3(-1, +1,  1));
//    vertices->push_back(osg::Vec3(-1, +1,  1));


//    // face array
//    osg::DrawElementsUInt *faceArray = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);

//    // bottom
//    faceArray->push_back(0); // face 1
//    faceArray->push_back(9);
//    faceArray->push_back(3);
//    faceArray->push_back(9); // face 2
//    faceArray->push_back(6);
//    faceArray->push_back(3);
//    // top
//    faceArray->push_back(21);  //face 3
//    faceArray->push_back(12);
//    faceArray->push_back(18);
//    faceArray->push_back(12);  //face 4
//    faceArray->push_back(15);
//    faceArray->push_back(18);
//    // left
//    faceArray->push_back(22);  //face 5
//    faceArray->push_back(10);
//    faceArray->push_back(13);
//    faceArray->push_back(10);  //face 6
//    faceArray->push_back(1);
//    faceArray->push_back(13);
//    // right
//    faceArray->push_back(16);  //face 7
//    faceArray->push_back(4);
//    faceArray->push_back(19);
//    faceArray->push_back(4);  //face 8
//    faceArray->push_back(7);
//    faceArray->push_back(19);
//    // front
//    faceArray->push_back(14);  //face 9
//    faceArray->push_back(2);
//    faceArray->push_back(17);
//    faceArray->push_back(2);   //face 10
//    faceArray->push_back(5);
//    faceArray->push_back(17);
//    // back
//    faceArray->push_back(20);  //face 11
//    faceArray->push_back(8);
//    faceArray->push_back(23);
//    faceArray->push_back(8);   //face 12
//    faceArray->push_back(11);
//    faceArray->push_back(23);

//    primitiveSets->push_back(faceArray);

//    // normal array
//    normals->push_back(osg::Vec3(+1, 0, 0));
//    normals->push_back(osg::Vec3(-1, 0, 0));
//    normals->push_back(osg::Vec3(0, +1, 0));
//    normals->push_back(osg::Vec3(0, -1, 0));
//    normals->push_back(osg::Vec3(0, 0, +1));
//    normals->push_back(osg::Vec3(0, 0, -1));

//    // normal index
//    osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 24, 4> *normalIndexArray;
//    normalIndexArray = new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 24, 4>();

//    // bottom front left
//    normalIndexArray->push_back(5);
//    normalIndexArray->push_back(3);
//    normalIndexArray->push_back(0);
//    // bottom front right
//    normalIndexArray->push_back(5);
//    normalIndexArray->push_back(2);
//    normalIndexArray->push_back(0);
//    // bottom back right
//    normalIndexArray->push_back(5);
//    normalIndexArray->push_back(2);
//    normalIndexArray->push_back(1);
//    // bottom back left
//    normalIndexArray->push_back(5);
//    normalIndexArray->push_back(3);
//    normalIndexArray->push_back(1);

//    // top front left
//    normalIndexArray->push_back(4);
//    normalIndexArray->push_back(3);
//    normalIndexArray->push_back(0);
//    // top front right
//    normalIndexArray->push_back(4);
//    normalIndexArray->push_back(2);
//    normalIndexArray->push_back(0);
//    // top back right
//    normalIndexArray->push_back(4);
//    normalIndexArray->push_back(2);
//    normalIndexArray->push_back(1);
//    // top back left
//    normalIndexArray->push_back(4);
//    normalIndexArray->push_back(3);
//    normalIndexArray->push_back(1);

//    geometry->setVertexArray(vertices);

//    geometry->setNormalArray(normals);
//    geometry->setNormalIndices(normalIndexArray);
//    geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
//    geometry->addPrimitiveSet(faceArray);

//    osg::Vec4Array* colors = new osg::Vec4Array;
//    colors->push_back(color);
//    geometry->setColorArray(colors);
//    geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
//}

void Clustering::Cube::computeGeode()
{
	// define normals for each face
	osg::ref_ptr<osg::Vec3Array> cube_normals = new osg::Vec3Array;
	cube_normals->push_back( osg::Vec3( 0.0f,-1.0f, 0.0f ) );
	cube_normals->push_back( osg::Vec3( 0.0f,-1.0f, 0.0f ) );
	cube_normals->push_back( osg::Vec3( 0.0f,-1.0f, 0.0f ) );
	cube_normals->push_back( osg::Vec3( 0.0f,-1.0f, 0.0f ) );

	cube_normals->push_back( osg::Vec3( 0.0f,+1.0f, 0.0f ) );
	cube_normals->push_back( osg::Vec3( 0.0f,+1.0f, 0.0f ) );
	cube_normals->push_back( osg::Vec3( 0.0f,+1.0f, 0.0f ) );
	cube_normals->push_back( osg::Vec3( 0.0f,+1.0f, 0.0f ) );

	cube_normals->push_back( osg::Vec3( +1.0f,+0.0f, 0.0f ) );
	cube_normals->push_back( osg::Vec3( +1.0f,+0.0f, 0.0f ) );
	cube_normals->push_back( osg::Vec3( +1.0f,+0.0f, 0.0f ) );
	cube_normals->push_back( osg::Vec3( +1.0f,+0.0f, 0.0f ) );

	cube_normals->push_back( osg::Vec3( -1.0f,+0.0f, 0.0f ) );
	cube_normals->push_back( osg::Vec3( -1.0f,+0.0f, 0.0f ) );
	cube_normals->push_back( osg::Vec3( -1.0f,+0.0f, 0.0f ) );
	cube_normals->push_back( osg::Vec3( -1.0f,+0.0f, 0.0f ) );

	cube_normals->push_back( osg::Vec3( 0.0f,+0.0f,-1.0f ) );
	cube_normals->push_back( osg::Vec3( 0.0f,+0.0f,-1.0f ) );
	cube_normals->push_back( osg::Vec3( 0.0f,+0.0f,-1.0f ) );
	cube_normals->push_back( osg::Vec3( 0.0f,+0.0f,-1.0f ) );

	cube_normals->push_back( osg::Vec3( 0.0f,+0.0f,+1.0f ) );
	cube_normals->push_back( osg::Vec3( 0.0f,+0.0f,+1.0f ) );
	cube_normals->push_back( osg::Vec3( 0.0f,+0.0f,+1.0f ) );
	cube_normals->push_back( osg::Vec3( 0.0f,+0.0f,+1.0f ) );

	osg::Vec3 vPosition = osg::Vec3( 0, 0, 0 );

	float fRadius = 1.0f;

	// note, counterclockwise winding order with respect to normals
	osg::Vec3 myCoords[] = {
		// bottom face
		osg::Vec3( -fRadius, -fRadius, -fRadius ) + vPosition,
		osg::Vec3( +fRadius, -fRadius, -fRadius ) + vPosition,
		osg::Vec3( +fRadius, -fRadius, +fRadius ) + vPosition,
		osg::Vec3( -fRadius, -fRadius, +fRadius ) + vPosition,

		// top face
		osg::Vec3( -fRadius, +fRadius, +fRadius ) + vPosition,
		osg::Vec3( +fRadius, +fRadius, +fRadius ) + vPosition,
		osg::Vec3( +fRadius, +fRadius, -fRadius ) + vPosition,
		osg::Vec3( -fRadius, +fRadius, -fRadius ) + vPosition,

		// right face
		osg::Vec3( +fRadius, -fRadius, -fRadius ) + vPosition,
		osg::Vec3( +fRadius, +fRadius, -fRadius ) + vPosition,
		osg::Vec3( +fRadius, +fRadius, +fRadius ) + vPosition,
		osg::Vec3( +fRadius, -fRadius, +fRadius ) + vPosition,

		// left face
		osg::Vec3( -fRadius, -fRadius, +fRadius ) + vPosition,
		osg::Vec3( -fRadius, +fRadius, +fRadius ) + vPosition,
		osg::Vec3( -fRadius, +fRadius, -fRadius ) + vPosition,
		osg::Vec3( -fRadius, -fRadius, -fRadius ) + vPosition,

		// front face
		osg::Vec3( -fRadius, +fRadius, -fRadius ) + vPosition,
		osg::Vec3( +fRadius, +fRadius, -fRadius ) + vPosition,
		osg::Vec3( +fRadius, -fRadius, -fRadius ) + vPosition,
		osg::Vec3( -fRadius, -fRadius, -fRadius ) + vPosition,

		// back face
		osg::Vec3( -fRadius, -fRadius, +fRadius ) + vPosition,
		osg::Vec3( +fRadius, -fRadius, +fRadius ) + vPosition,
		osg::Vec3( +fRadius, +fRadius, +fRadius ) + vPosition,
		osg::Vec3( -fRadius, +fRadius, +fRadius ) + vPosition
	};

	int numCoords = static_cast<int>( sizeof( myCoords )/sizeof( osg::Vec3 ) );
	osg::Vec3Array* vertices = new osg::Vec3Array( static_cast<unsigned int>( numCoords ), myCoords );
	geometry->setVertexArray( vertices );

	geometry->addPrimitiveSet( new
							   osg::DrawArrays( osg::PrimitiveSet::QUADS,0,numCoords ) );

	geometry->setNormalArray( cube_normals.get() );
	geometry->setNormalBinding( osg::Geometry::BIND_PER_VERTEX );

	geode->addDrawable( geometry );
}

void Clustering::Cube::transform( osg::Vec3d position, osg::Vec3d scale, osg::Vec4d color )
{
	osg::StateSet* ss = geometry->getOrCreateStateSet();
	/*
	// only wireframe (outline / contour)
		osg::ref_ptr<osg::PolygonMode> pm = new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
		ss->setAttributeAndModes(pm.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	// line width
		osg::LineWidth* linewidth = new osg::LineWidth();
		linewidth->setWidth(20.0f);
		ss->setAttributeAndModes(linewidth, osg::StateAttribute::ON);
	*/
// transparent

//    ss->setMode( GL_BLEND, osg::StateAttribute::ON );
//    ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	osg::Material* material = new osg::Material();
	material->setAmbient( osg::Material::FRONT,color );
	material->setDiffuse( osg::Material::FRONT,color );
	material->setSpecular( osg::Material::FRONT,color );
	material->setAlpha( osg::Material::FRONT,static_cast<float>( color.a() ) );

	ss->setAttribute( material,osg::StateAttribute::OVERRIDE );
	ss->setMode( GL_DEPTH_TEST,osg::StateAttribute::ON );
	ss->setMode( GL_LIGHTING,osg::StateAttribute::OFF );
	ss->setMode( GL_BLEND,osg::StateAttribute::ON );
	ss->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask( false );
	ss->setAttributeAndModes( depth, osg::StateAttribute::ON );
	ss->setAttributeAndModes( new osg::BlendFunc, osg::StateAttribute::ON );

	at->setPosition( position * 1 );
	at->setScale( scale );

//    dodecahedronGeode->setUserValue("id", QString::number(id).toStdString());

}
