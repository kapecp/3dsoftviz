#include "GitLib/GitVersion.h"

#include "GitLib/GitFile.h"

#include <QDebug>

Repository::Git::GitVersion::GitVersion()
	: commitId( "" ), author( "" ), date( "" ), changedFiles( QList<Repository::Git::GitFile*>() )
{
	this->changedFiles =  QList<Repository::Git::GitFile*>();
}

Repository::Git::GitVersion::GitVersion( QString commitId )
	: commitId( commitId ), author( "" ), date( "" ), changedFiles( QList<Repository::Git::GitFile*>() )
{

}

Repository::Git::GitVersion::~GitVersion()
{
	qDeleteAll( this->changedFiles );
}

void Repository::Git::GitVersion::printVersion()
{
	// Vypis vseobecnych informacii o verzii
	qDebug() << "CommitId : " << this->getCommitId();
	qDebug() << "Author : " << this->getAuthor();
	qDebug() << "Date : " << this->getDate();
	qDebug() << "List size : " << this->getChangedFiles().size();

	// Pre kazdy subor v ramci verzii vypisem jeho nazov, systemovu cestu a typ
	foreach ( Repository::Git::GitFile* file , this->getChangedFiles() ) {
		qDebug() << file->getFilename() <<  " " << file->getFilepath() << " " << file->getTypeAsString();
	}
}

QList<Repository::Git::GitFile*> Repository::Git::GitVersion::getGitFilesByType( Repository::Git::GitType gitType )
{
	QList<Repository::Git::GitFile*> result = QList<Repository::Git::GitFile*>();

	// Pre kazdy git file skontrolujem, ci splna uvedany typ
	foreach ( Repository::Git::GitFile* gitFile, getChangedFiles() ) {

		// Ak typ suboru sa zhoduje z uvedanym typom, tak subor pridam do vysledku
		if ( gitFile->getType() == gitType ) {
			result.append( gitFile );
		}
	}

	return result;
}

QList<Repository::Git::GitFile*> Repository::Git::GitVersion::getGitFilesByExtension( QString extensions )
{
	QList<Repository::Git::GitFile*> result = QList<Repository::Git::GitFile*>();

	// Pre kazdy git file skontrolujem, ci ma jednu z extensions
	foreach ( Repository::Git::GitFile* gitFile, getChangedFiles() ) {

		// Zistim extension
		QString extension = gitFile->getFilename().mid( gitFile->getFilename().lastIndexOf( "." ) + 1 );

		// Ak sa extension nachadza v extensions, tak do vysledku pridam git file
		if ( extensions.indexOf( extension ) > -1 ) {
			result.append( gitFile );
		}
	}

	return result;
}

QList<Repository::Git::GitFile*> Repository::Git::GitVersion::getGitFilesByTypeAndExtension( QString extensions, Repository::Git::GitType type )
{
	QList<Repository::Git::GitFile*> result = QList<Repository::Git::GitFile*>();

	// Pre kazdy git file skontrolujem, ci ma jednu z extensions a zaroven uvedeny typ
	foreach ( Repository::Git::GitFile* gitFile, getChangedFiles() ) {

		// Ak sa zhoduje typ, tak skontrolujem extension
		if ( gitFile->getType() == type ) {

			// Zistim extension
			QString extension = gitFile->getFilename().mid( gitFile->getFilename().lastIndexOf( "." ) + 1 );

			// Ak sa extension nachadza v extensions, tak do vysledku pridam git file
			if ( extensions.indexOf( extension ) > -1 ) {
				result.append( gitFile );
			}
		}
	}

	return result;
}
