#ifndef Importer_ImportInfoHandler_H
#define Importer_ImportInfoHandler_H
//-----------------------------------------------------------------------------
#include <string>
//-----------------------------------------------------------------------------

namespace Importer {

/**
 * \brief Handles messages from importer.
 * [interface]
 * Implementations should define reactions to importer status:
 * - messages
 * - errors
 * - progress
 */
class ImportInfoHandler {

public:

	/***/
	virtual ~ImportInfoHandler (void) {};

	/**
	 * \brief Handles message (information from the importer).
	 * \param[in] text Message text.
	 */
	virtual void addMessage (
		const std::wstring text
	) = 0;

	/**
	 * \brief Handles error.
	 * \param[in] text Error text.
	 */
	virtual void reportError (
		const std::wstring text
	) = 0;

	/**
	 * \brief Handles progress change.
	 * \param[in] value Progress value <0, 100>.
	 */
	virtual void setProgress (
		const unsigned int value
	) = 0;

}; // class

} // namespace

#endif // Importer_ImportInfoHandler_H
