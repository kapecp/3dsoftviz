/**
*  TextureWrapper.h
*  Projekt 3DVisual
*/
#ifndef VIEWER_TEXTURE_WRAPPER_DEF
#define VIEWER_TEXTURE_WRAPPER_DEF 1

#include <osg/ImageSequence>
#include <osg/Texture2D>
#include <osg/ref_ptr>
#include <osgDB/ReadFile>
#include <sstream>

#include "Viewer/DataHelper.h"

#include "Util/ApplicationConfig.h"

namespace Vwr
{
	/**
	*  \class TextureWrapper
	*  \brief Texture operations wrapper
	*  \author Michal Paprcka 
	*  \date 29. 4. 2010
	*/
	class TextureWrapper
	{
	public:


		/**
		*  \fn public static  getNodeTexture
		*  \brief Returns default node texture
		*  \return osg::ref_ptr node texture
		*/
		static osg::ref_ptr<osg::Texture2D> getNodeTexture();


		/**
		*  \fn public static  getOrientedEdgeTexture
		*  \brief Returns default oriented edge texture
		*  \return osg::ref_ptr<osg::Texture2D> oriented edge texture
		*/
		static osg::ref_ptr<osg::Texture2D> getOrientedEdgeTexture();


		/**
		*  \fn public static  getEdgeTexture
		*  \brief Returns default edge texture
		*  \return osg::ref_ptr edge texture
		*/
		static osg::ref_ptr<osg::Texture2D> getEdgeTexture();


		/**
		*  \fn public static  reloadTextures
		*  \brief Reloads all textures
		*/
		static void reloadTextures();

	private:

		/**
		*  osg::ref_ptr nodeTexture
		*  \brief default node texture
		*/
		static osg::ref_ptr<osg::Texture2D> nodeTexture;


		/**
		*  osg::ref_ptr<osg::Texture2D> edgeTexture
		*  \brief default edge texture
		*/
		static osg::ref_ptr<osg::Texture2D> edgeTexture;


		/**
		*  osg::ref_ptr orientedEdgeTexture
		*  \brief default oriented edge texture
		*/
		static osg::ref_ptr<osg::Texture2D> orientedEdgeTexture;

	};
}

#endif