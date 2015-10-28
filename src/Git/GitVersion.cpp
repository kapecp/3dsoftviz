#include "Git/GitVersion.h"

#include "Git/GitFile.h"

#include <QDebug>

Git::GitVersion::GitVersion() {
    this->changedFiles =  QList<Git::GitFile*>();
}

Git::GitVersion::GitVersion( QString commitId ) : commitId( commitId ) {
    this->changedFiles =  QList<Git::GitFile*>();
}

void Git::GitVersion::printVersion() {
    // Vypis vseobecnych informacii o verzii
    qDebug() << "CommitId : " << this->getCommitId();
    qDebug() << "Author : " << this->getAuthor();
    qDebug() << "Date : " << this->getDate();
    qDebug() << "List size : " << this->getChangedFiles().size();

    // Pre kazdy subor v ramci verzii vypisem jeho nazov, systemovu cestu a typ
    foreach( Git::GitFile* file , this->getChangedFiles() ) {
        qDebug() << file->getFilename() <<  " " << file->getFilepath() << " " << file->getTypeAsString();
    }
}

QList<Git::GitFile*> Git::GitVersion::getGitFilesByType( Git::GitType gitType ) {
    QList<Git::GitFile*> result = QList<Git::GitFile*>();

    // Pre kazdy git file skontrolujem, ci splna uvedany typ
    foreach( Git::GitFile* gitFile, getChangedFiles() ) {

        // Ak typ suboru sa zhoduje z uvedanym typom, tak subor pridam do vysledku
        if( gitFile->getType() == gitType ) {
            result.append( gitFile );
        }
    }

    return result;
}

QList<Git::GitFile*> Git::GitVersion::getGitFilesByExtension( QString extensions ) {
    QList<Git::GitFile*> result = QList<Git::GitFile*>();

    // Pre kazdy git file skontrolujem, ci ma jednu z extensions
    foreach( Git::GitFile* gitFile, getChangedFiles() ) {

        // Zistim extension
        QString extension = gitFile->getFilename().mid( gitFile->getFilename().lastIndexOf(".") + 1 );

        // Ak sa extension nachadza v extensions, tak do vysledku pridam git file
        if( extensions.indexOf( extension ) > -1 ) {
            result.append( gitFile );
        }
    }

    return result;
}

QList<Git::GitFile*> Git::GitVersion::getGitFilesByTypeAndExtension( QString extensions, Git::GitType type ) {
    QList<Git::GitFile*> result = QList<Git::GitFile*>();

    // Pre kazdy git file skontrolujem, ci ma jednu z extensions a zaroven uvedeny typ
    foreach( Git::GitFile* gitFile, getChangedFiles() ) {

        // Ak sa zhoduje typ, tak skontrolujem extension
        if( gitFile->getType() == type ) {

            // Zistim extension
            QString extension = gitFile->getFilename().mid( gitFile->getFilename().lastIndexOf(".") + 1 );

            // Ak sa extension nachadza v extensions, tak do vysledku pridam git file
            if( extensions.indexOf( extension ) > -1 ) {
            result.append( gitFile );
            }
        }
    }

    return result;
}
