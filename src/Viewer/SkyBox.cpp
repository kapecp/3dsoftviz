// zdroj: http://anidam.sourceforge.net/doxygen/skyBox_8cpp-source.html

/***************************************************************************
*   Copyright (C) 2007 by                                                 *
*         Pierre-yves JEZEQUEL, Julien MICHOT, Loic MOISAN,               *
*         Julien PAPILLON, Sebastien PINEAUD, Barthelemy SERRES           *
*                                                                         *
*   https://sourceforge.net/projects/anidam                               *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include "Viewer/SkyBox.h"

#include "Util/ApplicationConfig.h"

#include <QDebug>

namespace Vwr {

SkyBox::SkyBox()
{
	//SkyBox::readCubeMap();
}
SkyBox::~SkyBox()
{

}

osg::TextureCubeMap* SkyBox::readCubeMap( int cubeType )
{
	osg::TextureCubeMap* cubemap = new osg::TextureCubeMap;
	try {
		Util::ApplicationConfig* appConf = Util::ApplicationConfig::get();
		osg::Image* imageEast = nullptr;
		osg::Image* imageWest = nullptr;
		osg::Image* imageSouth = nullptr;
		osg::Image* imageNorth = nullptr;
		osg::Image* imageUp = nullptr;
		osg::Image* imageDown = nullptr;

		if ( cubeType == -1 ) { // Black skybox - black cube map
			imageEast = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.Black" ).toStdString() );
			imageWest = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.Black" ).toStdString() );
			imageNorth = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.Black" ).toStdString() );
			imageSouth = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.Black" ).toStdString() );
			imageUp = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.Black" ).toStdString() );
			imageDown = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.Black" ).toStdString() );
		}
		else if ( cubeType == -2 ) { // White skybox - white cube map
			imageEast = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.White" ).toStdString() );
			imageWest = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.White" ).toStdString() );
			imageNorth = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.White" ).toStdString() );
			imageSouth = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.White" ).toStdString() );
			imageUp = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.White" ).toStdString() );
			imageDown = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.White" ).toStdString() );
		}
		else { // Default skybox
			imageEast = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.East" ).toStdString() );
			imageWest = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.West" ).toStdString() );
			imageNorth = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.North" ).toStdString() );
			imageSouth = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.South" ).toStdString() );
			imageUp = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.Up" ).toStdString() );
			imageDown = osgDB::readImageFile( appConf->getValue( "Viewer.SkyBox.Down" ).toStdString() );
		}

		if ( imageWest && imageEast && imageDown && imageUp && imageSouth && imageNorth ) {
			cubemap->setImage( osg::TextureCubeMap::POSITIVE_X, imageWest );
			cubemap->setImage( osg::TextureCubeMap::NEGATIVE_X, imageEast );
			cubemap->setImage( osg::TextureCubeMap::POSITIVE_Y, imageDown );
			cubemap->setImage( osg::TextureCubeMap::NEGATIVE_Y, imageUp );
			cubemap->setImage( osg::TextureCubeMap::POSITIVE_Z, imageSouth );
			cubemap->setImage( osg::TextureCubeMap::NEGATIVE_Z, imageNorth );

			cubemap->setWrap( osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE );
			cubemap->setWrap( osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE );
			cubemap->setWrap( osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE );

			cubemap->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
			cubemap->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
		}

	}
	catch ( std::exception& e ) {
		qDebug() << "Skybox error" << e.what();
	}
	return cubemap;
}


// Update texture matrix for cubemaps



class MoveEarthySkyWithEyePointTransform : public osg::Transform
{
public:
//src/Viewer/SkyBox.cpp:111:  Is this a non-const reference? If so, make const or use a pointer: osg::Matrix& matrix  [runtime/references] [2]
	virtual bool computeLocalToWorldMatrix( osg::Matrix& matrix,osg::NodeVisitor* nv ) const
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>( nv );
		if ( cv ) {
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.preMult( osg::Matrix::translate( eyePointLocal ) );
		}
		return true;
	}

	virtual bool computeWorldToLocalMatrix( osg::Matrix& matrix,osg::NodeVisitor* nv ) const
	{
		osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>( nv );
		if ( cv ) {
			osg::Vec3 eyePointLocal = cv->getEyeLocal();
			matrix.postMult( osg::Matrix::translate( -eyePointLocal ) );
		}
		return true;
	}
};


osg::Node* SkyBox::createSkyBox( int skyboxType )
{

	osg::StateSet* stateset = new osg::StateSet();

	osg::TexEnv* te = new osg::TexEnv;
	te->setMode( osg::TexEnv::REPLACE );
	stateset->setTextureAttributeAndModes( 0, te, osg::StateAttribute::ON );

	osg::TexGen* tg = new osg::TexGen;
	tg->setMode( osg::TexGen::NORMAL_MAP );
	stateset->setTextureAttributeAndModes( 0, tg, osg::StateAttribute::ON );

	osg::TexMat* tm = new osg::TexMat;
	stateset->setTextureAttribute( 0, tm );

	osg::TextureCubeMap* skymap = readCubeMap( skyboxType );
	stateset->setTextureAttributeAndModes( 0, skymap, osg::StateAttribute::ON );

	stateset->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
	stateset->setMode( GL_CULL_FACE, osg::StateAttribute::OFF );

	// clear the depth to the far plane.
	osg::Depth* depth = new osg::Depth;
	depth->setFunction( osg::Depth::ALWAYS );
	depth->setRange( 1.0,1.0 );
	stateset->setAttributeAndModes( depth, osg::StateAttribute::ON );

	stateset->setRenderBinDetails( 6,"RenderBin" );

	osg::Drawable* drawable = new osg::ShapeDrawable( new osg::Sphere( osg::Vec3( 0.0f,0.0f,0.0f ),100 ) );

	osg::Geode* geode = new osg::Geode;
	geode->setCullingActive( false );
	geode->setStateSet( stateset );
	geode->addDrawable( drawable );


	osg::Transform* transform = new MoveEarthySkyWithEyePointTransform;
	transform->setCullingActive( false );
	transform->addChild( geode );


	osg::ClearNode* clearNode = new osg::ClearNode;
	//  clearNode->setRequiresClear(false);
	clearNode->setCullCallback( new TexMatCallback( *tm ) );
	clearNode->addChild( transform );

	return clearNode;
}

} // namespace Vwr
