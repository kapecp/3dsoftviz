#include "OsgQtBrowser/QLogWebPage.h"
#include <QFileInfo>

void QLogWebPage::javaScriptConsoleMessage( const QString& message, int lineNumber, const QString& sourceID )
{
	// If sourceID is "undefined" then exception was probably thrown.
	// Otherwise sourceID contains path to file where message was logged and we want only the file name
	QString filename = sourceID;
	if ( filename != "undefined" ) {
		QStringList parts = sourceID.split( "/" );
		filename = parts.value( parts.length() - 1 );
	}

	// Log formatted message
	qDebug( "%s:%d %s", qPrintable( filename ), lineNumber, qPrintable( message ) );
	fflush( stderr );
}

