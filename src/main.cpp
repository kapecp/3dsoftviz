#include <QApplication>

#include "Manager/Manager.h"
#include "Core/Core.h"
#include "Util/Cleaner.h"
#include "OpenCV/OpenCVCore.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	new Cleaner(&app);
	AppCore::Core::getInstance(&app);
	Manager::GraphManager::getInstance();

#ifdef OPENCV_FOUND
	OpenCV::OpenCVCore::getInstance(&app);
#endif


}

