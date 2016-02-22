#include "Importer/ImporterFactory.h"
//-----------------------------------------------------------------------------
#include "Importer/GraphMLImporter.h"
#include "Importer/GXLImporter.h"
#include "Importer/RSFImporter.h"
//-----------------------------------------------------------------------------

namespace Importer {

bool ImporterFactory::createByFileExtension(
	std::shared_ptr<StreamImporter>& importer,
	bool& importerFound,
	const QString& fileExtension
)
{
	bool ok = true;

	importerFound = false;

	if ( 0 == fileExtension.compare( "graphml", Qt::CaseInsensitive ) ) {
		importer.reset( new GraphMLImporter );
		importerFound = true;
	}

	if ( 0 == fileExtension.compare( "gxl", Qt::CaseInsensitive ) ) {
		importer.reset( new GXLImporter );
		importerFound = true;
	}

	if ( 0 == fileExtension.compare( "rsf", Qt::CaseInsensitive ) ) {
		importer.reset( new RSFImporter );
		importerFound = true;
	}

	return ok;
}

} // namespace
