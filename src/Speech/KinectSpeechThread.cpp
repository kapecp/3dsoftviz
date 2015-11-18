#include "Core/Core.h"
#include "Speech/KinectSpeechThread.h"

using Speech;
using SpeechSDK

// constructor loads grammar file with speech commands
// DLL
Speech::KinectSpeechThread::KinectSpeechThread() :
	m_SpeechClass( new SpeechSDKClass( L"../share/3dsoftviz/SpeechGrammarFile.grxml" ) ),
	isConnected( false ),
	cancel( false )
{
}


Speech::KinectSpeechThread::~KinectSpeechThread( void )
{
	if ( isConnected ) {
		this->m_SpeechClass->cancelRecognition();
		this->m_SpeechClass->stopRecognition();
	}
}

// initialize kinect along with audio and grammar
int Speech::KinectSpeechThread::initializeSpeech()
{
	if ( this->m_SpeechClass->initRecognition() ) {
		qDebug() << "Speech initialization failure";
		this->isConnected=false;
		return 1;
	}
	if ( this->m_SpeechClass->CreateFirstConnected( 0 )!=0 ) {
		qDebug() << "Speech connection failure";
		this->isConnected=false;
		return 1;
	}
	this->isConnected=true;
	return 0;
}

// run thread for speech recognition
// commands will be shown in the command line output
void Speech::KinectSpeechThread::run()
{
	qDebug() << "Speech initialized! Possible commands:";
	qDebug() << "\"select all nodes\"";
	qDebug() << "\"select left side\"";
	qDebug() << "\"select right side\"";
	qDebug() << "\"clear screen\"";
	qDebug() << "\"sphere\"";
	qDebug() << "\"unset restrictions\"";
	while ( !cancel ) {
		// recognize next input
		// this method is blocking, code will stop here until some word is received
		const wchar_t* a= this->m_SpeechClass->recognize();
		// compare recognized word with the TAG in grammar file
		// when recognized, do commands specific for each phrase
		if ( 0== wcscmp( a,L"ALL" ) ) {
			qDebug() << "command: select all nodes";
			AppCore::Core::getInstance()->getCoreWindow()->multiSelectClicked( true );
			QOSG::ViewerQT* viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
			if ( viewer!=NULL ) {
				int mWindowWidth=viewer->width();
				int mWindowHeight=viewer->height();
				viewer->getEventQueue()->mouseButtonPress( 0,0,Qt::LeftButton );
				viewer->getEventQueue()->mouseMotion( mWindowWidth,mWindowHeight );
				viewer->getEventQueue()->mouseButtonRelease( mWindowWidth,mWindowHeight,Qt::LeftButton );
			}
		}
		if ( 0== wcscmp( a,L"SPHERE" ) ) {
			qDebug() << "command: sphere";
			AppCore::Core::getInstance()->getCoreWindow()->setRestriction_Sphere();
		}
		if ( 0== wcscmp( a,L"LEFT" ) ) {
			qDebug() << "command: select left side";
			AppCore::Core::getInstance()->getCoreWindow()->multiSelectClicked( true );
			QOSG::ViewerQT* viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
			if ( viewer!=NULL ) {
				int mWindowWidth=viewer->width();
				int mWindowHeight=viewer->height();
				viewer->getEventQueue()->mouseButtonPress( 0,0,Qt::LeftButton );
				viewer->getEventQueue()->mouseMotion( mWindowWidth/2,mWindowHeight );
				viewer->getEventQueue()->mouseButtonRelease( mWindowWidth/2,mWindowHeight,Qt::LeftButton );
			}
		}
		if ( 0== wcscmp( a,L"RIGHT" ) ) {
			qDebug() << "command: select right side";
			AppCore::Core::getInstance()->getCoreWindow()->multiSelectClicked( true );
			QOSG::ViewerQT* viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
			if ( viewer!=NULL ) {
				int mWindowWidth=viewer->width();
				int mWindowHeight=viewer->height();
				viewer->getEventQueue()->mouseButtonPress( mWindowWidth/2,0,Qt::LeftButton );
				viewer->getEventQueue()->mouseMotion( mWindowWidth,mWindowHeight );
				viewer->getEventQueue()->mouseButtonRelease( mWindowWidth,mWindowHeight,Qt::LeftButton );
			}
		}
		if ( 0== wcscmp( a,L"CLEAR" ) ) {
			qDebug() << "command: clear screen";
			AppCore::Core::getInstance()->getCoreWindow()->multiSelectClicked( false );
			QOSG::ViewerQT* viewer=AppCore::Core::getInstance()->getCoreWindow()->GetViewerQt();
			if ( viewer!=NULL ) {
				viewer->getEventQueue()->mouseButtonPress( 0,0,Qt::LeftButton );
			}
		}
		if ( 0== wcscmp( a,L"UNSET" ) ) {
			qDebug() << "command: unset restrictions";
			AppCore::Core::getInstance()->getCoreWindow()->unsetRestriction();
		}
	}
}
