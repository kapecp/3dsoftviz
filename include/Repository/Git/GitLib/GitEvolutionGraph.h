#ifndef GITEVOLUTIONGRAPH_H
#define GITEVOLUTIONGRAPH_H

#include "GitLib/GitExport.h"
#include "GitLib/GitType.h"

#include <QString>
#include <QStringList>
#include <QList>
#include <QMap>

namespace Repository {
namespace Git {
class GitVersion;
class GitFile;
class GitMetaData;

class GITLIB_EXPORT GitEvolutionGraph
{
public:
	/**
	 * Explicit default constructor
	 * @brief Creates blank evolution graph
	 */
	GitEvolutionGraph();

	~GitEvolutionGraph();

	/**
	 * Explicit default constructor
	 * @brief Creates evolution graph with set up filepath
	 * @param filePath Path to the git repository on file system
	 */
	GitEvolutionGraph( QString filePath );

	/**
	 * void setVersions( QList<Repository::Git::GitVersion*> versions )
	 * @brief Setter of list of versions
	 * @param versions list of versions
	 */
	void setVersions( QList<Repository::Git::GitVersion*> versions )
	{
		this->versions = versions;
	}

	/**
	 * QList<Repository::Git::GitVersion*> getVersions()
	 * @brief Getter of list of versions
	 * @return list of versions
	 */
	QList<Repository::Git::GitVersion*> getVersions()
	{
		return this->versions;
	}

    void removeVersionAt( int position ) {
        this->versions.removeAt( position );
    }

	/**
	 * void setFilePath( QString filePath )
	 * @brief Setter of path to the git repository on file system
	 * @param filePath Path to the git repository on file system
	 */
	void setFilePath( QString filePath )
	{
		this->filePath = filePath;
	}

	/**
	 * QString getFilePath()
	 * @brief Getter of path to the git repository on file system
	 * @return path to the git repository on file system
	 */
	QString getFilePath()
	{
		return this->filePath;
	}

	/**
	 * Repository::Git::GitVersion* getVersion( int position )
	 * @brief Getter of specific version in list of versions
	 * @param position Index of version in list of versions
	 * @return Specific versions from list
	 */
	Repository::Git::GitVersion* getVersion( int position )
	{
		return this->versions.at( position );
	}

	/**
	 * int getLifespan()
	 * @brief Returns lifespan of deleted files in graph
	 * @return lifespan of deleted files in graph
	 */
	int getLifespan()
	{
		return this->lifespan;
	}

	/**
	 * void setLifespan( int lifespan )
	 * @brief Sets a lifespan of deleted files in graph
	 * @param lifespan Lifespan of deleted files in graph
	 */
	void setLifespan( int lifespan )
	{
		this->lifespan = lifespan;
	}

	/**
	 * QMap<QString, int> getRemovedFiles()
	 * @brief Returns map, where key is deleted file path in project and value version, in which was deleted
	 * @return map, where key is deleted file path in project and value version, in which was deleted
	 */
	QMap<QString, int> getRemovedFiles()
	{
		return this->removedFiles;
	}

	/**
	 * void addRemovedFiles( QStringList list, int version )
	 * @brief Adds removed files with version to map
	 * @param list List of files in path
	 * @param version Version where file was deleted
	 */
	void addRemovedFiles( QStringList list, int version );

    /**
     * void addLuaNodeMapping( QString identifier, int luaNodeId )
     * @brief Add mapping of lua node indetifier to actual lua node id
     * @param identifier Unique identifier of lua node
     * @param luaNodeId Unique id of current loaded lua node
     */
//    void addLuaNodesMapping( QString identifier, int luaNodeId );

    /**
     * QMap<QString, int> getLuaNodeMapping()
     * @brief Returns map, where key is unique identifier of lua node and value is unique id of current loaded lua node
     * @return map, where key is unique identifier of lua node and value is unique id of current loaded lua node
     */
//    QMap<QString, int> getLuaNodesMapping() {
//        return this->luaNodesMapping;
//    }

    /**
     * void addLuaEdgesMapping( QString identifier, int luaEdgeId )
     * @brief Add mapping of lua edge indetifier to actual lua edge id
     * @param identifier Unique identifier of lua edge
     * @param luaNodeId Unique id of current loaded lua edge
     */
//    void addLuaEdgesMapping( QString identifier, int luaEdgeeId );

