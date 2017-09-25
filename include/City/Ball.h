#pragma once

#include <osg/PositionAttitudeTransform>
#include "Floor.h"
#include <QList>
#include <QString>
#include <osg/Material>
#include <osg/BoundingBox>

namespace City {
class Ball : public osg::PositionAttitudeTransform
{
private:
	QString name;
	float minBaseSize;
	float baseSize;
	bool lieOnGround;
	osg::ref_ptr<osg::Geode> label;
	bool labelVisible;
	osg::ref_ptr<osg::Material> lastMaterial;

public:
	Ball( const QString& name = QString() );
	void setBaseSize( float size );
	float getBaseSize() const;
	void setLieOnGround( bool state );
	void setLabel( const QString& name );
	void showLabel( bool state );
	osg::BoundingBox getBoundingBox() const;
	void refresh();
	void select( bool selected );
	float getMinBaseSize() const;
};
}
