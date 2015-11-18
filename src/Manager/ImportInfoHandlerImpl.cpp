#include "Manager/ImportInfoHandlerImpl.h"
//-----------------------------------------------------------------------------
#include "Core/Core.h"
#include "QOSG/MessageWindows.h"

namespace Manager {

//chybove hlasky

#if defined(__GNUC__)
#define NORETURN __attribute__ ((noreturn))
#elif defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#endif

void ImportInfoHandlerImpl::addMessage()
{
	throw "not implemented";
}

void ImportInfoHandlerImpl::reportError(
	const QString& text
)
{
	AppCore::Core::getInstance()->messageWindows->showMessageBox(
		"Chyba",
		text,
		true
	);
}

void ImportInfoHandlerImpl::setProgress(
	const unsigned int value
)
{
	AppCore::Core::getInstance()->messageWindows->setProgressBarValue( static_cast<int>( value ) );
}

} // namespace
