#ifndef KINECTSPEECHTHREAD_H
#define KINECTSPEECHTHREAD_H

#include <QThread>
//#pragma component(interface, on, references, __interface)
#include <SpeechSDK.h>

namespace Speech

{
class KinectSpeechThread;

/**
	 * @brief Class KinectSpeechThread
	 * @author Autor: Marek Jakab
	 * @date 22.3.2014
	 */
class KinectSpeechThread : public QThread
{
	Q_OBJECT

public:
	KinectSpeechThread();
	~KinectSpeechThread(void);
	int initializeSpeech();
	SpeechSDK::SpeechSDKClass *m_SpeechClass;
	bool isConnected;
	bool cancel;

	/**
		 * @author Autor: Marek Jakab
		 * @brief run Starts thread
		 */
	void run();

};
}


#endif // KINECTSPEECHTHREAD_H
