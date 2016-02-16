#ifndef GITEVOLUTIONGRAPHMANAGER_H
#define GITEVOLUTIONGRAPHMANAGER_H

#include "GitLib/GitExport.h"

#include <QString>

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

    virtual Repository::Git::GitEvolutionGraph* getEvolutionGraphByAuthor( QString authorName );

    virtual Repository::Git::GitEvolutionGraph* getEvolutionGraphByExtension( QString extension );

private:

    GitEvolutionGraphManager();

    virtual Repository::Git::GitEvolutionGraph* cloneEvolutionGraph();

    virtual Repository::Git::GitVersion* cloneVersion( Repository::Git::GitVersion* version );

    virtual Repository::Git::GitFile* cloneFile( Repository::Git::GitFile* file );

    virtual Repository::Git::GitFileDiffBlock* cloneDiffBlock( Repository::Git::GitFileDiffBlock* block );

    virtual Repository::Git::GitFileDiffBlockLine* cloneDiffBlockLine( Repository::Git::GitFileDiffBlockLine* line );

    static Repository::Git::GitEvolutionGraphManager* instance;

    static Repository::Git::GitEvolutionGraph* masterEvolutionGraph;





}; // class
} // namespace
}
#endif // GITEVOLUTIONGRAPHMANAGER_H
