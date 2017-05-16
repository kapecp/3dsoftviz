#include "Viewer/PerlinNoiseTextureGenerator.h"

#include "Viewer/DataHelper.h"

#include <osg/Image>

#include <noiseutils.h>
#include <noise.h>

#include <iostream>

namespace Vwr {


osg::ref_ptr<osg::Texture2D> PerlinNoiseTextureGenerator::getCoudTexture( int w, int h,unsigned char r, unsigned char g, unsigned char b, unsigned char alpha ) //w and h speak for themselves, zoom wel zoom in and out on it, I usually  use 75. P stands for persistence, this controls the roughness of the picture, i use 1/2
{
	noise::module::Perlin perlinModule;

	noise::utils::NoiseMap heightMap;
	noise::utils::NoiseMapBuilderSphere heightMapBuilder;

	heightMapBuilder.SetSourceModule( perlinModule );
	heightMapBuilder.SetDestNoiseMap( heightMap );
	heightMapBuilder.SetDestSize( w, h );
	heightMapBuilder.SetBounds( -90.0, 90.0, -180.0, 180.0 );

	heightMapBuilder.Build();

	noise::utils::RendererImage renderer;
	noise::utils::Image image;

	renderer.SetSourceNoiseMap( heightMap );
	renderer.SetDestImage( image );

	renderer.ClearGradient();
	renderer.AddGradientPoint( -1.0000, noise::utils::Color( r, g, b, alpha ) );
	renderer.AddGradientPoint( 1.0000, noise::utils::Color( 0, 0, 0, 255 ) );

	renderer.Render();

	unsigned char* data = new unsigned char[w * h * 3];

	int index = 0;

	for ( int y = 0; y < h; y++ ) {
		for ( int x = 0; x < w; x++ ) {
			data[index++] = image.GetValue( x, y ).red;
			data[index++] = image.GetValue( x, y ).green;
			data[index++] = image.GetValue( x, y ).blue;
		}
	}

	osg::ref_ptr<osg::Image> i = new( std::nothrow ) osg::Image;
	i->setImage( w, h, 1,  GL_RGB, GL_RGB, GL_UNSIGNED_BYTE, data, osg::Image::USE_NEW_DELETE );

	osg::ref_ptr<osg::Texture2D> tex = DataHelper::createTexture( i );

	return tex;
}

} // namespace Vwr
