#ifndef GITUTILS_H
#define GITUTILS_H

#include "GitLib/GitExport.h"

#include <QString>
#include <QList>
#include <QMap>

namespace Repository {

namespace Git {
class GitEvolutionGraph;
class GitFile;
class GitFunction;

class GITLIB_EXPORT GitUtils
{
public:

	/**
     * bool static changeDir( QString path )
	 * @brief Changes the current working directory
	 * @param path Path to the requested working directory
	 * @return Returns true if current working directory was successfully changed, otherwise returns false
	 */
    bool static changeDir( QString path );

	/**
     * bool static existGit( QString path )
	 * @brief Checks if chosen path contains .git folder
	 * @param path Path to the git repository stored on file system
	 * @return Returns true, if chosen folder contains .git folder, otherwise returns false
	 */
    bool static existGit( QString path );

	/**
     * QString static makeTmpFileFromCommand( QString command, QString filepath )
	 * @brief Creates temporary file from output of executed git command in system console
	 * @param command Command, which will be executed
	 * @param filepath Path to the git repository in file system
	 * @return path to the temporary file
	 */
    QString static makeTmpFileFromCommand( QString command, QString filepath );

    QString static getBooleanAsString( bool value );

    /**
     * bool static changeCommit( QString commitId )
     * @brief Change Git repository folder to mirror state of specific commit
     * @param commitId Identifier of commit, to which repository will be changed.
     * @return Returns true on success, otherwise false.
     */
    bool static changeCommit( QString commitId, QString filePath );

    QList<QString> static getModifiedLuaNodesFromVersion( Repository::Git::GitEvolutionGraph* evolutionGraph, int versionNumber );



    QList<QString> static getIdentifiersOfChangedItems( Repository::Git::GitFile* file, Repository::Git::GitFile* otherFile );
    QList<QString> static compareTwoFunctions( Repository::Git::GitFunction* function, Repository::Git::GitFunction* otherFunction, QMap<QString, bool>* functions, QString masterIdentifier );

private:

//    int static getLastModifiedVersionIndex( QString fileName, int fromIndex, Repository::Git::GitEvolutionGraph* evolutionGraph );

}; // class
} // namespace
}
#endif // GITUTILS_H