    /**
     * QMap<QString, int> getLuaEdgesMapping()
     * @brief Returns map, where key is unique identifier of lua edge and value is unique id of current loaded lua edge
     * @return map, where key is unique identifier of lua edge and value is unique id of current loaded lua edge
     */
//    QMap<QString, int> getLuaEdgesMapping() {
//        return this->luaEdgesMapping;
//    }

    /**
     * void addLatestGitFileCallTree( QString identifier, Repository::Git::GitFile* gitFile )
     * @brief Add latest version of git file call tree with specific identifier
     * @param identifier Identifier of git file
     * @param gitFile Reference to git file which consists only with call tree
     */
//    void addLatestGitFileCallTree( QString identifier, Repository::Git::GitFile* gitFile ) {
//        this->latestGitFileCallTree.insert( identifier, gitFile );
//    }

    /**
     * QMap<QString, Repository::Git::GitFile*> getLatestGitFileCallTree()
     * @brief Returns map of latest version of stored git files, which consists only with file call tree
     * @return Map of latest version of stored git files, which consists only with file call tree
     */
//    QMap<QString, Repository::Git::GitFile*> getLatestGitFileCallTree() {
//        return this->latestGitFileCallTree;
//    }

	/**
	 * void removeRemovedFiles( QString name )
	 * @brief Removes file from map with specific key
	 * @param name Key of the file
	 */
	void removeRemovedFiles( QString name );

    /**
     * void addRemovedFile( QString identifier )
     * @brief Add removed file by its identifier
     * @param identifier Node identifier
     */
    void addChangedNodeOrEdge( QString identifier, Repository::Git::GitType type ) {
        this->changedNodesAndEdges->insert( identifier, type );
    }

    void removeChangedNodeOrEdge( QString identifier ) {
        this->changedNodesAndEdges->remove( identifier );
    }

    /**
     * int addNodeOccurence( QString identifier )
     * @brief Add node occurence in map by incrementing the stored value
     * @param identifier Key of node in map
     * @return Newly set value for identifier
     */
//    int addNodeOccurence( QString identifier );

    /**
     * int removeNodeOccurence( QString identifier )
     * @brief Remove node occurence in map by decrementing the stored value
     * @param identifier Key of node in map
     * @return Newly set value for identifier
     */
//    int removeNodeOccurence( QString identifier );

    /**
     * QMap<QString, int> getNodeOccurence()
     * @brief Getter of node occurence
     * @return Map of node occurence where key is node identifier and value is node occurence in graph
     */
//    QMap<QString, int> getNodeOccurence() {
//        return this->nodesOccurence;
//    }

    QMap<QString, Repository::Git::GitType>* getChangedNodesAndEdge() {
        return this->changedNodesAndEdges;
    }

//    void addLastVersionDiff( QString identifier, QString commitId ) {
//        this->lastVersionDiff.insert( identifier, commitId );
//    }

//    QMap<QString, QString> getLastVersionDiff() {
//        return this->lastVersionDiff;
//    }

//    void addLastFunctionInterval( QString identifier, QString interval ) {
//        this->lastFunctionInterval.insert( identifier, interval );
//    }

//    QMap<QString, QString> getLastFunctionInterval() {
//        return this->lastFunctionInterval;
//    }

    Repository::Git::GitMetaData* getMetaDataFromIdentifier( QString identifier );

//    QMap<QString, Repository::Git::GitMetaData*> getMetaData() {
//        return this->metaData;
//    }

	/**
	 * void printRemovedFiles()
	 * @brief Prints out all paths and versions of map
	 */
	void printRemovedFiles();

private:
	/**
	 * QList<Repository::Git::GitVersion*> versions
	 * @brief List of versions
	 */
	QList<Repository::Git::GitVersion*> versions;

	/**
	 * QMap<QString, int> removedFiles
	 * @brief Map of names of files which was deleted in Nth version
	 */
	QMap<QString, int> removedFiles;

    QMap<QString, Repository::Git::GitType>* changedNodesAndEdges;

    QMap<QString, Repository::Git::GitMetaData*> metaData;

	/**
	 * int lifespan
	 * @brief Lifespan of deleted files in graph
	 */
	int lifespan;

	/**
	 * QString filePath
	 * @brief Path to the git repository on file system
	 */
	QString filePath;

}; // class
} // namespace
}
#endif // GITEVOLUTIONGRAPH_H
