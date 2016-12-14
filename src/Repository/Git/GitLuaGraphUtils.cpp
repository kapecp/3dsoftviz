#include "Repository/Git/GitLuaGraphUtils.h"

#include "LuaGraph/LuaGraph.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitMetaData.h"

#include <QDebug>

Repository::Git::GitLuaGraphUtils::GitLuaGraphUtils()
{
}

Repository::Git::GitLuaGraphUtils::GitLuaGraphUtils( Lua::LuaGraph* luaGraph, Repository::Git::GitEvolutionGraph* evolutionGraph ) : luaGraph( luaGraph ), evolutionGraph( evolutionGraph )
{

}

void Repository::Git::GitLuaGraphUtils::fillMetricsMetaData()
{

	for ( QMap<qlonglong, Lua::LuaNode*>::iterator iterator = this->luaGraph->getNodes()->begin(); iterator != this->luaGraph->getNodes()->end(); ++iterator ) {
		Lua::LuaNode* luaNode = iterator.value();

		if ( luaNode->getParams().getValue()["type"].asString() == "function" ) {
			int lines = static_cast<int>( luaNode->getParams().getValue()["metrics"].asTable()["LOC"].asTable()["lines"].asNumber() );
			int linesBlank = static_cast<int>( luaNode->getParams().getValue()["metrics"].asTable()["LOC"].asTable()["lines_blank"].asNumber() );
			int linesCode = static_cast<int>( luaNode->getParams().getValue()["metrics"].asTable()["LOC"].asTable()["lines_code"].asNumber() );
			int linesComment = static_cast<int>( luaNode->getParams().getValue()["metrics"].asTable()["LOC"].asTable()["lines_comment"].asNumber() );
			int linesNonEmpty = static_cast<int>( luaNode->getParams().getValue()["metrics"].asTable()["LOC"].asTable()["lines_nonempty"].asNumber() );

			this->evolutionGraph->getMetaDataFromIdentifier( luaNode->getIdentifier() )->insertMetrics( "lines", lines );
			this->evolutionGraph->getMetaDataFromIdentifier( luaNode->getIdentifier() )->insertMetrics( "linesBlank", linesBlank );
			this->evolutionGraph->getMetaDataFromIdentifier( luaNode->getIdentifier() )->insertMetrics( "linesCode", linesCode );
			this->evolutionGraph->getMetaDataFromIdentifier( luaNode->getIdentifier() )->insertMetrics( "linesComment", linesComment );
			this->evolutionGraph->getMetaDataFromIdentifier( luaNode->getIdentifier() )->insertMetrics( "linesNonEmpty", linesNonEmpty );
		}
	}
}

QMap<QString, int>* Repository::Git::GitLuaGraphUtils::getMetrics( QString identifier )
{
	QMap<QString, int>* resultMap = new QMap<QString, int>();

	Lua::LuaNode* luaNode = this->luaGraph->findNodeByLuaIdentifier( identifier );
	if ( luaNode ) {
		resultMap->insert( "lines", static_cast<int>( luaNode->getParams().getValue()["metrics"].asTable()["LOC"].asTable()["lines"].asNumber() ) );
		resultMap->insert( "linesBlank", static_cast<int>( luaNode->getParams().getValue()["metrics"].asTable()["LOC"].asTable()["lines_blank"].asNumber() ) );
		resultMap->insert( "linesCode", static_cast<int>( luaNode->getParams().getValue()["metrics"].asTable()["LOC"].asTable()["lines_code"].asNumber() ) );
		resultMap->insert( "linesComment", static_cast<int>( luaNode->getParams().getValue()["metrics"].asTable()["LOC"].asTable()["lines_comment"].asNumber() ) );
		resultMap->insert( "linesNonEmpty", static_cast<int>( luaNode->getParams().getValue()["metrics"].asTable()["LOC"].asTable()["lines_nonempty"].asNumber() ) );
	}

	return resultMap;
}

QMap<QString, int>* Repository::Git::GitLuaGraphUtils::compareMetrics( QMap<QString, int>* currentMetrics, QMap<QString, int>* previousMetrics )
{
	QMap<QString, int>* resultMap =  new QMap<QString, int>();

	resultMap->insert( "lines", currentMetrics->value( "lines" ) - previousMetrics->value( "lines" ) );
	resultMap->insert( "linesBlank", currentMetrics->value( "linesBlank" ) - previousMetrics->value( "linesBlank" ) );
	resultMap->insert( "linesCode", currentMetrics->value( "linesCode" ) - previousMetrics->value( "linesCode" ) );
	resultMap->insert( "linesComment", currentMetrics->value( "linesComment" ) - previousMetrics->value( "linesComment" ) );
	resultMap->insert( "linesNonEmpty", currentMetrics->value( "linesNonEmpty" ) - previousMetrics->value( "linesNonEmpty" ) );

	qDebug() << "diff lines" << resultMap->value( "lines" );
	qDebug() << "diff linesBlank" << resultMap->value( "linesBlank" );
	qDebug() << "diff linesCode" << resultMap->value( "linesCode" );
	qDebug() << "diff linesComment" << resultMap->value( "linesComment" );
	qDebug() << "diff linesNonEmpty" << resultMap->value( "linesNonEmpty" );

	return resultMap;
}
