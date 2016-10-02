#ifdef SPEECHSDK_EXPORTS
#define SPEECHSDK_API __declspec(dllexport)
#else
#define SPEECHSDK_API __declspec(dllimport)
#endif

//------------------------------------------------------------------------------
// <copyright file="SpeechBasics.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once

#include "KinectAudioStream.h"
#include "resource.h"

// For configuring DMO properties
#include <wmcodecdsp.h>

// For FORMAT_WaveFormatEx and such
#include <uuids.h>

// For Kinect SDK APIs
#include <NuiApi.h>

// For speech APIs
// NOTE: To ensure that application compiles and links against correct SAPI versions (from Microsoft Speech
//       SDK), VC++ include and library paths should be configured to list appropriate paths within Microsoft
//       Speech SDK installation directory before listing the default system include and library directories,
//       which might contain a version of SAPI that is not appropriate for use together with Kinect sensor.
#include <sapi.h>
#include <sphelper.h>

namespace SpeechSDK {
/// <summary>
/// Main application class for SpeechBasics sample.
/// </summary>
class SpeechSDKClass
{
public:
	SPEECHSDK_API SpeechSDKClass( LPCWSTR name );
	SPEECHSDK_API ~SpeechSDKClass();

	SPEECHSDK_API int initRecognition();
	SPEECHSDK_API void stopRecognition();
	SPEECHSDK_API void cancelRecognition();

	//static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	SPEECHSDK_API HRESULT CreateFirstConnected( int i );
	SPEECHSDK_API LPCWSTR recognize();
	SPEECHSDK_API LPCWSTR ProcessSpeech();

private:
	void setGrammarName( LPCWSTR name );
	static void setFileName( LPCWSTR name );
	HRESULT InitializeAudioStream();
	HRESULT StartSpeechRecognition();
	static bool cancel;
	// Speech grammar
	ISpRecoGrammar* m_pSpeechGrammar;
	LPCWSTR GrammarFileName;
	// Current Kinect sensor
	INuiSensor* m_pNuiSensor;
	// Audio stream captured from Kinect.
	KinectAudioStream* m_pKinectAudioStream;
	// Stream given to speech recognition engine
	ISpStream* m_pSpeechStream;
	// Speech recognizer
	ISpRecognizer* m_pSpeechRecognizer;
	// Speech recognizer context
	ISpRecoContext* m_pSpeechContext;
	// Event triggered when we detect speech recognition
	HANDLE m_hSpeechEvent;
	HRESULT CreateSpeechRecognizer();
	HRESULT LoadSpeechGrammar();
};
}
