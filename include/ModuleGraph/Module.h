#pragma once

#include <osg/PositionAttitudeTransform>

namespace ModuleGraph {
/**
	*  \class Module
	*  \brief
	*  \author Denis Illes
	*  \date 9. 11. 2016
	*/

class Module : public osg::PositionAttitudeTransform
{

public:

	Module();
	float getLength();
	float getWidth();
	float getHeigth();
	void setLength( float newLength );
	void setWidth( float newWidth );
	void setHeigth( float newHeigth );
	void setModuleSize( float newLength, float newWidth, float newHeigth );
	void refresh();

private:

	float length;
	float width;
	float heigth;

};
}
