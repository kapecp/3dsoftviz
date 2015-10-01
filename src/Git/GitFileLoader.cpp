#include "Git/GitFileLoader.h"

#include <QTemporaryFile>
#include <QDir>
#include <QFile>
#include <QProcess>

Git::GitFileLoader::GitFileLoader( QString filepath ) {
    this->filePath = filepath;
}

bool Git::GitFileLoader::getDataAboutGit() {
    bool ok = true;
    QString lFilePath = this->filePath;
    QString lGitCommand = "git log --raw --name-status --reverse --date=short --pretty=format:\"%H%n%an%n%ad\"";
    QString lTmp = makeTmpFileFromLog( lGitCommand, lFilePath );

    qDebug() << "Hello from getDataAboutGit";
    qDebug() << lTmp << " som dostal z makeTmpFileFromLog";

    QFile file( lTmp );

    // kontrola na uspesnost ulozenia vystupu commandu do suboru
    if( !file.size() ) {
        ok = false;
    }

    if(file.remove()) {
        qDebug() << "Podarilo sa odstranit temp subor";
    } else {
        qDebug() << "Nepodarilo sa odstranit temp subor";
    }

    return ok;
}

QString Git::GitFileLoader::makeTmpFileFromLog( QString command, QString filepath ) {
    bool ok = true;

    qDebug() << "GetCurrentDir returns " << ok ;
    qDebug() << QDir::tempPath();

    QTemporaryFile tempFile;
    tempFile.setFileTemplate( QDir::toNativeSeparators( QDir::tempPath() + "/" +  "qXXXXXX" ) );
    tempFile.setAutoRemove( false );
    if(!tempFile.open()) {
        qDebug() << "Nepodarilo sa vytvorit tmp file";
    }

    if( ok ) {
        ok = changeDir( filepath );
        qDebug() << "changeDir returns " << ok;
        qDebug() << "cwd = " << QDir::current();
    }

    if( ok ) {
        ok = existGit( filepath );
        qDebug() << "existGit returns " << ok;
    }

    if( ok ) {
        QProcess* process = new QProcess;
        process->setStandardOutputFile( QDir::toNativeSeparators( tempFile.fileName () ) );
        QString lCommand = QString( command ); // + " > " + QDir::toNativeSeparators( tempFile.fileName() ) + " 2>&1" );
        qDebug() << "Vysledny command : " << lCommand;
        process->start( lCommand );
        process->waitForFinished();
        process->close();
        process->terminate();
    }

    return tempFile.fileName();
}

bool Git::GitFileLoader::changeDir( QString path ) {
    return QDir::setCurrent( path );
}

bool Git::GitFileLoader::existGit( QString path ) {
    return QDir(QDir::toNativeSeparators(path + "/.git")).exists();
}
