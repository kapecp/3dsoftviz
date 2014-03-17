#include <QApplication>

#include "Manager/Manager.h"
#include "Core/Core.h"
#include "Util/Cleaner.h"

#ifdef OPENCV_FOUND
    #include "OpenCV/OpenCVCore.h"
#endif

#include "LuaInterface/LuaInterface.h"
#include "LuaGraph/LuaNode.h"
#include "LuaGraph/LuaGraph.h"

#include "iostream"

int main(int argc, char *argv[])
{

    Lua::LuaGraph* g = Lua::LuaGraph::loadGraph();
    delete g;

	QApplication app(argc, argv);
	new Cleaner(&app);
	AppCore::Core::getInstance(&app);
	Manager::GraphManager::getInstance();

#ifdef OPENCV_FOUND
	OpenCV::OpenCVCore::getInstance(&app);
#endif


}
