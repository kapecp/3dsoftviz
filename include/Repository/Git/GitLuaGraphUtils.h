#ifndef GITLUAGRAPHUTILS_H
#define GITLUAGRAPHUTILS_H

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

class GitLuaGraphUtils
{
public:
	GitLuaGraphUtils();

	GitLuaGraphUtils( Lua::LuaGraph* luaGraph, Repository::Git::GitEvolutionGraph* evolutionGraph );

	void fillMetricsMetaData();

	QMap<QString, int>* getMetrics( QString identifier );

	static QMap<QString, int>* compareMetrics( QMap<QString, int>* currentMetrics, QMap<QString, int>* previousMetrics );
private:

	Lua::LuaGraph* luaGraph;
	Repository::Git::GitEvolutionGraph* evolutionGraph;

}; // class
} // Git namespace
} // Repository namespace
#endif // GITLUAGRAPHUTILS_H
