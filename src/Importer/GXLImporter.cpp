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

	bool ok = true;

	while (
		ok
		&&
		!xml.hasError ()
		&&
		!xml.atEnd ()
	) {

	}

	if (
		!ok
		||
		xml.hasError ()
	) {
		context.getInfoHandler ().reportError (
			std::wstring (L"Zvoleny subor nie je validny GXL subor.")
		);
	}

	xml.clear ();

	return ok && !xml.hasError ();
}

} // namespace
