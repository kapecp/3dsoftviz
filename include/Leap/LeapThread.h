#ifndef LEAPTHREAD_H
#define LEAPTHREAD_H

#include "Leap.h"

namespace Leap {

class LeapThread;

/**
 * @author Peter Sutarik
 * @brief The LeapThread class
 * @brief Thread for processing LeapMotion data
 */
class LeapThread : public QThread
{
    Q_OBJECT

public:
    LeapThread( QObject* parent=NULL );
    ~LeapThread( void );
    void run();

};
}
#endif // LEAPTHREAD_H
