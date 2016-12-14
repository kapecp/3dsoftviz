#include "LuaGraphVisualizer.h"

namespace Lua {


class ModuleGraphVisualizer : public Lua::LuaGraphVisualizer
{

public:

	ModuleGraphVisualizer( Data::Graph* graph, osg::ref_ptr<osg::Camera> camera = 0 );

	void visualize();

	void onUpdate();
};

}


