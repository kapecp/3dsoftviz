/* -*-c++-*- osgModeling - Copyright (C) 2008 Wang Rui <wangray84@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.

* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.

* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef OSGMODELING_MODEL
#define OSGMODELING_MODEL 1

#include <iostream>
#include <osg/CopyOp>
#include <osg/Geometry>
#include "Math/Curve.h"

namespace osgModeling {

/** Modeling base class
 * This is the base class of all osgModeling models.
 */
class OSGMODELING_EXPORT Model : public osg::Geometry
{
public:
	enum GenerateParts { CAP1_PART=0x1, BODY_PART=0x2, CAP2_PART=0x4, ALL_PARTS=CAP1_PART|BODY_PART|CAP2_PART };
	enum GenerateCoords { NORMAL_COORDS=0x1, TEX_COORDS=0x2, ALL_COORDS=NORMAL_COORDS|TEX_COORDS };
	enum AuxFunctions { FLIP_NORMAL=0x1, USE_WIREFRAME=0x2 };

	Model():
		osg::Geometry(),
		_updated( false ), _partsToGenerate( BODY_PART ), _coordsToGenerate( ALL_COORDS ), _funcs( 0 )
	{
	}

	Model( const osg::Geometry& copy, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY ):
		osg::Geometry( copy,copyop ),
		_updated( true ),
		_partsToGenerate( 0 ), _coordsToGenerate(0), _funcs( 0 )
	{
	}

	Model( const Model& copy, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY ):
		osg::Geometry( copy,copyop ),
		_updated( copy._updated ), _partsToGenerate( copy._partsToGenerate ), _coordsToGenerate( copy._coordsToGenerate ),
		_funcs( copy._funcs )
	{
	}

	META_Object( osgModeling, Model );

	/** Set which part of the model should be generated. Use 'OR' operation to select from enum GenerateParts. */
	inline void setGenerateParts( int gp=ALL_PARTS )
	{
		if ( _partsToGenerate!=gp ) {
			_updated = false;
			_partsToGenerate = gp;
		}
	}
	inline int getGenerateParts() const
	{
		return _partsToGenerate;
	}

	/** Set whether to generate normal/texture coords from enum GenerateCoords. */
	inline void setGenerateCoords( int gc=ALL_COORDS )
	{
		if ( _coordsToGenerate!=gc ) {
			_updated = false;
			_coordsToGenerate = gc;
		}
	}
	inline int getGenerateCoords() const
	{
		return _coordsToGenerate;
	}

	virtual void updateImplementation() {}

	virtual void drawImplementation( osg::RenderInfo& renderInfo ) const
	{
		if ( !_updated ) {
			osg::notify( osg::WARN ) << "osgModeling::" << className() << ": Call update() to update changed models." <<std::endl;
		}

		osg::Geometry::drawImplementation( renderInfo );
	}

protected:
	virtual ~Model() {}

	bool _updated;

	int _partsToGenerate;
	int _coordsToGenerate;
	int _funcs;

};

}

#endif
