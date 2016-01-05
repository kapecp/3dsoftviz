#include "GitLib/GitUtils.h"

#include <QTemporaryFile>
#include <QDir>
#include <QProcess>
#include <QDebug>

QString Repository::Git::GitUtils::makeTmpFileFromCommand( QString command, QString filepath )
{
	bool ok = true;

	// Ulozim si current working directory
	QString cwd = QDir::currentPath();

	// Nastavim absolutnu cestu k  temp file ako template a zakazem automaticke mazanie
	QTemporaryFile tempFile;
	tempFile.setFileTemplate( QDir::toNativeSeparators( QDir::tempPath() + "/" +  "qXXXXXX" ) );
	tempFile.setAutoRemove( false );

	// Ak sa nepodarilo vytvorit temp subor, tak nastavim flag "ok" na false a vypisem chybu
	if ( !tempFile.open() ) {
		qDebug() << "Nepodarilo sa vytvorit tmp subor";
		ok = false;
	}

	// Ak sa podarilo vytvorit temp subor, tak zmenim current working directory
	if ( ok ) {
		ok = changeDir( filepath );
	}

	// Ak sa podarilo zmenit current working directory, tak skontroluje existenciu git repozitara
	if ( ok ) {
		ok = existGit( filepath );
	}

	// Ak existuje na danej ceste git repozitar, tak vykonam command a vystup ulozim do temp suboru
	if ( ok ) {
		QProcess process;
		process.setStandardOutputFile( QDir::toNativeSeparators( tempFile.fileName() ) );
        process.start( command );
		process.waitForFinished();
		process.close();
		process.terminate();

	}

	// Vratim povodny current working directory, ak sa nepodari zmenit, vypisem do konzoly
	if ( !changeDir( cwd ) ) {
		qDebug() << "Nepodarilo sa vratit na povodny current working directory";
	}

	// Vratim absolutnu cestu k temp suboru
	return tempFile.fileName();
}

bool Repository::Git::GitUtils::changeDir( QString path )
{
	return QDir::setCurrent( path );
}

bool Repository::Git::GitUtils::existGit( QString path )
{
	return QDir( QDir::toNativeSeparators( path + "/.git" ) ).exists();
}

bool Repository::Git::GitUtils::changeCommit( QString commitId, QString filePath ) {
    bool ok = true;
    QString lCommand = "git checkout " + commitId;

    QString lTmp = makeTmpFileFromCommand( lCommand, filePath );

    QFile file( lTmp );

    // Kontrola na uspesnost ulozenia vystupu commandu do suboru, ak sa podarilo tak "ok" ostane true
    if ( !file.size() ) {
        ok = false;
    }

    // Ak sa mi nepodari vymazat subor, tak vypisem jeho nazov
    if ( !file.remove() ) {
        qDebug() << "Nepodarilo sa odstranit temp subor" << file.fileName();
    }

    return ok;
}



