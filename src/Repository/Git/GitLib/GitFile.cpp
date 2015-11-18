#include "GitLib/GitFile.h"
#include "GitLib/GitFileDiffBlock.h"

#include <QDebug>

Repository::Git::GitFile::GitFile()
	: filename( "" ), filepath( "" ), type( Repository::Git::GitType::NONE ), gitFileDiffBlocks( QList<Repository::Git::GitFileDiffBlock*>() )
{

}


Repository::Git::GitFile::~GitFile()
{
	qDeleteAll( this->gitFileDiffBlocks );
}

Repository::Git::GitFile::GitFile( QString filename, QString filepath, GitType type ) : filename( filename ), filepath( filepath ), type( type ),
	gitFileDiffBlocks( QList<Repository::Git::GitFileDiffBlock*>() )
{

}

QString Repository::Git::GitFile::getTypeAsString()
{

	// zoberie typ suboru a vrati ho ako QString
	switch ( this->getType() ) {
		case Repository::Git::GitType::ADDED:
			return "Added";
		case Repository::Git::GitType::MODIFIED:
			return "Modified";
		case Repository::Git::GitType::REMOVED:
			return "Removed";
		case Repository::Git::GitType::NONE:
			return "NONE";
		default:
			return "Wrong or Not set";
	}
}

void Repository::Git::GitFile::printContent()
{

	// vypise informacie o subore
	qDebug() << "filename: " << getFilename();
	qDebug() << "filepath: " << getFilepath();
	qDebug() << "type: " << getTypeAsString();

	// pre kazdy diff blok vypis informacie o danom bloku
	foreach ( GitFileDiffBlock* block, getGitFileDiffBlocks() ) {
		block->printInfo();
	}
}
