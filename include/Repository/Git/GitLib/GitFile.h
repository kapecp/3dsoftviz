#ifndef GITFILE_H
#define GITFILE_H

#include "GitLib/GitType.h"
#include "GitLib/GitExport.h"

#include <QString>
#include <QList>

namespace Repository {

namespace Git {
class GitFileDiffBlock;

class GITLIB_EXPORT GitFile
{
public:
	/**
	    * GitFile
	    * \brief Implicit constructor
	    */
	GitFile( void );

	~GitFile( void );

	/**
	    * \fn GitFile
	    * \brief Explicit constructor
	    * \param filename - name of Git file
	    * \param filepath - path to Git file
	    * \param type - type of Git file
	 */
	GitFile( QString filename, QString filepath, Repository::Git::GitType type );

	/**
	    * \fn getFilename
	    * \brief Returns filename
	 */
	QString getFilename()
	{
		return filename;
	}

	/**
	    * \fn getFilepath
	    * \brief Returns filepath
	 */
	QString getFilepath()
	{
		return filepath;
	}

	/**
	    * \fn getType
	    * \brief Returns type of Git file
	 */
	Repository::Git::GitType getType()
	{
		return type;
	}

	/**
	 * \fn getTypeAsString
	 * \brief Returns type of Git file as QString
	 */
	QString getTypeAsString();

	/**
	    * \fn getGitFileDiffBlocks
	    * \brief Returns list of diff blocks in the Git file
	 */
	QList<Repository::Git::GitFileDiffBlock*> getGitFileDiffBlocks()
	{
		return gitFileDiffBlocks;
	}

	/**
	    * \fn addGitFileDiffBlock
	    * \brief Append diff block to the list
	    * \param block - Block to be added
	 */
	void addGitFileDiffBlock( Repository::Git::GitFileDiffBlock* block )
	{
		this->gitFileDiffBlocks.append( block );
	}

	/**
	    * \fn setGitFileDiffBlocks
	    * \brief Set diff blocks in the Git file
	    * \param gitFileDiffBlocks Diff blocks from Git file to be set
	 */
	void setGitFileDiffBlocks( QList<Repository::Git::GitFileDiffBlock*> gitFileDiffBlocks )
	{
		this->gitFileDiffBlocks = gitFileDiffBlocks;
	}

	/**
	 * \fn printContent
	 * \brief Prints out all info about file.
	 */
	void printContent();

private:
	/**
	    * QString filename
	    * \brief Name of Git file
	    */
	QString filename;

	/**
	    * QString filepath
	    * \brief Path to the Git file
	    */
	QString filepath;

	/**
	    * GitFileType type
	    * \brief Type of the Git file in version. Value can be ADDED for newly added file, REMOVED for removed file and MODIFIED for modified file
	    */
	Repository::Git::GitType type;

	/**
	    * QList<GitFileDiffBlock> gitFileDiffBlocks
	    * \brief List of diff blocks from the Git file
	    */
	QList<Repository::Git::GitFileDiffBlock*> gitFileDiffBlocks;
}; // class
} // namespace
}
#endif // GITFILE_H
