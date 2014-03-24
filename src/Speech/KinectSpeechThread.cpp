
#include "Speech/KinectSpeechThread.h"

using namespace Speech;
using namespace SpeechSDK;

Speech::KinectSpeechThread::KinectSpeechThread()
{
	this->m_SpeechClass = new SpeechSDKClass(L"../share/3dsoftviz/SpeechGrammarFile.grxml");
	this->m_SpeechClass->initRecognition();
	this->m_SpeechClass->CreateFirstConnected();
}


Speech::KinectSpeechThread::~KinectSpeechThread(void)
{

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
