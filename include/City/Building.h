#pragma once

#include <osg/PositionAttitudeTransform>
#include "Floor.h"
#include <QList>
#include <QString>
#include <osg/Material>
#include <osg/BoundingBox>

namespace City {
class Building : public osg::PositionAttitudeTransform
{
private:
	QString name;
	QString info;
	QList<osg::ref_ptr<Floor>> floors;
	float minBuildingHeight;
	float buildingHeight;
	bool triangleRoof;
	bool lieOnGround;
	osg::ref_ptr<osg::Geode> label;
	bool labelVisible;
	osg::ref_ptr<osg::Material> lastMaterial;

public:
	Building( const QString& name = QString(), const QString& info = QString(), const QList<Floor*>& inputFloors = QList<Floor*>() );
	void setHeight( float height );
	float getHeight( bool includeRoof = false ) const;
	void setBaseSize( float size );
	float getBaseSize() const;
	void setTriangleRoof( bool state );
	void setLieOnGround( bool state );
	void setLabel( const QString& name );
	void showLabel( bool state );
	const QString& getInfo() const;
	osg::BoundingBox getBoundingBox() const;
	void refresh();
	void select( bool selected );
	float getMinHeight() const;
};
}
