//zdroj: http://anidam.sourceforge.net/doxygen/skyBox_8h-source.html

#ifndef VIEWER_SKY_BOX_DEF
#define VIEWER_SKY_BOX_DEF 1

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
#include <QObject>

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Quat>
#include <osg/Matrix>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Transform>
#include <osg/Material>
#include <osg/NodeCallback>
#include <osg/Depth>
#include <osg/CullFace>
#include <osg/TexMat>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osg/TexEnvCombine>
#include <osg/TextureCubeMap>
#include <osg/VertexProgram>


#include <osgDB/Registry>
#include <osgDB/ReadFile>

#include <osgUtil/SmoothingVisitor>
#include <osgUtil/Optimizer>
#include <osgUtil/CullVisitor>

namespace Vwr
{
class SkyBox{

public:
    SkyBox();
    ~SkyBox();
    osg::Node* createSkyBox();

private:
    osg::TextureCubeMap* readCubeMap();

};
}
#endif
