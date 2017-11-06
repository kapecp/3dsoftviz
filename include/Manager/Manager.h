/*!
 * Manager.h
 * Projekt 3DVisual
 */
#ifndef Manager_MANAGER_DEF
#define Manager_MANAGER_DEF 1

#include <QMap>
#include <QString>

#include <QtXml/QDomElement>
#include <QFile>
#include <qfileinfo.h>

#include <memory>

namespace Layout {
class LayoutThread;
}

namespace Model {
class DB;
}
namespace Repository {
namespace Git {
class GitEvolutionGraph;
class GitFile;
}
}

namespace Data {
class Graph;
}

namespace Manager {
/**
 * \class Manager
     * \brief Manager provides functionality to manage graphs (loading, creating, holding, editing and deleting).
     *
     * Class is implemented as singleton.
     *
     * \author Pavol Perdik
     * \date 25.4.2010
     */
class GraphManager
{
public:
	~GraphManager();

	/**
	 * \fn inline getDB
	 * \brief Return db
	 */
	::Model::DB* getDB()
	{
		return db;
	}

	/**
	 * \fn getAvaliableGraphs
	 * \brief Returns map of availiable graphs (partialy implemented, nowadays, it returns only one graph - active graph).
	 */
	QMap<qlonglong, Data::Graph*> getAvaliableGraphs()
	{
		return graphs;
	}

	/**
	 * \fn loadGraph
	 * \brief Loads graph from GraphML file.
	 */
	Data::Graph* loadGraph( QString filepath );

	/**
	 * \fn createNewGraph
	 * \brief Creates new graph.
	 */
	Data::Graph* createNewGraph( QString name, bool fixedPositions = false );


	/**
	 * \fn createNewMatrixGraph
	 * \brief Creates new graph for matrix representation.
	 */
	Data::Graph* createNewMatrixGraph( QString name );

	/**
	 * \fn loadGraphFromGit
	 * \brief Loads graph from git repo.
	 */
	bool loadEvolutionGraphFromGit( QString filepath );

	/**
	 * \fn loadGraphFromDB
	 * \brief Loads selected graph from database.
	 */
	Data::Graph* loadGraphFromDB( qlonglong graphID, qlonglong layoutID );

	Data::Graph* importEvolutionGraph( QString filepath );

	/**
	 * \fn simpleGraph
	 * \brief Creates simple triangle graph. Method was created as example of using API for creating graphs.
	 */
	Data::Graph* simpleGraph();

	/**
	 * @author Autor:Dávid Durčák
	 * \fn saveActiveGraphToDB
	 * \brief Saves active graph to DB.
	*/
	void saveActiveGraphToDB();

	/**
	 * @author Autor:Dávid Durčák
	 * \fn saveActiveLayoutToDB
	 * \brief Saves active layout to DB and if graph is not, save it also
	 * @param layoutName Name of laoyt in database
	 */
	void saveActiveLayoutToDB( const QString layoutName );

	/**
	 * \fn exportGraph
	 * \brief Exports graph into file (not yet implemented).
	 */
	void exportGraph( Data::Graph* graph, QString filepath );

	/**
	 * \fn createGraph
	 * \brief Creates empty graph, puts it into the working graphs and returns it.
	 */
	Data::Graph* createGraph( QString graphname );

	/**
	 * \fn removeGraph
	 * \brief Removes graph from working graphs and also from DB (if connected).
	 */
	void removeGraph( Data::Graph* graph );

	/**
	 * \fn closeGraph
	 * \brief Removes graph from working graphs. Do NOT remove it from DB.
	 */
	void closeGraph( Data::Graph* graph );

	/**
	 * \fn getActiveGraph
	 * \brief Returns active graphs. (imeplemnted as workaround for working with only one graph for now).
	 */
	Data::Graph* getActiveGraph()
	{
		return activeGraph.get();
	}

	/**
	 * Repository::Git::GitEvolutionGraph* getActiveEvolutionGraph()
	 * @brief Returns active evolution graph
	 * @return active evolution graph
	 */
	Repository::Git::GitEvolutionGraph* getActiveEvolutionGraph()
	{
		return this->activeEvolutionGraph;
	}

	/**
	 * \fn getInstance
	 * \brief Returns instance of class.
	 */
	static Manager::GraphManager* getInstance();

	/**
	 * @brief showProgressBar
	 */
	void showProgressBar();

	/**
	 * @brief setProgressBarValue
	 * @param value
	 */
	void setProgressBarValue( int value );

	/**
	 * @brief closeProgressBar
	 */
	void closeProgressBar();

	/**
	 * bool nextVersion( Layout::LayoutThread* layout )
	 * @brief Update graph to the next version
	 * @param layout Layout
	 * @return Returns true, if update was successful, otherwise false.
	 */
	bool nextVersion( Layout::LayoutThread* layout );

	/**
	 * bool previousVersion( Layout::LayoutThread* layout )
	 * @brief Update graph to the previous version
	 * @param layout Layout
	 * @return Returns true, if update was successful, otherwise false.
	 */
	bool previousVersion( Layout::LayoutThread* layout );

	/**
	 * bool changeToVersion( Layout::LayoutThread* layout, int toVersion )
	 * @brief Changes version to the selected index.
	 * @param layout Layout
	 * @param toVersion Selected version index
	 * @return
	 */
	bool changeToVersion( Layout::LayoutThread* layout, int toVersion );

	/**
	 * void getDiffInfo( QString path, int version )
	 * @brief Finds git file for specific path and sets diff info to the file
	 * @param path Project path of the file
	 * @param version Index of currently vizualized version in graph
	 */
	void getDiffInfo( QString path, int version );

private:
	/**
	*  \fn private runTestCase(qint32 action)
	*  \brief Runs one of predefined Graph tests
	*  \param action  code of the test
	*/
	void runTestCase( qint32 action );

	/**
	*  \fn private  emptyGraph
	*  \brief returns empty graph
	*  \deprecated
	*  \return Data::Graph *
	*/
	Data::Graph* emptyGraph( QString name = "simple" );

	/**
	*  \fn private constructor GraphManager
	*  \brief private constructor
	*  \param  app
	*/
	GraphManager();

	/**
	*  Manager::GraphManager * manager
	*  \brief singleton object
	*/
	static GraphManager* manager;

	/**
	*  QMap<qlonglong,Data::Graph*> graphs
	*  \brief map of working graphs
	*/
	QMap<qlonglong, Data::Graph*> graphs;

	/**
	*  Model::DB * db
	*  \brief connection to DB
	*/
	::Model::DB* db;


	/**
	*  Data::Graph * activeGraph
	*  \brief active graph
	*/
	std::shared_ptr<Data::Graph> activeGraph;

	/**
	 * @brief if no database connection find, set to true
	 */
	bool noDatabaseFind;

	/**
	 * Repository::Git::GitEvolutionGraph* activeEvolutionGraph
	 * @brief Active evolution graph
	 */
	Repository::Git::GitEvolutionGraph* activeEvolutionGraph;
}; // class
} // namespace

#endif
