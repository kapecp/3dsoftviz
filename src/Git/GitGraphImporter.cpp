#include "Git/GitGraphImporter.h"

using namespace std;
namespace Importer {

bool GitGraphImporter::import( ImporterContext &context ) {
    this->context = &context;

    this->filepath = this->context->getFilePath();

    return false;
}

} // namespace
