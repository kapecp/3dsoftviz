#ifndef GITFILELOADER_H
#define GITFILELOADER_H

#include <QString>

namespace Git {

class GitFileLoader {
public:
    GitFileLoader();
    GitFileLoader( QString filepath );

    bool getDataAboutGit();
private:
    bool changeDir( QString path );
    bool existGit( QString path );
    QString makeTmpFileFromLog( QString command, QString filepath );

    QString filePath;

};
}
#endif // GITFILELOADER_H
