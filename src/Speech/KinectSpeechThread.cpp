
#include "Speech/KinectSpeechThread.h"

using namespace Speech;
using namespace SpeechSDK;

Speech::KinectSpeechThread::KinectSpeechThread()
{
	this->m_SpeechClass = new SpeechSDKClass(L"../share/3dsoftviz/SpeechGrammarFile.grxml");
}


Speech::KinectSpeechThread::~KinectSpeechThread(void)
{
	if (isConnected)
	{
		this->m_SpeechClass->cancelRecognition();
		this->m_SpeechClass->stopRecognition();
	}
}

int Speech::KinectSpeechThread::initializeSpeech()
{
	if (this->m_SpeechClass->initRecognition())
	{
		qDebug() << "Speech initialization failure";
		this->isConnected=false;
		return 1;
	}
	if (this->m_SpeechClass->CreateFirstConnected()<0)
	{
		qDebug() << "Speech connection failure";
		this->isConnected=false;
		return 1;
	}
	this->isConnected=true;
	return 0;
}

void Speech::KinectSpeechThread::run()
{
	while (true)
	{
		const wchar_t *a= this->m_SpeechClass->recognize();
		if (0== wcscmp(a,L"NULL")){
			printf("null\n");
		}
		if (0== wcscmp(a,L"FORWARD")){
			printf("forward\n");
		}
		if (0== wcscmp(a,L"BACK")){
			printf("backward\n");
		}
		if (0== wcscmp(a,L"LEFT")){
			printf("left\n");
		}
		if (0== wcscmp(a,L"RIGHT")){
			printf("right\n");
		}
	}
}
