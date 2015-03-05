/**
*  PerlinNoiseTextureGenerator.h
*  Projekt 3DVisual
*/
#ifndef VIEWER_PERLIN_NOISE_GENERATOR_DEF
#define VIEWER_PERLIN_NOISE_GENERATOR_DEF 1

#include <osg/Texture2D>

namespace Vwr {
/**
	*  \class PerlinNoiseTextureGenerator
	*  \brief Generates textures form perlin noise
	*  \author Michal Paprcka
	*  \date 29. 4. 2010
	*/
class PerlinNoiseTextureGenerator
{
public:

	/**
		*  \fn public static  getCoudTexture(int w, int h, int r, int g, int b, int alpha)
		*  \brief Generates perlin noise with given parameters and creates a texture from it
		*  \param        w     texture width
		*  \param        h     texture height
		*  \param        r     red amount
		*  \param        g     green amount
		*  \param        b     blue amount
		*  \param        alpha    alpha amount
		*  \return osg::ref_ptr texture
		*/

	static osg::ref_ptr<osg::Texture2D> getCoudTexture( int w, int h, unsigned char r, unsigned char g, unsigned char b, unsigned char alpha );
};
}

#endif
