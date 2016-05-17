#ifndef GITLUAGRAPHANALYZER_H
#define GITLUAGRAPHANALYZER_H

#include <QMap>
#include <QString>

namespace Lua {
class LuaGraph;
}

namespace Repository {
namespace Git {
class GitEvolutionGraph;
class GitFunction;
class GitFile;

class GitLuaGraphAnalyzer {
public:
    /**
     * GitLuaGraphAnalyzer()
     * @brief Default constructor
     */
    GitLuaGraphAnalyzer();

    /**
     * GitLuaGraphAnalyzer( Lua::LuaGraph* luaGraph, Repository::Git::EvolutionGraph* evolutionGraph )
     * @brief Constructor of lua graph analyzer, where lua graph and evolution graph are defined.
     * @param luaGraph Reference to loaded Lua graph
     * @param evolutionGraph Reference to evolution graph
     */
    GitLuaGraphAnalyzer( Lua::LuaGraph* luaGraph, Repository::Git::GitEvolutionGraph* evolutionGraph );

    /**
     *  destructor
     */
    ~GitLuaGraphAnalyzer();

    /**
     * void analyze()
     * @brief Main function to analyze given Lua graph and create evolution graph representation of specific version
     */
    void analyze();

    void findFunctionRowsFromFile( Repository::Git::GitFile* file );

    void compareFilesAndSaveToEvolutionGraph( Repository::Git::GitFile* newFile, Repository::Git::GitFile* oldFile );

    void compareFunctions( Repository::Git::GitFunction* newFunction, Repository::Git::GitFunction* oldFunction, QMap<QString, bool>* functions, QString masterIdentifier, Repository::Git::GitFile* masterFile );

    int calculateRealResult( qlonglong luaId );

    void setLuaGraph( Lua::LuaGraph* luaGraph ) {
        this->luaGraph = luaGraph;
    }

    Lua::LuaGraph* getLuaGraph() {
        return this->luaGraph;
    }

    void setEvolutionGraph( Repository::Git::GitEvolutionGraph* evolutionGraph ) {
        this->evolutionGraph = evolutionGraph;
    }

    Repository::Git::GitEvolutionGraph* getEvolutionGraph() {
        return this->evolutionGraph;
    }

    void setVersionNumber( int versionNumber ) {
        this->versionNumber = versionNumber;
    }

    int getVersionNumber() {
        return this->versionNumber;
    }

    void addFunction( Repository::Git::GitFunction* function );

    void setFunctions( QMap<QString, Repository::Git::GitFunction*>* functions ) {
        this->functions = functions;
    }

    QMap<QString, Repository::Git::GitFunction*>* getFunctions() {
        return this->functions;
    }

private:

    bool intervalsIntersects( int firstStart, int firstEnd, int secondStart, int secondEnd );

    /**
     * Lua::LuaGraph* luaGraph
     * @brief Lua graph
     */
    Lua::LuaGraph* luaGraph;

    /**
     * Repository::Git::EvolutionGraph* evolutionGraph
     * @brief Git evolution graph
     */
    Repository::Git::GitEvolutionGraph* evolutionGraph;

    /**
     * int versionNumber
     * @brief Version number in evolution graph, which will be analyzed
     */
    int versionNumber;

    /**
     * QMap<QString, Repository::Git::GitFunction*>* functions
     * @brief Map of git functions as values where key is function's identifier
     */
    QMap<QString, Repository::Git::GitFunction*>* functions;
}; // class
} // Git namespace
} // Repository namespace
#endif // GITLUAGRAPHANALYZER_H
