#pragma once

#include <osg/PositionAttitudeTransform>
#include "Building.h"
#include <QtCore/QList>
#include <functional>

namespace Clustering {
class Residence : public osg::PositionAttitudeTransform
{
private:
	QList<osg::ref_ptr<Building>> attributesBuildings;
	osg::ref_ptr<osg::PositionAttitudeTransform> attributesBuildingsNode;
	QList<osg::ref_ptr<Building>> gettersSettersBuildings;
	osg::ref_ptr<osg::PositionAttitudeTransform> gettersSettersBuildingsNode;
	QList<osg::ref_ptr<Building>> internalMethodsBuildings;
	osg::ref_ptr<osg::PositionAttitudeTransform> internalMethodsBuildingsNode;
	QList<osg::ref_ptr<Building>> interfaceMethodsBuildings;
	osg::ref_ptr<osg::PositionAttitudeTransform> interfaceMethodsBuildingsNode;

private:
	void forEachBuilding( std::function<void( Building& b )> func );

public:
	Residence();
	void addAttributeBuilding( Building* attrBuilding );
	void addGetterSeterBuilding( Building* getSetBuilding );
	void addInternalBuilding( Building* internalBuilding );
	void addInterfaceBuilding( Building* interfaceBuilding );
	void showLabels( bool state );
	void selectAll( bool state );
	void refresh();
};
}
