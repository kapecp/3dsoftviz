#ifndef GITVERSION_H
#define GITVERSION_H

#include <QString>
#include <QList>

namespace Git {

class GitFile;

class GitVersion {
public:
    GitVersion();
    GitVersion( QString commitId );

    QString getCommitId() {
        return this->commitId;
    }

    void setAuthor( QString author ) {
        this->author = author;
    }

    QString getAuthor() {
        return this->author;
    }

    void setDate( QString date ) {
        this->date = date;
    }

    QString getDate() {
        return this->date;
    }

    QList<Git::GitFile*> getChangedFiles() {
        return this->changedFiles;
    }

    void addChangedFile( Git::GitFile* file ) {
        this->changedFiles.append( file );
    }

    void printVersion();

private:
    QString commitId;
    QString author;
    QString date;
    QList<Git::GitFile*> changedFiles;
};
}
#endif // GITVERSION_H
