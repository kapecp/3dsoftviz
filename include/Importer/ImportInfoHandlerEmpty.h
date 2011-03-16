#ifndef Importer_ImportInfoHandlerEmpty_H
#define Importer_ImportInfoHandlerEmpty_H
//-----------------------------------------------------------------------------
#include "Importer/ImportInfoHandler.h"
//-----------------------------------------------------------------------------

namespace Importer {

/**
 * \brief Empty info handler.
 * Each method does nothing.
 */
class ImportInfoHandlerEmpty
	: public ImportInfoHandler {

public:

	/***/
	virtual ~ImportInfoHandlerEmpty (void) {};

	/***/
	virtual void addMessage (
		const QString &text
	) {
	};

	/***/
	virtual void reportError (
		const QString &text
	) {
	};

	/***/
	virtual void setProgress (
		const unsigned int value
	) {
	};

}; // class

} // namespace

#endif // Importer_ImportInfoHandlerEmpty_H
