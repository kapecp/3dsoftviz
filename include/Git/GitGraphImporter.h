#ifndef GITGRAPHIMPORTER_H
#define GITGRAPHIMPORTER_H

#include "Importer/StreamImporter.h"

#include <QList>
#include <QMap>

namespace Data {
class Type;
}

namespace Git {
class GitFile;
}

namespace Importer {

class ReadNodesStore;
class ReadEdgesStore;
class GraphOperations;

class GitGraphImporter : public StreamImporter {
public:
    virtual ~GitGraphImporter ( void ) {};

    virtual bool import( ImporterContext &context );

    virtual bool makeGraph (
            QList<Git::GitFile*> gitFiles
    );

    virtual bool addNode (
            QStringList &list
    );

    virtual bool addEdge (
            QStringList &list
    );
/*
    virtual bool compare(
            QString &readEdges,
            QString &edgeName
    );
*/
private:
    ImporterContext *context;

    QString filepath;
    std::auto_ptr<Importer::GraphOperations> graphOp;
    std::auto_ptr<Importer::ReadNodesStore> readNodes;
    std::auto_ptr<Importer::ReadEdgesStore> readEdges;
    QMap<QString, bool> usedEdge;
    QMap<QString, bool> usedNode;

    // default types
    Data::Type* edgeType;
    Data::Type* nodeType;

}; // class
} // namespace
#endif // GITFILE_H
