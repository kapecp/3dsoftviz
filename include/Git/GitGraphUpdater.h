#ifndef GITGRAPHUPDATER_H
#define GITGRAPHUPDATER_H

#include <QStringList>
#include <QList>

namespace Data {
class Graph;
}

namespace Git {
class GitEvolutionGraph;
class GitFile;

class GitGraphUpdater {
public:
    GitGraphUpdater( int currentVersion, Git::GitEvolutionGraph* evolutionGraph, Data::Graph* activeGraph );

    Data::Graph* previousVersion();

    Data::Graph* nextVersion();

    Data::Graph* changeToVersion( int toVersion );

    Git::GitEvolutionGraph* getEvolutionGraph() {
        return this->evolutionGraph;
    }

    void setEvolutionGraph( Git::GitEvolutionGraph* evolutionGraph ) {
        this->evolutionGraph = evolutionGraph;
    }

    Data::Graph* getActiveGraph() {
        return this->activeGraph;
    }

    void setActiveGraph( Data::Graph* activeGraph ) {
        this->activeGraph = activeGraph;
    }

    int getCurrentVersion() {
        return this->currentVersion;
    }

    void setCurrentVersion( int currentVersion ) {
        this->currentVersion = currentVersion;
    }

private:

    void addNodesToGraph( QStringList list );
    void addEdgesToGraph( QStringList list );
    void addAuthorEdgesToGraph( QString authorName, QList<Git::GitFile*> gitFiles );
    void removeAuthorEdgesFromGraph( QString authorName, QList<Git::GitFile*> gitFiles );
    void removeNodesFromGraph( QStringList list );
    void removeEdgesFromGraph( QStringList list );
    void modifyNodesInGraph( QStringList list );

    bool moreVersionChanged;

    int currentVersion;
    Git::GitEvolutionGraph* evolutionGraph;
    Data::Graph* activeGraph;
}; // class
} // namespace
#endif // GITGRAPHUPDATER_H
