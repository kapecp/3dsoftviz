#ifndef EVEBTTHREAH_H
#define EVEBTTHREAH_H

#include <QThread>

//using namespace System::Windows::Forms;

namespace LibMouse3d{
namespace LibCoreMouse3d{


/**
*@author Michal Fasanek
*@date 23.11.2015
*@brief LibCore class responsible for catching device activity.
*/
//TODO class MOUSE3DLIB_EXPORT EventThread : public QThread{
class EventThread : public QThread{

    Q_OBJECT

public:


    //constructor
    EventThread(QObject* parent = NULL);

    //destructor
    ~EventThread(void);

    void run();

};

}
}
#endif
