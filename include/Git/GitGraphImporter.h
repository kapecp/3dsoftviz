#ifndef GITGRAPHIMPORTER_H
#define GITGRAPHIMPORTER_H

#include "Importer/StreamImporter.h"

namespace Importer {

class GitGraphImporter : public StreamImporter {
public:
    virtual ~GitGraphImporter ( void ) {};

    virtual bool import( ImporterContext &context );

private:
    ImporterContext *context;

    QString filepath;
}; // class
} // namespace
#endif // GITFILE_H
