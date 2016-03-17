#include "GitLib/GitVersion.h"

#include "GitLib/GitFile.h"

#include <QDebug>

Repository::Git::GitVersion::GitVersion()
    : commitId( "" ), author( "" ), date( "" ), isLoaded( false ), changedFiles( new QMap<QString, Repository::Git::GitFile*>() )
{

}

Repository::Git::GitVersion::GitVersion( QString commitId )
    : commitId( commitId ), author( "" ), date( "" ), isLoaded( false ), changedFiles( new QMap<QString, Repository::Git::GitFile*>() )
{

}

Repository::Git::GitVersion::~GitVersion()
{
    qDeleteAll( *this->changedFiles );
//    delete this->changedFiles;
}

void Repository::Git::GitVersion::printVersion()
{
	// Vypis vseobecnych informacii o verzii
	qDebug() << "CommitId : " << this->getCommitId();
	qDebug() << "Author : " << this->getAuthor();
	qDebug() << "Date : " << this->getDate();
    qDebug() << "List size : " << this->getChangedFiles()->size();

	// Pre kazdy subor v ramci verzii vypisem jeho nazov, systemovu cestu a typ
    for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = this->changedFiles->begin(); iterator != this->changedFiles->end(); ++iterator ) {
        Repository::Git::GitFile* file = iterator.value();
		qDebug() << file->getFilename() <<  " " << file->getFilepath() << " " << file->getTypeAsString();
	}
}

QMap<QString, Repository::Git::GitFile*>* Repository::Git::GitVersion::getGitFilesByType( Repository::Git::GitType gitType )
{
    QMap<QString, Repository::Git::GitFile*>* result = new QMap<QString, Repository::Git::GitFile*>();

	// Pre kazdy git file skontrolujem, ci splna uvedany typ
    for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = this->changedFiles->begin(); iterator != this->changedFiles->end(); ++iterator ) {
        Repository::Git::GitFile* gitFile = iterator.value();

		// Ak typ suboru sa zhoduje z uvedanym typom, tak subor pridam do vysledku
		if ( gitFile->getType() == gitType ) {
            result->insert( gitFile->getIdentifier(), gitFile );
		}
	}

	return result;
}

QMap<QString, Repository::Git::GitFile*>* Repository::Git::GitVersion::getGitFilesByExtension( QString extensions )
{
    QMap<QString, Repository::Git::GitFile*>* result = new QMap<QString, Repository::Git::GitFile*>();

	// Pre kazdy git file skontrolujem, ci ma jednu z extensions
    for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = this->changedFiles->begin(); iterator != this->changedFiles->end(); ++iterator ) {
        Repository::Git::GitFile* gitFile = iterator.value();

		// Zistim extension
		QString extension = gitFile->getFilename().mid( gitFile->getFilename().lastIndexOf( "." ) + 1 );

		// Ak sa extension nachadza v extensions, tak do vysledku pridam git file
		if ( extensions.indexOf( extension ) > -1 ) {
            result->insert( gitFile->getIdentifier(), gitFile );
		}
	}

	return result;
}

QMap<QString, Repository::Git::GitFile*>* Repository::Git::GitVersion::getGitFilesByTypeAndExtension( QString extensions, Repository::Git::GitType type )
{
    QMap<QString, Repository::Git::GitFile*>* result = new QMap<QString, Repository::Git::GitFile*>();

	// Pre kazdy git file skontrolujem, ci ma jednu z extensions a zaroven uvedeny typ
    for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = this->changedFiles->begin(); iterator != this->changedFiles->end(); ++iterator ) {
        Repository::Git::GitFile* gitFile = iterator.value();

		// Ak sa zhoduje typ, tak skontrolujem extension
		if ( gitFile->getType() == type ) {

			// Zistim extension
			QString extension = gitFile->getFilename().mid( gitFile->getFilename().lastIndexOf( "." ) + 1 );

			// Ak sa extension nachadza v extensions, tak do vysledku pridam git file
			if ( extensions.indexOf( extension ) > -1 ) {
                result->insert( gitFile->getIdentifier(), gitFile );
			}
		}
	}

	return result;
}

Repository::Git::GitFile* Repository::Git::GitVersion::getGitFileByIdentifier( QString identifier ) {
    for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = this->changedFiles->begin(); iterator != this->changedFiles->end(); ++iterator ) {
        Repository::Git::GitFile* file = iterator.value();
        if( !QString::compare( identifier, file->getIdentifier() ) ){
            return file;
        }
    }

    return nullptr;
}

void Repository::Git::GitVersion::addChangedFile( Repository::Git::GitFile *file ) {
    this->changedFiles->insert( file->getIdentifier(), file );
}
