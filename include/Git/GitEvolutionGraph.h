#ifndef GITEVOLUTIONGRAPH_H
#define GITEVOLUTIONGRAPH_H

#include <QString>
#include <QList>

namespace Git {
class GitVersion;

class GitEvolutionGraph {
public:
    GitEvolutionGraph();

    GitEvolutionGraph( QString filePath );

    void setVersions( QList<Git::GitVersion*> versions ) {
        this->versions = versions;
    }

    QList<Git::GitVersion*> getVersions() {
        return this->versions;
    }

    void setOrder( int order ) {
        this->order = order;
    }

    int getOrder() {
        return this->order;
    }

    void setFilePath( QString filePath ) {
        this->filePath = filePath;
    }

    QString getFilePath() {
        return this->filePath;
    }

private:
    QList<Git::GitVersion*> versions;
    int order;
    QString filePath;

};
}
#endif // GITEVOLUTIONGRAPH_H
