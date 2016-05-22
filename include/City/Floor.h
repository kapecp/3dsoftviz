#pragma once

#include <osg/PositionAttitudeTransform>

namespace Clustering {
class Floor : public osg::PositionAttitudeTransform
{
private:
	float baseSize;
	float floorHeight;
	bool divideBorder;

public:
	Floor();
	void setBaseSize( float size );
	float getBaseSize() const;
	void setFloorHeight( float height );
	float getFloorHeight() const;
	void setDivideBorder( bool state );
	void refresh();
	static float getFloorMinHeight();
};
}
