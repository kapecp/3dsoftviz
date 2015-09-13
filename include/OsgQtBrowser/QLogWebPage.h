#ifndef QLOGWEBPAGE_H
#define QLOGWEBPAGE_H

#include <QWebPage>

/**
*  \class QLogWebPage
*  \brief Simple subclass of QWebPage supporting console logging
*  \author Michael Gloger
*/
class QLogWebPage : public QWebPage
{
protected:
	/**
		*  \fn protected  javaScriptConsoleMessage
		*  \brief called when there is message logged in javascript console
		*/
	void javaScriptConsoleMessage( const QString& message, int lineNumber, const QString& sourceID );
};

#endif // QLOGWEBPAGE_H
