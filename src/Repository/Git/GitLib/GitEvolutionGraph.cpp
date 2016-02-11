#include "GitLib/GitEvolutionGraph.h"

#include "GitLib/GitUtils.h"

#include <QDebug>
#include <QMapIterator>

Repository::Git::GitEvolutionGraph::GitEvolutionGraph()
    : versions( QList<Repository::Git::GitVersion*>() ), removedFiles( QMap<QString, int>() ), luaNodesMapping( QMap<QString, int>() ), latestGitFileCallTree( QMap<QString, Repository::Git::GitFile*>() ), lifespan( 0 ), filePath( "" )
{

}

Repository::Git::GitEvolutionGraph::~GitEvolutionGraph()
{
    qDebug() << "Destruktor GitEvolutionGraph";
    Git::GitUtils::changeCommit( QString( "origin" ), this->getFilePath() );
	qDeleteAll( this->getVersions() );
}

Repository::Git::GitEvolutionGraph::GitEvolutionGraph( QString filePath )
	: versions( QList<Repository::Git::GitVersion*>() ), removedFiles( QMap<QString, int>() ), lifespan( 0 ), filePath( filePath )
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

void Repository::Git::GitEvolutionGraph::addLuaNodesMapping( QString identifier, int luaNodeId ) {
    if( this->luaNodesMapping.contains( identifier ) ) {
        int previousId = this->luaNodesMapping.find( identifier ).value();
        this->luaNodesMapping.insert( identifier, luaNodeId );
        int newId = this->luaNodesMapping.find( identifier ).value();

//        qDebug() << identifier << "from" << previousId << "to" << newId << "/" << luaNodeId;
    } else {
        this->luaNodesMapping.insert( identifier, luaNodeId );
        int storedId = this->luaNodesMapping.find( identifier ).value();

//        qDebug() << "Inserted" << identifier << "to" << storedId << "/" << luaNodeId;
    }

}

void Repository::Git::GitEvolutionGraph::addLuaEdgesMapping( QString identifier, int luaEdgeId ) {
    QStringList nodes = identifier.split("+");

    QString newIdentifier = nodes.at( 1 ) + "+" + nodes.at( 0 );

    if( this->luaEdgesMapping.contains( identifier ) ) {
        int previousId = this->luaEdgesMapping.find( identifier ).value();
        this->luaEdgesMapping.insert( identifier, luaEdgeId );
        int newId = this->luaEdgesMapping.find( identifier ).value();

//        qDebug() << identifier << "from" << previousId << "to" << newId << "/" << luaEdgeId;
    } else if( this->luaEdgesMapping.contains( newIdentifier ) ) {
        int previousId = this->luaEdgesMapping.find( identifier ).value();
        this->luaEdgesMapping.insert( identifier, luaEdgeId );
        int newId = this->luaEdgesMapping.find( identifier ).value();

//        qDebug() << identifier << "from" << previousId << "to" << newId << "/" << luaEdgeId;
    } else {
        this->luaEdgesMapping.insert( identifier, luaEdgeId );
        int storedId = this->luaEdgesMapping.find( identifier ).value();

//        qDebug() << "Inserted" << identifier << "to" << storedId << "/" << luaEdgeId;
    }
}
