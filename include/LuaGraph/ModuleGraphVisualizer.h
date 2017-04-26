#include "LuaGraphVisualizer.h"

#include <QSet>
#include <osg/Material>

#include "City/Module.h"
#include "City/Building.h"

namespace Lua {


class ModuleGraphVisualizer : public Lua::LuaGraphVisualizer
{

public:

	ModuleGraphVisualizer( Data::Graph* graph, osg::ref_ptr<osg::Camera> camera = 0 );
	QSet<qlonglong> nodeIdsToDelete;
	void visualize();

	void onUpdate();

	void deleteNonModuleNodes();

private:
	void deleteReturnsNode();

	void reloadGraph();

	void adjustBuildingForNode(osg::ref_ptr<Data::Node> node, float baseSize, float height, bool lieOnGround, osg::ref_ptr<osg::Material> colour, QString textureName);

};

}


