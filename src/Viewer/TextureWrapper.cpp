#include "Viewer/TextureWrapper.h"

#include "Viewer/DataHelper.h"
#include "Util/ApplicationConfig.h"

#include <osg/ImageSequence>
#include <osgDB/ReadFile>

#include <sstream>

namespace Vwr {

#include <leathers/push>
#include <leathers/exit-time-destructors>
#include <leathers/global-constructors>

osg::ref_ptr<osg::Texture2D> TextureWrapper::nodeTexture;
osg::ref_ptr<osg::Texture2D> TextureWrapper::edgeTexture;
osg::ref_ptr<osg::Texture2D> TextureWrapper::orientedEdgeTexture;

#include <leathers/pop>

osg::ref_ptr<osg::Texture2D> TextureWrapper::getNodeTexture()
{
	if ( nodeTexture == NULL ) {
		nodeTexture = DataHelper::readTextureFromFile( Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
	}
	return nodeTexture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::getOrientedEdgeTexture()
{
	// Gloger: changed edge & oriented edge textures
	// This is only for thesis images prettyfying. Do not merge or reuse at will
	if ( orientedEdgeTexture == NULL ) {
		//orientedEdgeTexture = DataHelper::readTextureFromFile( Util::ApplicationConfig::get()->getValue( "Viewer.Textures.OrientedEdge" ) );
	}
	//return orientedEdgeTexture;

	if ( orientedEdgeTexture == NULL ) {
		osg::ref_ptr<osg::ImageSequence> imageSequence = new osg::ImageSequence;
		imageSequence->setMode( osg::ImageSequence::PRE_LOAD_ALL_IMAGES );

		Util::ApplicationConfig* appConf = Util::ApplicationConfig::get();

		for ( int x = 0; x < 15; x++ ) {
			std::stringstream os;
			os << appConf->getValue( "Viewer.Textures.OrientedEdgePrefix" ).toStdString() << x << appConf->getValue( "Viewer.Textures.OrientedEdgeSuffix" ).toStdString();

			osg::ref_ptr<osg::Image> image = osgDB::readImageFile( os.str() );
			if ( image.valid() ) {
				imageSequence->addImage( image.get() );
			}
		}

		imageSequence->setLength( 0.4 );
		imageSequence->setLoopingMode( osg::ImageStream::LOOPING );
		imageSequence->play();

		orientedEdgeTexture = new osg::Texture2D;
		orientedEdgeTexture->setImage( imageSequence.get() );

		orientedEdgeTexture->setDataVariance( osg::Object::DYNAMIC );
		orientedEdgeTexture->setWrap( osg::Texture::WRAP_S, osg::Texture::REPEAT );
		orientedEdgeTexture->setWrap( osg::Texture::WRAP_T, osg::Texture::REPEAT );
	}

	return orientedEdgeTexture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::getEdgeTexture()
{
	if ( edgeTexture == NULL ) {
		edgeTexture = DataHelper::readTextureFromFile( Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );
	}
	return edgeTexture;
}

void TextureWrapper::reloadTextures()
{
	nodeTexture = NULL;
	edgeTexture = NULL;
	orientedEdgeTexture = NULL;
}

} // namespace Vwr
