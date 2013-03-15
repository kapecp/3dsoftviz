#include "Viewer/TextureWrapper.h"

using namespace Vwr;
using namespace std;

osg::ref_ptr<osg::Texture2D> TextureWrapper::nodeTexture;
osg::ref_ptr<osg::Texture2D> TextureWrapper::edgeTexture;
osg::ref_ptr<osg::Texture2D> TextureWrapper::orientedEdgeTexture;

osg::ref_ptr<osg::Texture2D> TextureWrapper::getNodeTexture()
{
	if(nodeTexture == NULL)
		nodeTexture = DataHelper::readTextureFromFile(Util::ApplicationConfig::get()->getValue("Viewer.Textures.Node"));
	return nodeTexture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::getOrientedEdgeTexture()
{
	if(orientedEdgeTexture == NULL)
	{
		osg::ref_ptr<osg::ImageSequence> imageSequence = new osg::ImageSequence;
		imageSequence->setMode(osg::ImageSequence::PRE_LOAD_ALL_IMAGES);

		Util::ApplicationConfig * appConf = Util::ApplicationConfig::get();

		for (int x = 0; x < 15; x++)
		{
			stringstream os;
			os << appConf->getValue("Viewer.Textures.OrientedEdgePrefix").toStdString() << x << appConf->getValue("Viewer.Textures.OrientedEdgeSuffix").toStdString();

			osg::ref_ptr<osg::Image> image = osgDB::readImageFile(os.str());
			if (image.valid())
			{
				imageSequence->addImage(image.get());
			}
		}

		imageSequence->setLength(0.4);
		imageSequence->setLoopingMode(osg::ImageStream::LOOPING);
		imageSequence->play();

		orientedEdgeTexture = new osg::Texture2D;
		orientedEdgeTexture->setImage(imageSequence.get());

		orientedEdgeTexture->setDataVariance(osg::Object::DYNAMIC);
		orientedEdgeTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
		orientedEdgeTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	}

	return orientedEdgeTexture;
}

osg::ref_ptr<osg::Texture2D> TextureWrapper::getEdgeTexture()
{
	if(edgeTexture == NULL)
		edgeTexture = DataHelper::readTextureFromFile(Util::ApplicationConfig::get()->getValue("Viewer.Textures.Edge"));
	return edgeTexture;
}

void TextureWrapper::reloadTextures()
{
	nodeTexture = NULL;
	edgeTexture = NULL;
	orientedEdgeTexture = NULL;
}