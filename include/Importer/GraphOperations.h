#ifndef Importer_GraphOperations_H
#define Importer_GraphOperations_H
//-----------------------------------------------------------------------------
namespace Data {
class Graph;
class Type;
}
//-----------------------------------------------------------------------------

namespace Importer {

/**
 * \brief Provides operations performed on the graph object used by the importers.
 */
class GraphOperations
{

public:

	/**
	 * \brief Creates a new object over the specified graph.
	 * \param[in] graph Graph to perform the operations on.
	 */
	explicit GraphOperations(
		Data::Graph& graph
	);

	/***/
	virtual ~GraphOperations( void ) {}

	/**
	 * \brief Adds default edge and node type to the graph and returns the created objects.
	 * \param[out] edgeType Created default edge type.
	 * \param[out] nodeType Created default node type.
	 * \return ( = success)
	 */
	bool addDefaultTypes(
		Data::Type*& edgeType,
		Data::Type*& nodeType
	);

private:

	/**
	 * \brief Graph to perform the operations on.
	 */
	Data::Graph& graph_;

}; // class

} // namespace

#endif // Importer_GraphOperations_H
