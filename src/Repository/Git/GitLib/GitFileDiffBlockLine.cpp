#include "GitLib/GitFileDiffBlockLine.h"

Repository::Git::GitFileDiffBlockLine::GitFileDiffBlockLine()
	: line( "" ), lineNumber( 0 ), lineType( Repository::Git::GitType::MODIFIED )
{

}

Repository::Git::GitFileDiffBlockLine::GitFileDiffBlockLine( QString line, int lineNumber, Repository::Git::GitType lineType )
	: line( line ), lineNumber( lineNumber ), lineType( lineType )
{

}

QString Repository::Git::GitFileDiffBlockLine::getLineTypeAsString()
{

	// zoberie typ riadku v diff bloku a vrati ho ako QString
	switch ( this->lineType ) {
		case Repository::Git::GitType::ADDED:
			return "Added";
		case Repository::Git::GitType::REMOVED:
			return "Removed";
		case Repository::Git::GitType::MODIFIED:
			return "Modified";
		case Repository::Git::GitType::NONE:
			return "None";
		default:
			return "Wrong or Not set";
	}
}

