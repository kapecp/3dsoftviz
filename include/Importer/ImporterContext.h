#ifndef Importer_ImporterContext_H
#define Importer_ImporterContext_H
//-----------------------------------------------------------------------------

#include "Importer/ImportInfoHandler.h"
//-----------------------------------------------------------------------------
#include <QIODevice>
//-----------------------------------------------------------------------------

namespace Data {
class Graph;
}

namespace Importer {

/**
 * \brief Contains object for the importer to work with.
 * Data manipulated by the importer during the import process:
 * - input: input data (stream)
 * - output: graph root object (graph), progress info (info handler)
 */
class ImporterContext
{

public:

	/**
	 * \brief Creates new context object containing provided references.
	 * \param[in] stream Input stream.
	 * \param[in] graph Graph root object.
	 * \param[in] importInfoHandler Import progress info handler.
	 */
	ImporterContext(
		QIODevice& stream,
		Data::Graph& graph,
		ImportInfoHandler& importInfoHandler
	);

	/**
	    * \brief Creates new context object containing provided references.
	    * \param[in] stream Input stream.
	    * \param[in] graph Graph root object.
	    * \param[in] importInfoHandler Import progress info handler.
	    * \param[in] filePath Input path to git repo.
	 */
	ImporterContext(
		QIODevice& stream,
		Data::Graph& graph,
		ImportInfoHandler& importInfoHandler,
		QString& filePath
	);

	/***/
	virtual ~ImporterContext( void );

	/**
	 * \brief \link ImporterContext::stream_ [getter] \endlink
	 */
	QIODevice& getStream( void );

	/**
	 * \brief \link ImporterContext::graph_ [getter] \endlink
	 */
	Data::Graph& getGraph( void );

	/**
	 * \brief \link ImporterContext::infoHandler_ [getter] \endlink
	 */
	ImportInfoHandler& getInfoHandler( void );

	/**
	 * \brief \link ImporterContext::filePath_ [getter] \endlink
	 */
	QString& getFilePath();


private:

	/**
	 * \brief Stream to read data from.
	 */
	QIODevice& stream_;

	/**
	 * \brief Graph to fill according to data read from the stream.
	 */
	Data::Graph& graph_;

	/**
	 * \brief Used by the importer to report status of the import (progress, messages, errors).
	 */
	ImportInfoHandler& infoHandler_;

	/**
	 * \brief Path to git repo
	 */
	QString filePath;

}; // class

} // namespace

#endif // Importer_ImporterContext_H
