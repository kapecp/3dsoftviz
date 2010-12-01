#include "Importer/GXLImporter.h"
//-----------------------------------------------------------------------------
#include "Util/Adapter_iostream_to_QIODevice.h"
//-----------------------------------------------------------------------------
#include <QtXml/QXmlStreamReader>
//-----------------------------------------------------------------------------
#include <memory>

namespace Importer {

bool GXLImporter::import (
	ImporterContext &context
) {
	Util::Adapter_iostream_to_QIODevice dev (context.getStream());
	QXmlStreamReader xml (&dev);

	context.getInfoHandler ().reportError (
		std::wstring (L"Gxl")
	);

	bool ok = true;

	if (ok) {
		ok = !xml.hasError ();
	}

	while (ok && !xml.atEnd ()) {



		if (ok) {
			ok = !xml.hasError ();
		}
	}

	if (!ok) {
		context.getInfoHandler ().reportError (
			std::wstring (L"Zvoleny subor nie je validny GXL subor.")
		);
	}

	xml.clear ();

	return ok;
}

} // namespace
