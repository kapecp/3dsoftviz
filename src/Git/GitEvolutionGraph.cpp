#include "Git/GitEvolutionGraph.h"

Git::GitEvolutionGraph::GitEvolutionGraph() {

    // inicializacia premmenych na defaultne hodnoty
    this->getVersions() = QList<Git::GitVersion*>();
    this->filePath = "";
}

Git::GitEvolutionGraph::GitEvolutionGraph( QString filePath ) : filePath(filePath) {
    this->getVersions() = QList<Git::GitVersion*>();
}
