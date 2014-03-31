#include <QApplication>

#include "Manager/Manager.h"
#include "Core/Core.h"
#include "Util/Cleaner.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	new Cleaner(&app);
	AppCore::Core::getInstance(&app);


}

