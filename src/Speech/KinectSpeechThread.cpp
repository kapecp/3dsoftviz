#include "Core/Core.h"
#include "Speech/KinectSpeechThread.h"

using namespace Speech;
using namespace SpeechSDK;

Speech::KinectSpeechThread::KinectSpeechThread()
{
	this->m_SpeechClass = new SpeechSDKClass(L"../share/3dsoftviz/SpeechGrammarFile.grxml");
	this->cancel=false;
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
	while (!cancel)
	{
		const wchar_t *a= this->m_SpeechClass->recognize();
		if (0== wcscmp(a,L"NULL")){
			printf("null\n");
		}
		if (0== wcscmp(a,L"ALL")){
			printf("select all nodes\n");
			AppCore::Core::getInstance()->getCoreWindow()->multiSelectClicked(true);
			QOSG::ViewerQT *viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
			if(viewer!=NULL)
			{
				int mWindowWidth=viewer->width();
				int mWindowHeight=viewer->height();
				viewer->getEventQueue()->mouseButtonPress(0,0,Qt::LeftButton);
				viewer->getEventQueue()->mouseMotion(mWindowWidth,mWindowHeight);
				viewer->getEventQueue()->mouseButtonRelease(mWindowWidth,mWindowHeight,Qt::LeftButton);
			}
		}
		if (0== wcscmp(a,L"CIRCLE")){
			printf("circle\n");
			AppCore::Core::getInstance()->getCoreWindow()->setRestriction_Sphere();
		}
		if (0== wcscmp(a,L"LEFT")){
			printf("select left side\n");
			AppCore::Core::getInstance()->getCoreWindow()->multiSelectClicked(true);
			QOSG::ViewerQT *viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
			if(viewer!=NULL)
			{
				int mWindowWidth=viewer->width();
				int mWindowHeight=viewer->height();
				viewer->getEventQueue()->mouseButtonPress(0,0,Qt::LeftButton);
				viewer->getEventQueue()->mouseMotion(mWindowWidth/2,mWindowHeight);
				viewer->getEventQueue()->mouseButtonRelease(mWindowWidth/2,mWindowHeight,Qt::LeftButton);
			}
		}
		if (0== wcscmp(a,L"RIGHT")){
			printf("select right ride\n");
			AppCore::Core::getInstance()->getCoreWindow()->multiSelectClicked(true);
			QOSG::ViewerQT *viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
			if(viewer!=NULL)
			{
				int mWindowWidth=viewer->width();
				int mWindowHeight=viewer->height();
				viewer->getEventQueue()->mouseButtonPress(mWindowWidth/2,0,Qt::LeftButton);
				viewer->getEventQueue()->mouseMotion(mWindowWidth,mWindowHeight);
				viewer->getEventQueue()->mouseButtonRelease(mWindowWidth,mWindowHeight,Qt::LeftButton);
			}
		}
		if (0== wcscmp(a,L"CLEAR")){
			printf("clear screen\n");
			AppCore::Core::getInstance()->getCoreWindow()->multiSelectClicked(false);
			QOSG::ViewerQT *viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
			if(viewer!=NULL)
			{
				viewer->getEventQueue()->mouseButtonPress(0,0,Qt::LeftButton);
			}
		}
		if (0== wcscmp(a,L"UNSET")){
			printf("unset restrictions\n");
			AppCore::Core::getInstance()->getCoreWindow()->unsetRestriction();
		}
	}
}
