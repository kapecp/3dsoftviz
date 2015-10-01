#ifndef GITFILELOADER_H
#define GITFILELOADER_H

#include <QString>
#include <QList>

namespace Git {

class GitVersion;

class GitFileLoader {
public:
    GitFileLoader();
    GitFileLoader( QString filepath );

    QList<Git::GitVersion*> getGitVersions() {
        return this->versions;
    }

    void addGitVersion( Git::GitVersion* version ) {
        this->versions.append( version );
    }

    //bool getDiffInfo( QString filepath, QString currentCommitId, QString oldCommitId );
    bool getDataAboutGit();
private:
    bool changeDir( QString path );
    bool existGit( QString path );
    QString makeTmpFileFromCommand( QString command, QString filepath );

    QString filePath;
    QList<Git::GitVersion*> versions;

};
}
#endif // GITFILELOADER_H
