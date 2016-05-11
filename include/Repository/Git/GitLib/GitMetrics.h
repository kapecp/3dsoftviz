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

    int getFileChangedCount( QString identifier );

    int getFileChangedCount( QString identifier, int count, int start = 0 );

    int getFileChangedCount( QString identifier, int count, QString startCommitId );

    int getFileChangedCount( QString identifier, QString endCommitId, QString startCommitId );

    int getAuthorCount( QString author );

    int getAuthorCount( QString author, int count, int start = 0 );

    int getAuthorCount( QString author, int count, QString startCommitId );

    int getAuthorCount( QString author, QString endCommitId, QString startCommitId );

    QList<QString> getAuthorList();

    QList<QString> getAuthorList( int position );

    QList<QString> getFilesFromAuthor( QString author );
    QList<QString> getFilesFromAuthor( QString author, int position );

    QList<QString> getFunctionsFromFile( QString identifier );

    QList<QString> getFunctionConnectorsFromFile( QString identifier );

    QList<QString> getFunctionsFromFile( QString identifier, QString author );

    QList<QString> getFunctionConnectorsFromFile( QString identifier, QString author );

    QList<QString> getFunctionConnectorsFromFile( QString identifier, int level );

    QList<QString> getFunctionsFromFile( QString identifier, int level );

    QList<QString> getAllFiles();

    QList<QString> getFilesToPosition( int position );


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
