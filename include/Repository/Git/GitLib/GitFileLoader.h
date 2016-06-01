#ifndef GITFILELOADER_H
#define GITFILELOADER_H

#include "GitLib/GitExport.h"

#include <QString>
#include <QList>
#include <QFile>

namespace Repository {

namespace Git {
class GitVersion;
class GitFile;

class GITLIB_EXPORT GitFileLoader
{
public:
	// Constructors
	/**
	 * Explicit default constructor
	 * @brief Creates blank git file loader
	 */
	GitFileLoader();

	/**
	 * Explicit constructor with set up path to the file
	 * @brief Create git file loader with set up path to the file
	 * @param filepath Path to the file
	 * @param extensions Extensions separeted by dot.
	 */
	GitFileLoader( QString filepath, QString extensions );

	// public methods
	/**
	 * Repository::Git::GitFile* getDiffInfo( Repository::Git::GitFile* gitFile, QString currentCommitId, QString oldCommitId )
	 * @brief Reads difference between current commit and previous one, creates git diff blocks for git file and saves it in the git file.
	 * @param gitFile Git file, which git difference will be stored in git file
	 * @param currentCommitId Hash identifier of commit, which I want to be a HEAD
	 * @param oldCommitId Hash identifier of commit which I want to be compared with(It should be older than current commit)
	 */
	void getDiffInfo( Repository::Git::GitFile* gitFile, QString currentCommitId, QString oldCommitId );

	/**
	 * QList<Repository::Git::GitVersion*> getDataAboutGit()
	 * @brief Reads log of git repository and creates git versions based on information from git log command.
	 * @return List of versions, which are part of the repository
	 */
	QList<Repository::Git::GitVersion*> getDataAboutGit();

private:
	// private methods
	/**
	 * bool changeDir( QString path )
	 * @brief Changes the current working directory
	 * @param path Path to the requested working directory
	 * @return Returns true if current working directory was successfully changed, otherwise returns false
	 */
//	bool changeDir( QString path );

	/**
	 * bool existGit( QString path )
	 * @brief Checks if chosen path contains .git folder
	 * @param path Path to the git repository stored on file system
	 * @return Returns true, if chosen folder contains .git folder, otherwise returns false
	 */
//	bool existGit( QString path );

	/**
	 * Repository::Git::GitFile* readGitShowFile( QString tmpFile, Repository::Git::GitFile* gitFile )
	 * @brief Reads info of git show command for newly added file to the repository and stores the information into the git file
	 * @param tmpFile Temporary file where the data are stored from git show command
	 * @param gitFile Git file where the data will be stored
	 */
	void readGitShowFile( QString tmpFile, Repository::Git::GitFile* gitFile );

	/**
	 * void readGitDiffFile( QString tmpFile, Repository::Git::GitFile* gitFile )
	 * @brief Reads info of git diff command for modified or removed file in the repository and stores informaction into the git file
	 * @param tmpFile Temporary file where the data are stored from git diff command
	 * @param gitFile Git file where the data will be stored
	 */
	void readGitDiffFile( QString tmpFile, Repository::Git::GitFile* gitFile );

	/**
	 * QString makeTmpFileFromCommand( QString command, QString filepath )
	 * @brief Creates temporary file from output of executed git command in system console
	 * @param command Command, which will be executed
	 * @param filepath Path to the git repository in file system
	 * @return path to the temporary file
	 */
//	QString makeTmpFileFromCommand( QString command, QString filepath );

	// private variables
	/**
	 * String filePath
	 * @brief Path to the git repository in file system
	 */
	QString filePath;

	/**
	 * QString extensions
	 * @brief Extensions separeted by dot.
	 */
	QString extensions;

}; // class
} // namespace
}
#endif // GITFILELOADER_H
