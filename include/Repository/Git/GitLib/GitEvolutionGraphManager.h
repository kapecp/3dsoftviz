#ifndef GITEVOLUTIONGRAPHMANAGER_H
#define GITEVOLUTIONGRAPHMANAGER_H

#include "GitLib/GitExport.h"

#include <QString>
#include <QMap>
#include <QList>

namespace Repository {
namespace Git {
class GitEvolutionGraph;
class GitVersion;
class GitFile;
class GitFileDiffBlock;
class GitFileDiffBlockLine;

class GITLIB_EXPORT GitEvolutionGraphManager
{
public:
    ~GitEvolutionGraphManager();

    static GitEvolutionGraphManager* getInstance();

    virtual void setEvolutionGraph( Repository::Git::GitEvolutionGraph* evolutionGraph );

    virtual GitEvolutionGraphManager* filterByAuthor( QString authorName );

    virtual GitEvolutionGraphManager* filterByExtension( QString extensions );

    virtual GitEvolutionGraphManager* excludeDirectories( QString directories );

    virtual GitEvolutionGraphManager* createEvolutionGraphClone();

    virtual GitEvolutionGraph* getFilteredEvolutionGraph();
private:

    GitEvolutionGraphManager();

    virtual void cloneEvolutionGraph();

    virtual Repository::Git::GitVersion* cloneVersion( Repository::Git::GitVersion* version );

    virtual Repository::Git::GitFile* cloneFile( Repository::Git::GitFile* file );

    virtual Repository::Git::GitFileDiffBlock* cloneDiffBlock( Repository::Git::GitFileDiffBlock* block );

    virtual Repository::Git::GitFileDiffBlockLine* cloneDiffBlockLine( Repository::Git::GitFileDiffBlockLine* line );

    static Repository::Git::GitEvolutionGraphManager* instance;

    static Repository::Git::GitEvolutionGraph* masterEvolutionGraph;

    Repository::Git::GitEvolutionGraph* clonedEvolutionGraph;

    QMap<QString, Repository::Git::GitFile*>* filterVersionByExtensions( Repository::Git::GitVersion* version, QString extensions );

    QMap<QString, Repository::Git::GitFile*>* filterVersionByDirectories( Repository::Git::GitVersion* version, QString directories );


    QList<Repository::Git::GitVersion*> filterVersionByAuthor( Repository::Git::GitEvolutionGraph* evolutionGraph, QString author );

}; // class
} // namespace
}
#endif // GITEVOLUTIONGRAPHMANAGER_H
