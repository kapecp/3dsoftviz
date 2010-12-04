#include "Manager/ImportInfoHandlerImpl.h"
//-----------------------------------------------------------------------------
#include "Core/Core.h"

namespace Manager {

void ImportInfoHandlerImpl::addMessage (
	const std::wstring text
) {
	throw "not implemented";
}

void ImportInfoHandlerImpl::reportError (
	const std::wstring text
) {
	/*AppCore::Core::getInstance()->messageWindows->showMessageBox (
		"Chyba",
		QString::fromStdWString (text),
		true
	);*/
}

void ImportInfoHandlerImpl::setProgress (
	const unsigned int value
) {
	AppCore::Core::getInstance()->messageWindows->setProgressBarValue (value);
}

} // namespace
