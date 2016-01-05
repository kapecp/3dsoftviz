#ifndef GITUTILS_H
#define GITUTILS_H

#include "GitLib/GitExport.h"

#include <QString>

namespace Repository {

namespace Git {

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

    /**
     * bool static changeCommit( QString commitId )
     * @brief Change Git repository folder to mirror state of specific commit
     * @param commitId Identifier of commit, to which repository will be changed.
     * @return Returns true on success, otherwise false.
     */
    bool static changeCommit( QString commitId, QString filePath );

}; // class
} // namespace
}
#endif // GITUTILS_H
