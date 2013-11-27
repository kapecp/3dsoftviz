#include "Importer/ImportInfoHandler.h"

namespace Importer {

void ImportInfoHandler::reportError (
		const bool cond,
		const QString &text
		) {
	if (!cond) {
		reportError (text);
	}
}

} // namespace
