#ifndef Manager_ImportInfoHandlerImpl_H
#define Manager_ImportInfoHandlerImpl_H
//-----------------------------------------------------------------------------
#include "Importer/ImportInfoHandler.h"
//-----------------------------------------------------------------------------

namespace Manager {
class ImportInfoHandlerImpl
	: public Importer::ImportInfoHandler {

public:

	virtual ~ImportInfoHandlerImpl (void) {};

	virtual void addMessage (
		const std::wstring text
	);

	virtual void reportError (
		const std::wstring text
	);

	virtual void setProgress (
		const unsigned int value
	);

}; // class
} // namespace

#endif // Manager_ImportInfoHandlerImpl_H
