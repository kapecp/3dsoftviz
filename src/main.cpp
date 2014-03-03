#include <QApplication>

#include "Manager/Manager.h"
#include "Core/Core.h"
#include "Util/Cleaner.h"

#ifdef OPENCV_FOUND
    #include "OpenCV/OpenCVCore.h"
	#include "OpenCV/CamSelectCore.h"
#endif

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	new Cleaner(&app);
	AppCore::Core::getInstance(&app);
	Manager::GraphManager::getInstance();
	// problemy:
	// Q: ak odpojime zariadenie, posuvaju sa indexy.
	// A: treba premazat celu map a vytvorit nanovo
	// Q: mutexy
	// A: zrejme bude treba ale berie to strasne vela casu
#ifdef OPENCV_FOUND
	OpenCV::OpenCVCore::getInstance(&app);
#endif



}

