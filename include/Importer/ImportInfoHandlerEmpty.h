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
	: public ImportInfoHandler
{

public:
	using ImportInfoHandler::reportError;
	using ImportInfoHandler::setProgress;
	/***/
	virtual ~ImportInfoHandlerEmpty( void ) {}

	/***/

	virtual void addMessage()
	{
	}

	/***/
	virtual void reportError()
	{
	}

	/***/
	virtual void setProgress()
	{
	}

}; // class

} // namespace

#endif // Importer_ImportInfoHandlerEmpty_H
