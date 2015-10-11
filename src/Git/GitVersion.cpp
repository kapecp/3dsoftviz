#include "Git/GitVersion.h"

#include <QDebug>

#include "Git/GitFile.h"

Git::GitVersion::GitVersion() {}

Git::GitVersion::GitVersion( QString commitId ) : commitId( commitId ) {
    this->changedFiles =  QList<Git::GitFile*>();
}

void Git::GitVersion::printVersion() {
    qDebug() << "CommitId : " << this->getCommitId();
    qDebug() << "Author : " << this->getAuthor();
    qDebug() << "Date : " << this->getDate();
    qDebug() << "List size : " << this->getChangedFiles().size();
    foreach( Git::GitFile* file , this->getChangedFiles() ) {
        qDebug() << file->getFilepath() << " " << file->getTypeString();
    }
}
