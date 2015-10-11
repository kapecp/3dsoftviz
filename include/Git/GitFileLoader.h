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

    //bool getDiffInfo( QString filepath, QString currentCommitId, QString oldCommitId );
    QList<Git::GitVersion*> getDataAboutGit();
private:
    bool changeDir( QString path );
    bool existGit( QString path );
    QString makeTmpFileFromCommand( QString command, QString filepath );

    QString filePath;

};
}
#endif // GITFILELOADER_H
