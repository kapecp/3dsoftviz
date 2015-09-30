#ifndef GITFILE_H
#define GITFILE_H

#include <QString>

namespace Git {

class GitFileDiffBlock;
enum class GitFileType {
    ADDED,
    REMOVED,
    MODIFIED
};

class GitFile {
public:
    /**
        * GitFile
        * \brief Implicit constructor
        */
    GitFile( void ) {};

    /**
        * \fn GitFile
        * \brief Explicit constructor
        * \param filename - name of Git file
        * \param filepath - path to Git file
        * \param type - type of Git file
     */
    GitFile( QString filename, QString filepath, GitFileType type);

    /**
        * \fn getFilename
        * \brief Returns filename
     */
    QString getFilename() {
        return filename;
    }

    /**
        * \fn getFilepath
        * \brief Returns filepath
     */
    QString getFilepath() {
        return filepath;
    }

    /**
        * \fn getType
        * \brief Returns type of Git file
     */
    GitFileType getType() {
        return type;
    }

    /**
        * \fn getGitFileDiffBlocks
        * \brief Returns list of diff blocks in the Git file
     */
    QList<GitFileDiffBlock> getGitFileDiffBlocks() {
        return GitFileDiffBlocks;
    }

    /**
        * \fn setGitFileDiffBlocks
        * \brief Set diff blocks in the Git file
        * \param gitFileDiffBlocks Diff blocks from Git file to be set
     */
    void setGitFileDiffBlocks( QList<GitFileDiffBlock> gitFileDiffBlocks ) {
        this->gitFileDiffBlocks = gitFileDiffBlocks;
    }

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
    GitFileType type;

    /**
        * QList<GitFileDiffBlock> gitFileDiffBlocks
        * \brief List of diff blocks from the Git file
        */
    QList<GitFileDiffBlock> gitFileDiffBlocks;
};
}
#endif // GITFILE_H
