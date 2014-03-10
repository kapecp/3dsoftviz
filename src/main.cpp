#include <QApplication>

#include "Manager/Manager.h"
#include "Core/Core.h"
#include "Util/Cleaner.h"

#ifdef OPENCV_FOUND
    #include "OpenCV/OpenCVCore.h"
#endif

#include "LuaInterface/LuaInterface.h"

int main(int argc, char *argv[])
{

    Lua::LuaInterface *lua = Lua::LuaInterface::getInstance();
    lua->executeFile("../share/3dsoftviz/scripts/main.lua");
    std::string var[] = {"test", "foo"};
    std::cout << lua->getString(2, var) << " " << lua->getString("test2") << std::endl;
    int fortyTwo = lua->getInt("test3");
    std::cout << fortyTwo << std::endl;

	QApplication app(argc, argv);
	new Cleaner(&app);
	AppCore::Core::getInstance(&app);
	Manager::GraphManager::getInstance();

#ifdef OPENCV_FOUND
	OpenCV::OpenCVCore::getInstance(&app);
#endif


}
