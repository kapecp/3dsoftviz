#ifndef GITEVOLUTIONGRAPH_H
#define GITEVOLUTIONGRAPH_H

#include <QString>
#include <QList>

namespace Git {
class GitVersion;

class GitEvolutionGraph {
public:
    /**
     * Explicit default constructor
     * @brief Creates blank evolution graph
     */
    GitEvolutionGraph();

    /**
     * Explicit default constructor
     * @brief Creates evolution graph with set up filepath
     * @param filePath Path to the git repository on file system
     */
    GitEvolutionGraph( QString filePath );

    /**
     * void setVersions( QList<Git::GitVersion*> versions )
     * @brief Setter of list of versions
     * @param versions list of versions
     */
    void setVersions( QList<Git::GitVersion*> versions ) {
        this->versions = versions;
    }

    /**
     * QList<Git::GitVersion*> getVersions()
     * @brief Getter of list of versions
     * @return list of versions
     */
    QList<Git::GitVersion*> getVersions() {
        return this->versions;
    }

    /**
     * void setFilePath( QString filePath )
     * @brief Setter of path to the git repository on file system
     * @param filePath Path to the git repository on file system
     */
    void setFilePath( QString filePath ) {
        this->filePath = filePath;
    }

    /**
     * QString getFilePath()
     * @brief Getter of path to the git repository on file system
     * @return path to the git repository on file system
     */
    QString getFilePath() {
        return this->filePath;
    }

    /**
     * Git::GitVersion* getVersion( int position )
     * @brief Getter of specific version in list of versions
     * @param position Index of version in list of versions
     * @return Specific versions from list
     */
    Git::GitVersion* getVersion( int position ) {
        return this->versions.at( position );
    }

private:
    /**
     * QList<Git::GitVersion*> versions
     * @brief List of versions
     */
    QList<Git::GitVersion*> versions;

    /**
     * QString filePath
     * @brief Path to the git repository on file system
     */
    QString filePath;

}; // class
} // namespace
#endif // GITEVOLUTIONGRAPH_H
