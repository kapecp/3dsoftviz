#include "Git/GitEvolutionGraph.h"

Git::GitEvolutionGraph::GitEvolutionGraph() {
    this->getVersions() = QList<Git::GitVersion*>();
    this->filePath = "";
    this->order = 0;
}

Git::GitEvolutionGraph::GitEvolutionGraph( QString filePath ) : filePath(filePath) {
    this->getVersions() = QList<Git::GitVersion*>();
    this->order = 0;
}
