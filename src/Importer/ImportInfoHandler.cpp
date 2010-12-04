#include "Importer/ImportInfoHandler.h"

namespace Importer {

void ImportInfoHandler::reportError (
	const bool cond,
	const std::wstring &text
) {
	if (!cond) {
		reportError (text);
	}
}

} // namespace
