#ifndef GITMETRICS_H
#define GITMETRICS_H

#include "GitLib/GitExport.h"

#include <QString>
#include <QList>

namespace Repository {

namespace Git {
class GitEvolutionGraph;

class GITLIB_EXPORT GitMetrics
{
public:
    GitMetrics();

    GitMetrics( Repository::Git::GitEvolutionGraph* evolutionGraph );

    int getChangedCount( QString identifier );

    int getChangedCount( QString identifier, int count, int start = 0 );

    int getChangedCount( QString identifier, int count, QString startCommitId );

    int getChangedCount( QString identifier, QString endCommitId, QString startCommitId );

    int getAuthorCount( QString author );

    int getAuthorCount( QString author, int count, int start = 0 );

    int getAuthorCount( QString author, int count, QString startCommitId );

    int getAuthorCount( QString author, QString endCommitId, QString startCommitId );

    QList<QString> getAuthorList();

    QList<QString> getAuthorList( int position );

    QList<QString> getFilesFromAuthor( QString author );
    QList<QString> getFilesFromAuthor( QString author, int position );


    void setEvolutionGraph( Repository::Git::GitEvolutionGraph* evolutionGraph ) {
        this->evolutionGraph = evolutionGraph;
    }

    Repository::Git::GitEvolutionGraph* getEvolutionGraph() {
        return this->evolutionGraph;
    }

private:

    Repository::Git::GitEvolutionGraph* evolutionGraph;

}; // class
} // namespace
}
#endif // GITMETRICS_H
