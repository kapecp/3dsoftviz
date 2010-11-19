#ifndef Importer_ImportInfoHandler_H
#define Importer_ImportInfoHandler_H
//-----------------------------------------------------------------------------
#include <string>
//-----------------------------------------------------------------------------

namespace Importer {
class ImportInfoHandler {

public:

	virtual ~ImportInfoHandler (void) {};

	virtual void addMessage (
		const std::wstring text
	) = 0;

	virtual void reportError (
		const std::wstring text
	) = 0;

	virtual void setProgress (
		const unsigned int value
	) = 0;

}; // class
} // namespace

#endif // Importer_ImportInfoHandler_H
