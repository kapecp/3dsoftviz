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
    // kedy sa ma zastavit rozpoznavanie
    bool mCancel;
    fdGlove *pGlove;
    int mLastGesture;
    bool mNodeSelected;

    static const int flat_hand = 15;
    static const int fist = 0;
    static const int index_finger = 1;
    static const int little_finger = 8;
    static const int index_and_little_finger = 5;
    static const int index_and_middle_finger = 3;



};
}

#endif
