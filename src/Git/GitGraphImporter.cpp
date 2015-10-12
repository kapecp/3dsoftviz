#include "Git/GitGraphImporter.h"

#include "Importer/GraphOperations.h"
#include "Importer/ReadNodesStore.h"

using namespace std;
namespace Importer {

bool GitGraphImporter::import( ImporterContext &context ) {
    // context
    this->context = &context;

    // helpers
    this->filepath = this->context->getFilePath();
    this->graphOp.reset( new GraphOperations( this->context->getGraph() ) );
    this->readNodes.reset( new Importer::ReadNodesStore() );
    this->usedEdge;
    this->usedNode;

    // default types
    this->edgeType = NULL;
    this->nodeType = NULL;
    this->graphOp->addDefaultTypes( edgeType, nodeType );

    bool ok = true;

    return false;
}

} // namespace
