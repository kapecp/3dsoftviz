#include "Util/Cleaner.h"
#include <iostream>

using namespace Util;
using namespace std;

void Cleaner::clean()
{
    //sem kod na vycistenie pamata
    std::cout<< "About to quit\n";
}

Cleaner::Cleaner(QApplication* app)
{
    connect(app,SIGNAL(aboutToQuit()),this,SLOT(clean()));
}
