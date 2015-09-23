#include "Util/Cleaner.h"
#include <iostream>

namespace Util {

void Cleaner::clean()
{
	//sem kod na vycistenie pamata
	std::cout<< "About to quit\n";
}

Cleaner::Cleaner( QApplication* app )
{
	connect( app,SIGNAL( aboutToQuit() ),this,SLOT( clean() ) );
}

} // namespace Util
