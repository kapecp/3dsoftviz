#ifndef QLOGWEBPAGE_H
#define QLOGWEBPAGE_H

#include <QtGlobal>

#if QT_VERSION >= 0x050000
	#include <QtWebEngine>
	#include <QWebEnginePage>

	#define QWEBPAGE QWebEnginePage
#else
	#include <QWebPage>

	#define QWEBPAGE QWebPage
#endif



/**
*  \class QLogWebPage
*  \brief Simple subclass of QWebPage supporting console logging
*  \author Michael Gloger
*/

class QLogWebPage : public QWEBPAGE
{
protected:
	/**
		*  \fn protected  javaScriptConsoleMessage
		*  \brief called when there is message logged in javascript console
		*/
	void javaScriptConsoleMessage( const QString& message, int lineNumber, const QString& sourceID );
};

#endif // QLOGWEBPAGE_H
