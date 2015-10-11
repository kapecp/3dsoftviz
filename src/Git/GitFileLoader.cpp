#include "Git/GitFileLoader.h"

#include <QTemporaryFile>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QTextStream>
#include <QtCore/QRegExp>

#include "Git/GitVersion.h"
#include "Git/GitFile.h"

Git::GitFileLoader::GitFileLoader( QString filepath ) {
    this->filePath = filepath;
}

QList<Git::GitVersion*> Git::GitFileLoader::getDataAboutGit() {

    bool ok = true;
    QList<Git::GitVersion*> versions = QList<Git::GitVersion*>();;
    QString lFilePath = this->filePath;
    QString lGitCommand = "git log --raw --name-status --reverse --date=short --pretty=format:\"%H%n%an%n%ad\"";
    QString lTmp = makeTmpFileFromCommand( lGitCommand, lFilePath );

    QFile file( lTmp );

    // kontrola na uspesnost ulozenia vystupu commandu do suboru
    if( !file.size() ) {
        ok = false;
    }

    if( ok  && file.open( QIODevice::ReadOnly ) ) {
        QTextStream reader( &file );
        QString line;        
        while( !reader.atEnd() ) {       
            line = reader.readLine();
            Git::GitVersion* version = new Git::GitVersion( line );

            line = reader.readLine();
            version->setAuthor( line );

            line = reader.readLine();
            version->setDate( line );

            bool read = true;
            while( read ) {
                line = reader.readLine();
                QRegExp A( "^A\\s" );
                QRegExp M( "^M\\s" );
                QRegExp D( "^D\\s" );

                if( A.indexIn( line ) != -1 ) {
                    line.replace(A, "project/");
                    Git::GitFile* file = new Git::GitFile( line, line, Git::GitFileType::ADDED );
                    version->addChangedFile( file );
                } else if( M.indexIn( line ) != -1 ) {
                    line.replace(M, "project/");
                    Git::GitFile* file = new Git::GitFile( line, line, Git::GitFileType::MODIFIED );
                    version->addChangedFile( file );
                } else if( D.indexIn( line ) != -1 ) {
                    line.replace(D, "project/");
                    Git::GitFile* file = new Git::GitFile( line, line, Git::GitFileType::REMOVED );
                    version->addChangedFile( file );
                } else if( line.length() == 40 ) {
                    reader.readLine();
                    reader.readLine();
                } else {
                    read = false;
                }
            }
            versions.append(version);
        }
    }

    if(file.remove()) {
        qDebug() << "Podarilo sa odstranit temp subor";
    } else {
        qDebug() << "Nepodarilo sa odstranit temp subor";
    }

    versions.at(15)->printVersion();

    return versions;
}

QString Git::GitFileLoader::makeTmpFileFromCommand( QString command, QString filepath ) {
    bool ok = true;

    QString cwd = QDir::currentPath();

    QTemporaryFile tempFile;
    tempFile.setFileTemplate( QDir::toNativeSeparators( QDir::tempPath() + "/" +  "qXXXXXX" ) );
    tempFile.setAutoRemove( false );
    if(!tempFile.open()) {
        qDebug() << "Nepodarilo sa vytvorit tmp file";
    }

    if( ok ) {
        ok = changeDir( filepath );
    }

    if( ok ) {
        ok = existGit( filepath );
    }

    if( ok ) {
        QProcess* process = new QProcess;
        process->setStandardOutputFile( QDir::toNativeSeparators( tempFile.fileName () ) );
        QString lCommand = QString( command );
        process->start( lCommand );
        process->waitForFinished();
        process->close();
        process->terminate();
    }

    if( !changeDir( cwd ) ) {
        qDebug() << "Nepodarilo sa vratit na povodny current working directory";
    }

    return tempFile.fileName();
}

bool Git::GitFileLoader::changeDir( QString path ) {
    return QDir::setCurrent( path );
}

bool Git::GitFileLoader::existGit( QString path ) {
    return QDir(QDir::toNativeSeparators(path + "/.git")).exists();
}

