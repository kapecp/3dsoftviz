#ifndef GITVERSION_H
#define GITVERSION_H

#include "GitLib/GitType.h"
#include "GitLib/GitExport.h"

#include <QString>
#include <QMap>

namespace Repository {

namespace Git {
class GitFile;

class GITLIB_EXPORT GitVersion
{
public:
	// constructors
	/**
	 * Explicit default constructor
	 * @brief Creates blank git version
	 */
	GitVersion();


	~GitVersion();

	/**
	 * Explicit constructor with set up commit ID
	 * @brief Creates git version with set up commit ID
	 * @param commitId Hash identifier of version (commit)
	 */
	GitVersion( QString commitId );

	// public methods
	/**
	 * QString getCommitId()
	 * @brief Getter of version identifier
	 * @return version identifier
	 */
	QString getCommitId()
	{
		return this->commitId;
	}

    /**
     * void setCommitId( QString commitId )
     * @brief Setter of version identifier
     * @param commitId version identifier
     */
    void setCommitId( QString commitId ) {
        this->commitId = commitId;
    }

	/**
	 * void setAuthor( QString author )
	 * @brief Setter of commit author
	 * @param author Name of commit author
	 */
	void setAuthor( QString author )
	{
		this->author = author;
	}

	/**
	 * QString getAuthor()
	 * @brief Getter of commit author
	 * @return Name of commit author
	 */
	QString getAuthor()
	{
		return this->author;
	}

	/**
	 * void setDate( QString date )
	 * @brief Setter of commit date
	 * @param date Commit date
	 */
	void setDate( QString date )
	{
		this->date = date;
	}

	/**
	 * QString getDate()
	 * @brief Getter of commit date
	 * @return commit date
	 */
	QString getDate()
	{
		return this->date;
	}

	/**
	 * QList<Repository::Git::GitFile*> getChangedFiles()
	 * @brief Getter of changed files in commit
	 * @return List of changed files in commmit
	 */
    QMap<QString, Repository::Git::GitFile*>* getChangedFiles()
	{
		return this->changedFiles;
	}

	/**
	 * void setChangedFiles( QList<Repository::Git::GitFile*> gitFiles )
	 * @brief Setter of changed files list
	 * @param gitFiles List of changed files
	 */
    void setChangedFiles( QMap<QString, Repository::Git::GitFile*>* gitFiles )
	{
		this->changedFiles = gitFiles;
	}

	/**
	 * void addChangedFile( Repository::Git::GitFile* file )
	 * @brief Append git file to list of git files
	 * @param file File to be appended to the list
	 */
    void addChangedFile( Repository::Git::GitFile* file );

	/**
	 * QList<Repository::Git::GitFile*> getGitFilesByType( Repository::Git::GitType gitType )
	 * @brief Returns list of files with the specifig type
	 * @param gitType Type of files
	 * @return list of files with specifig type
	 */
    QMap<QString, Repository::Git::GitFile*>* getGitFilesByType( Repository::Git::GitType gitType );

	/**
	 * QList<Repository::Git::GitFile*> getGitFilesByExtension( QString extension)
	 * @brief Return list of files with specific file extension
	 * @param extensions File extensions
	 * @return List of files with specific file extension
	 */
    QMap<QString, Repository::Git::GitFile*>* getGitFilesByExtension( QString extensions );

	/**
	 * QList<Repository::Git::GitFile*> getGitFilesByTypeAndExtension( QString extension, Repository::Git::GitType type )
	 * @brief Returns list of files with specific type and file extension
	 * @param extension File extension
	 * @param type Type of files
	 * @return List of files with specific type and file extension
	 */
    QMap<QString, Repository::Git::GitFile*>* getGitFilesByTypeAndExtension( QString extensions, Repository::Git::GitType type );

    /**
     * void setIsLoaded( bool isLoaded )
     * @brief Sets whether version was loaded from Lua
     * @param isLoaded Status of version loading
     */
    void setIsLoaded( bool isLoaded ) {
        this->isLoaded = isLoaded;
    }

    /**
     * bool getIsLoaded()
     * @brief Returns true if version was loaded from Lua, otherwise false
     * @return True if version was loaded from Lua, otherwise false
     */
    bool getIsLoaded() {
        return this->isLoaded;
    }

    /**
     * Repository::Git::GitFile* getGitFileByIdentifier( QString identifier )
     * @brief Get the git file by its identifier
     * @param identifier Identifier of file
     * @return Git file with specific file identifier
     */
    Repository::Git::GitFile* getGitFileByIdentifier( QString identifier );

	/**
	 * void printVersion()
	 * @brief Prints out all info about the version
	 */
	void printVersion();

private:
	//private variables
	/**
	 * QString commitId
	 * @brief Hash identifier of version (commit)
	 */
	QString commitId;

	/**
	 * QString author
	 * @brief Name of commit author
	 */
	QString author;

	/**
	 * QString date
	 * @brief Date of commit
	 */
	QString date;

    /**
     * bool isLoaded
     * @brief True if version was processed from Lua, otherwise false
     */
    bool isLoaded;

	/**
	 * QList<Repository::Git::GitFile*> changedFiles
	 * @brief List of changed files in commit
	 */
    QMap<QString, Repository::Git::GitFile*>* changedFiles;
}; // class
} // namespace
}
#endif // GITVERSION_H
