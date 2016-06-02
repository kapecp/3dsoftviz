#ifndef LEAPSLEEPER_H
#define LEAPSLEEPER_H

#endif // LEAPSLEEPER_H

#include <QThread>

class LeapSleeper : public QThread
{
public:
	static void usleep( unsigned long usecs )
	{
		QThread::usleep( usecs );
	}
	static void msleep( unsigned long msecs )
	{
		QThread::msleep( msecs );
	}
	static void sleep( unsigned long secs )
	{
		QThread::sleep( secs );
	}
};
