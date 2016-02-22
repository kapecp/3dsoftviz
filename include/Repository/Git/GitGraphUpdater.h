#ifndef GITGRAPHUPDATER_H
#define GITGRAPHUPDATER_H

#include <QStringList>
#include <QMap>

namespace Data {
class Graph;
}
namespace Repository {
namespace Git {
class GitEvolutionGraph;
class GitFile;

class GitGraphUpdater
{
public:
	// constructor
	/**
	 * GitGraphUpdater( int currentVersion, Repository::Repository::Git::GitEvolutionGraph* evolutionGraph, Data::Graph* activeGraph )
	 * @brief Create git graph updater with set up current version of graph, evolution graph consists of evolution versions and active graph vizualized in main window
	 * @param currentVersion Current index of vizualized version of git repository
	 * @param evolutionGraph Graph which consists of all git repository versions
	 * @param activeGraph Currently vizualized graph
	 */
	GitGraphUpdater( int currentVersion, Repository::Git::GitEvolutionGraph* evolutionGraph, Data::Graph* activeGraph );

	~GitGraphUpdater();

	// public methods
    /**
     * void import()
     * @brief Import data to graph
     * @return Returns true if import was successfull, otherwise false.
     */
    bool import();

	/**
     * void previousVersion()
	 * @brief Sets up vizualized graph to the previous version.
	 */
	void previousVersion();

	/**
     * void nextVersion()
	 * @brief Sets up vizualized graph to the next version.
	 */
	void nextVersion();

	/**
     * void changeToVersion( int toVersion )
	 * @brief Sets up vizualized graph to the specific version
	 * @param toVersion Index of version to be vizualized
	 */
	void changeToVersion( int toVersion );

	/**
	 * Repository::Git::GitEvolutionGraph* getEvolutionGraph()
	 * @brief Getter of evolution graph
	 * @return Evolution graph
	 */
	Repository::Git::GitEvolutionGraph* getEvolutionGraph()
	{
		return this->evolutionGraph;
	}

	/**
	 * void setEvolutionGraph( Repository::Git::GitEvolutionGraph* evolutionGraph )
	 * @brief Setter of evolution graph
	 * @param evolutionGraph Evolution graph
	 */
	void setEvolutionGraph( Repository::Git::GitEvolutionGraph* evolutionGraph )
	{
		this->evolutionGraph = evolutionGraph;
	}

	/**
	 * Data::Graph* getActiveGraph()
	 * @brief Getter of current active graph
	 * @return Active graph
	 */
	Data::Graph* getActiveGraph()
	{
		return this->activeGraph;
	}

	/**
	 * void setActiveGraph( Data::Graph* activeGraph )
	 * @brief Setter of current active graph
	 * @param activeGraph Active graph
	 */
	void setActiveGraph( Data::Graph* activeGraph )
	{
		this->activeGraph = activeGraph;
	}

	/**
	 * int getCurrentVersion()
	 * @brief Getter of currently vizualized version index
	 * @return Index of currently vizualized version
	 */
	int getCurrentVersion()
	{
		return this->currentVersion;
	}

	void processRemovedNodes();

	/**
	 * void setCurrentVersion( int currentVersion )
	 * @brief Setter of currently vizualized version index
	 * @param currentVersion Index of currently vizualized version
	 */
	void setCurrentVersion( int currentVersion )
	{
		this->currentVersion = currentVersion;
	}

private:
	// private methods
	/**
	 * void addNodesToGraph( QStringList list )
	 * @brief Adds nodes from list of paths to the active graph
	 * @param list List of file paths
	 */
	void addNodesToGraph( QStringList list );

	/**
	 * void addEdgesToGraph( QStringList list )
	 * @brief Adds edges from list of paths to the active graph
	 * @param list List of file paths
	 */
	void addEdgesToGraph( QStringList list );

	/**
	 * void addAuthorEdgesToGraph( QString authorName, QList<Repository::Git::GitFile*> gitFiles )
	 * @brief Adds edge from author to nodes of current version, which have been added, modified or removed
	 * @param authorName Name of the version author
	 * @param gitFiles List of changed git files
	 */
    void addAuthorEdgesToGraph( QString authorName, QMap<QString, Repository::Git::GitFile*> gitFiles );

	/**
	 * void removeAuthorEdgesFromGraph( QString authorName, QList<Repository::Git::GitFile*> gitFiles )
	 * @brief Removes edge from author to nodes of previous version, which have been added, modified or removed
	 * @param authorName Name of the version author
	 * @param gitFiles List of changed git files
	 */
    void removeAuthorEdgesFromGraph( QString authorName, QMap<QString, Repository::Git::GitFile*> gitFiles );

	/**
	 * void removeNodesFromGraph( QStringList list )
	 * @brief Removes nodes from list of paths from the active graph
	 * @param list List of file paths
	 */
	void removeNodesFromGraph( QStringList list );

	/**
	 * void removeEdgesFromGraph( QStringList list )
	 * @brief Removes edges from list of paths from the active graph
	 * @param list List of file paths
	 */
	void removeEdgesFromGraph( QStringList list );

	/**
	 * void modifyNodesInGraph( QStringList list )
	 * @brief Modifies nodes from list of path in the active graph
	 * @param list List of file paths
	 */
	void modifyNodesInGraph( QStringList list );

	// private variables
	/**
	 * bool moreVersionChanged
	 * @brief Switch for modifying more than 1 version. Set true, if we modifying more than 1 version, otherwise false.
	 */
	bool moreVersionChanged;

	/**
	 * int currentVersion
	 * @brief Index of currently vizualized version in the active graph
	 */
	int currentVersion;

	/**
	 * Repository::Git::GitEvolutionGraph* evolutionGraph
	 * @brief Git evolution graph
	 */
	Repository::Git::GitEvolutionGraph* evolutionGraph;

	/**
	 * Data::Graph* activeGraph;
	 * @brief Current active graph
	 */
	Data::Graph* activeGraph;
}; // class
} // namespace
}
#endif // GITGRAPHUPDATER_H
