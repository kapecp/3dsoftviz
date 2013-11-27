#include "Util/Cleaner.h"

void Cleaner::clean()
{
	//sem kod na vycistenie pamata
	cout<< "About to quit\n";
}

Cleaner::Cleaner(QApplication* app)
{
	connect(app,SIGNAL(aboutToQuit()),this,SLOT(clean()));
}
