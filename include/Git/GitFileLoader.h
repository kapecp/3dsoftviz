#ifndef GITFILELOADER_H
#define GITFILELOADER_H

#include <QString>
#include <QList>
#include <QFile>

namespace Git {

class GitVersion;
class GitFile;

class GitFileLoader {
public:
    GitFileLoader();
    GitFileLoader( QString filepath );

    Git::GitFile* getDiffInfo( Git::GitFile* gitFile, QString currentCommitId, QString oldCommitId );
    QList<Git::GitVersion*> getDataAboutGit();
private:
    bool changeDir( QString path );
    bool existGit( QString path );
    Git::GitFile* readGitShowFile( QString tmpFile, Git::GitFile* gitFile );
    Git::GitFile* readGitDiffFile( QString tmpFile, Git::GitFile* gitFile );
    QString makeTmpFileFromCommand( QString command, QString filepath );

    QString filePath;

};
}
#endif // GITFILELOADER_H
