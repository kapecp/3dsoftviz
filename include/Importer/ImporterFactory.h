#ifndef Importer_ImporterFactory_H
#define Importer_ImporterFactory_H
//-----------------------------------------------------------------------------
#include "Importer/StreamImporter.h"
//-----------------------------------------------------------------------------
#include <QString>
//-----------------------------------------------------------------------------
#include <memory>

namespace Importer {

/**
 * \brief Contains methods to create instances of importer implementations according to specified criteria.
 */
class ImporterFactory
{

public:

	/**
	 * \brief Creates importer according to provided file extension.
	 * \param[out] importer Created importer.
	 * \param[out] importerFound If an appropriate importer has been found.
	 * \param[in] fileExtension File name extension.
	 * \return ( = success)
	 */
	static bool createByFileExtension(
		std::shared_ptr<StreamImporter>& importer,
		bool& importerFound,
		const QString& fileExtension
	);

private:

	/***/
	ImporterFactory( void ) {}

	/***/
	virtual ~ImporterFactory( void ) = 0;


}; // class

} // namespace

#endif // Importer_ImporterFactory_H
