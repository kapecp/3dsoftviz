#include "Importer/ImporterFactory.h"
//-----------------------------------------------------------------------------
#include "Importer/GraphMLImporter.h"
#include "Importer/GXLImporter.h"
#include "Importer/RSFImporter.h"
//-----------------------------------------------------------------------------

namespace Importer {

bool ImporterFactory::createByFileExtension (
	std::auto_ptr<StreamImporter> &importer,
	bool &importerFound,
	const std::string &fileExtension
) {
	bool ok = true;

	importerFound = false;

	// TODO: [ML] case insensitive compare

	if (fileExtension == "graphml") {
		importer.reset (new GraphMLImporter);
		importerFound = true;
	}

	if (fileExtension == "gxl") {
		importer.reset (new GXLImporter);
		importerFound = true;
	}

	if (fileExtension == "rsf") {
		importer.reset (new RSFImporter);
		importerFound = true;
	}

	return ok;
}

} // namespace
