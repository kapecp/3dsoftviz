#include "Manager/ImportInfoHandlerImpl.h"
//-----------------------------------------------------------------------------
#include "Core/Core.h"

namespace Manager {

//chybove hlasky
void ImportInfoHandlerImpl::addMessage (
		const QString &text
		) {
	throw "not implemented";
}

void ImportInfoHandlerImpl::reportError (
		const QString &text
		) {
	AppCore::Core::getInstance()->messageWindows->showMessageBox (
				"Chyba",
				text,
				true
				);
}

void ImportInfoHandlerImpl::setProgress (
		const unsigned int value
		) {
	AppCore::Core::getInstance()->messageWindows->setProgressBarValue (value);
}

} // namespace
