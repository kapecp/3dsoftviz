#include "GitLib/GitEvolutionGraph.h"

#include "GitLib/GitUtils.h"
#include "GitLib/GitMetaData.h"

#include <QDebug>
#include <QMapIterator>

Repository::Git::GitEvolutionGraph::GitEvolutionGraph()
//    : versions( QList<Repository::Git::GitVersion*>() ), removedFiles( QMap<QString, int>() ), lastVersionDiff( QMap<QString, QString>() ), removedNodesAndEdges( new QMap<QString, int>() ), lastFunctionInterval( QMap<QString, QString>() ), nodesOccurence( QMap<QString, int>() ), luaNodesMapping( QMap<QString, int>() ), latestGitFileCallTree( QMap<QString, Repository::Git::GitFile*>() ), lifespan( 0 ), filePath( "" )
    : versions( QList<Repository::Git::GitVersion*>() ), removedFiles( QMap<QString, int>() ), changedNodesAndEdges( new QMap<QString, Repository::Git::GitType>() ), metaData( QMap<QString, Repository::Git::GitMetaData*>() ), lifespan( 0 ), filePath( "" )
{

}

Repository::Git::GitEvolutionGraph::~GitEvolutionGraph()
{
//    qDebug() << "Destruktor GitEvolutionGraph";
    Git::GitUtils::changeCommit( QString( "origin" ), this->getFilePath() );
	qDeleteAll( this->getVersions() );
}

Repository::Git::GitEvolutionGraph::GitEvolutionGraph( QString filePath )
//    : versions( QList<Repository::Git::GitVersion*>() ), removedFiles( QMap<QString, int>() ), lastVersionDiff( QMap<QString, QString>() ), removedNodesAndEdges( new QMap<QString, int>() ), lastFunctionInterval( QMap<QString, QString>() ), nodesOccurence( QMap<QString, int>() ), luaNodesMapping( QMap<QString, int>() ), latestGitFileCallTree( QMap<QString, Repository::Git::GitFile*>() ), lifespan( 0 ), filePath( filePath )
    : versions( QList<Repository::Git::GitVersion*>() ), removedFiles( QMap<QString, int>() ), changedNodesAndEdges( new QMap<QString, Repository::Git::GitType>() ), metaData( QMap<QString, Repository::Git::GitMetaData*>() ), lifespan( 0 ), filePath( filePath )
{

}

void Repository::Git::GitEvolutionGraph::addRemovedFiles( QStringList list, int version )
{
	foreach ( QString item, list ) {
		if ( item != list.at( 0 ) ) {
			this->removedFiles.insert( item, version );
		}
	}
}

void Repository::Git::GitEvolutionGraph::removeRemovedFiles( QString name )
{
	this->removedFiles.remove( name );
}

void Repository::Git::GitEvolutionGraph::printRemovedFiles()
{
	QMapIterator<QString, int> iter( this->removedFiles );
	while ( iter.hasNext() ) {
		iter.next();
		qDebug() << iter.key() << iter.value();
	}
}

Repository::Git::GitMetaData* Repository::Git::GitEvolutionGraph::getMetaDataFromIdentifier( QString identifier ) {
    if( this->metaData.contains( identifier ) ) {
        return this->metaData.value( identifier );
    } else{
        Repository::Git::GitMetaData* metaData = new Repository::Git::GitMetaData();
        this->metaData.insert( identifier, metaData );
        return metaData;
    }
}
