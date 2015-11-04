#include "Git/GitFileDiffBlockLine.h"

Git::GitFileDiffBlockLine::GitFileDiffBlockLine()
{

	// inicializacia premennych na defaultne hodnoty
	this->line = "";
	this->lineNumber = 0;
	this->lineType = Git::GitType::MODIFIED;
}

Git::GitFileDiffBlockLine::GitFileDiffBlockLine( QString line, int lineNumber, Git::GitType lineType )
	: line( line ), lineNumber( lineNumber ), lineType( lineType )
{

}

QString Git::GitFileDiffBlockLine::getLineTypeAsString()
{

	// zoberie typ riadku v diff bloku a vrati ho ako QString
	switch ( this->lineType ) {
		case Git::GitType::ADDED:
			return "Added";
		case Git::GitType::REMOVED:
			return "Removed";
		case Git::GitType::NONE:
			return "None";
		default:
			return "Wrong or Not set";
	}
}

