#include "Manager/ResourceManager.h"
#include <osgDB/ReadFile>
#include <Viewer/DataHelper.h>
#include <osg/Material>

namespace Manager {
static ResourceManager* instance = nullptr;

#include <leathers/push>
#include <leathers/exit-time-destructors>
#include <leathers/global-constructors>
static QMap<QString, osg::ref_ptr<osg::Node>> meshes;
static QMap<QString, osg::ref_ptr<osg::Geode>> shapes;
static QMap<QString, osg::ref_ptr<osg::Texture>> textures;
static QMap<osg::Vec3, osg::ref_ptr<osg::Material>> materials;
#include <leathers/pop>

ResourceManager::ResourceManager()
{
}

ResourceManager* ResourceManager::getInstance()
{
	if ( !instance ) {
		instance = new ResourceManager();
	}
	return instance;
}

osg::ref_ptr<osg::Node> ResourceManager::getMesh( const QString& path )
{
	osg::ref_ptr<osg::Node> n = meshes.value( path );
	if ( !n.valid() ) {
		n = osgDB::readNodeFile( path.toStdString() );
		meshes.insert( path, n );
	}
	return n;
}

osg::ref_ptr<osg::Geode> ResourceManager::getShape( const QString& params, GeodeCreator creator )
{
	osg::ref_ptr<osg::Geode> s = shapes.value( params );
	if ( !s.valid() ) {
		s = creator( params );
		shapes.insert( params, s );
	}
	return s;
}

osg::ref_ptr<osg::Texture> ResourceManager::getTexture( const QString& path )
{
	osg::ref_ptr<osg::Texture> t = textures.value( path );
	if ( !t.valid() ) {
		t = Vwr::DataHelper::readTextureFromFile( path );
		textures.insert( path, t );
	}
	return t;
}

osg::ref_ptr<osg::Material> ResourceManager::getMaterial( const osg::Vec3& color )
{
	osg::ref_ptr<osg::Material> m = materials.value( color );
	if ( !m.valid() ) {
		m = new osg::Material();
		m->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4( color.x(), color.y(), color.z(), 1.0f ) );
		materials.insert( color, m );
	}
	return m;
}
}
