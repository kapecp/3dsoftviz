#pragma once

#include <osg/Geometry>
#include <QString>
#include <QtCore/QMap>
#include <functional>
#include <osg/Material>

namespace Manager {
/**
 * \class ResourceManager
 * \brief Manager provide meshes, textures and other resources
 *
 * Class is implemented as singleton.
 *
 * \author Stefan Horvath
 * \date 26.01.2016
 */
class ResourceManager
{
public:
	typedef std::function<osg::Geode*( const QString& )> GeodeCreator;

private:
	ResourceManager();

public:
	static ResourceManager* getInstance();
	osg::ref_ptr<osg::Node> getMesh( const QString& path );
	osg::ref_ptr<osg::Geode> getShape( const QString& params, GeodeCreator creator );
	osg::ref_ptr<osg::Texture> getTexture( const QString& path );
	osg::ref_ptr<osg::Material> getMaterial( const osg::Vec3& color );
};
}
