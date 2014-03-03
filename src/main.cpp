#include <QApplication>

#include "Manager/Manager.h"
#include "Core/Core.h"
#include "Util/Cleaner.h"

#ifdef OPENCV_FOUND
    #include "OpenCV/OpenCVCore.h"
#endif

// lua development
extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

// include diluculum header files
#include "Diluculum/CppObject.hpp"
#include "Diluculum/LuaExceptions.hpp"
#include "Diluculum/LuaFunction.hpp"
#include "Diluculum/LuaState.hpp"
#include "Diluculum/LuaUserData.hpp"
#include "Diluculum/LuaUtils.hpp"
#include "Diluculum/LuaValue.hpp"
#include "Diluculum/LuaVariable.hpp"
#include "Diluculum/Types.hpp"

static Diluculum::LuaState* luaState;

int main(int argc, char *argv[])
{
	luaState = new Diluculum::LuaState;
	luaState->doFile("../share/3dsoftviz/scripts/main.lua");

	QApplication app(argc, argv);
	new Cleaner(&app);
	AppCore::Core::getInstance(&app);
	Manager::GraphManager::getInstance();

#ifdef OPENCV_FOUND
	OpenCV::OpenCVCore::getInstance(&app);
#endif


}
