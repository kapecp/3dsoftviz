#ifndef FGLOVETHREAD_H
#define FGLOVETHREAD_H

#include <QtCore>
#include <QThread>
#include <fglove.h>

namespace Fglove{

class FgloveThread : public QThread{
public:
    FgloveThread( QObject *parent = 0);
    ~FgloveThread(void);
    void run();

private:
    fdGlove *pGlove;
    int mLastGesture;
    // variable to know if nodes are selected
    bool mNodeSelected;

    // gestures numbers
    static const int flat_hand = 15;
    static const int fist = 0;
    static const int index_finger = 1;
    static const int little_finger = 8;
    static const int index_and_little_finger = 5;
    static const int index_and_middle_finger = 3;

};
}

#endif
