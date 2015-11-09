#ifndef GITVERSION_H
#define GITVERSION_H

#include "Git/GitType.h"

#include <QString>
#include <QList>

namespace Git {
class GitFile;

class GitVersion
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
	 * QList<Git::GitFile*> getChangedFiles()
	 * @brief Getter of changed files in commit
	 * @return List of changed files in commmit
	 */
	QList<Git::GitFile*> getChangedFiles()
	{
		return this->changedFiles;
	}

	/**
	 * void setChangedFiles( QList<Git::GitFile*> gitFiles )
	 * @brief Setter of changed files list
	 * @param gitFiles List of changed files
	 */
	void setChangedFiles( QList<Git::GitFile*> gitFiles )
	{
		this->changedFiles = gitFiles;
	}

	/**
	 * void addChangedFile( Git::GitFile* file )
	 * @brief Append git file to list of git files
	 * @param file File to be appended to the list
	 */
	void addChangedFile( Git::GitFile* file )
	{
		this->changedFiles.append( file );
	}

	/**
	 * QList<Git::GitFile*> getGitFilesByType( Git::GitType gitType )
	 * @brief Returns list of files with the specifig type
	 * @param gitType Type of files
	 * @return list of files with specifig type
	 */
	QList<Git::GitFile*> getGitFilesByType( Git::GitType gitType );

	/**
	 * QList<Git::GitFile*> getGitFilesByExtension( QString extension)
	 * @brief Return list of files with specific file extension
	 * @param extensions File extensions
	 * @return List of files with specific file extension
	 */
	QList<Git::GitFile*> getGitFilesByExtension( QString extensions );

	/**
	 * QList<Git::GitFile*> getGitFilesByTypeAndExtension( QString extension, Git::GitType type )
	 * @brief Returns list of files with specific type and file extension
	 * @param extension File extension
	 * @param type Type of files
	 * @return List of files with specific type and file extension
	 */
	QList<Git::GitFile*> getGitFilesByTypeAndExtension( QString extensions, Git::GitType type );

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
	 * QList<Git::GitFile*> changedFiles
	 * @brief List of changed files in commit
	 */
	QList<Git::GitFile*> changedFiles;
}; // class
} // namespace
#endif // GITVERSION_H
