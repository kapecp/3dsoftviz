#ifndef Manager_ImportInfoHandlerImpl_H
#define Manager_ImportInfoHandlerImpl_H
//-----------------------------------------------------------------------------
#include "Importer/ImportInfoHandler.h"
//-----------------------------------------------------------------------------

#include <QString>

namespace Manager {

//chybove hlasky

#if defined(__GNUC__)
#define NORETURN __attribute__ ((noreturn))
#elif defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#endif

/**
 * \brief Reports import status using the QT application GUI.
 */
class ImportInfoHandlerImpl
	: public Importer::ImportInfoHandler
{

public:

	/***/
	virtual ~ImportInfoHandlerImpl( void ) {}

	/***/

	virtual void NORETURN addMessage();

	/**
	 * \brief Shows message box with error text.
	 * \param[] text Error text.
	 */
	virtual void reportError(
		const QString& text
	);

	/**
	 * \brief Updates progress bar position.
	 * \param[in] value Progress value <0, 100>.
	 */
	virtual void setProgress(
		const unsigned int value
	);

}; // class

} // namespace

#endif // Manager_ImportInfoHandlerImpl_H
