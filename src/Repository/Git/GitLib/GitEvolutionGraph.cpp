#include "Repository/Git/GitLib/GitEvolutionGraph.h"

#include <QDebug>
#include <QMapIterator>

Git::GitEvolutionGraph::GitEvolutionGraph()
    : versions(QList<Git::GitVersion*>()), removedFiles(QMap<QString, int>()), lifespan(0), filePath("")
{

}

Git::GitEvolutionGraph::~GitEvolutionGraph() {
    qDeleteAll(this->getVersions());
}

Git::GitEvolutionGraph::GitEvolutionGraph( QString filePath )
    : versions(QList<Git::GitVersion*>()), removedFiles(QMap<QString, int>()), lifespan(0), filePath( filePath )
{

}

void Git::GitEvolutionGraph::addRemovedFiles( QStringList list, int version )
{
	foreach ( QString item, list ) {
		if ( item != list.at( 0 ) ) {
			this->removedFiles.insert( item, version );
		}
	}
}

void Git::GitEvolutionGraph::removeRemovedFiles( QString name )
{
	this->removedFiles.remove( name );
}

void Git::GitEvolutionGraph::printRemovedFiles()
{
	QMapIterator<QString, int> iter( this->removedFiles );
	while ( iter.hasNext() ) {
		iter.next();
		qDebug() << iter.key() << iter.value();
	}
}
