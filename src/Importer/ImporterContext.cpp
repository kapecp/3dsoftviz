#include "Importer/ImporterContext.h"

#include "Data/Graph.h"

namespace Importer {

ImporterContext::ImporterContext(
	QIODevice& stream,
	Data::Graph& graph,
	ImportInfoHandler& infoHandler
) : stream_( stream ),
	graph_( graph ),
	infoHandler_( infoHandler )
{
	this->filePath = "";
}

ImporterContext::ImporterContext(
	QIODevice& stream,
	Data::Graph& graph,
	ImportInfoHandler& infoHandler,
	QString& filePath
) : stream_( stream ),
	graph_( graph ),
	infoHandler_( infoHandler ),
	filePath( filePath )
{
}

ImporterContext::~ImporterContext()
{

}

QIODevice& ImporterContext::getStream( void )
{
	return stream_;
}

Data::Graph& ImporterContext::getGraph( void )
{
	return graph_;
}

ImportInfoHandler& ImporterContext::getInfoHandler( void )
{
	return infoHandler_;
}

QString& ImporterContext::getFilePath()
{
	return filePath;
}

} // namespace
