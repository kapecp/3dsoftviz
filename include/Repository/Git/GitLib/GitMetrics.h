#ifndef GITMETRICS_H
#define GITMETRICS_H

#include "GitLib/GitExport.h"

#include <QString>

namespace Repository {

namespace Git {
class GitEvolutionGraph;

class GITLIB_EXPORT GitMetrics
{
public:
    GitMetrics();

    GitMetrics( Repository::Git::GitEvolutionGraph* evolutionGraph, QString commitId );

    int getChangedCount( QString identifier );

    int getChangedCount( QString identifier, int count, int start = 0 );

    void setEvolutionGraph( Repository::Git::GitEvolutionGraph* evolutionGraph ) {
        this->evolutionGraph = evolutionGraph;
    }

    Repository::Git::GitEvolutionGraph* getEvolutionGraph() {
        return this->evolutionGraph;
    }

    void setCommitId( QString commitId ) {
        this->commitId = commitId;
    }

    QString getCommitId() {
        return this->commitId;
    }

private:

    Repository::Git::GitEvolutionGraph* evolutionGraph;

    QString commitId;
}; // class
} // namespace
}
#endif // GITMETRICS_H
