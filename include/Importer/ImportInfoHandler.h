#ifndef Importer_ImportInfoHandler_H
#define Importer_ImportInfoHandler_H
//-----------------------------------------------------------------------------
#include <QString>
//-----------------------------------------------------------------------------

namespace Importer {

/**
 * \brief Handles messages from importer.
 * [abstract]
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
			const QString &text
			) = 0;

	/**
	 * \brief Handles error.
	 * \param[in] text Error text.
	 */
	virtual void reportError (
			const QString &text
			) = 0;

	/**
	 * \brief Handles error if the condition is not true.
	 * \param[in] cond Condition.
	 * \param[in] text Error text.
	 */
	void reportError (
			const bool cond,
			const QString &text
			);

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
