#include "GitLib/GitEvolutionGraph.h"

#include <QDebug>
#include <QMapIterator>

Repository::Git::GitEvolutionGraph::GitEvolutionGraph()
	: versions( QList<Repository::Git::GitVersion*>() ), removedFiles( QMap<QString, int>() ), lifespan( 0 ), filePath( "" )
{

}

Repository::Git::GitEvolutionGraph::~GitEvolutionGraph()
{
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
