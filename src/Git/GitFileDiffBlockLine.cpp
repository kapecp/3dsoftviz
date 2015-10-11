#include "Git/GitFileDiffBlockLine.h"

Git::GitFileDiffBlockLine::GitFileDiffBlockLine() {
    this->line = "";
    this->lineNumber = 0;
}

Git::GitFileDiffBlockLine::GitFileDiffBlockLine( QString line, int lineNumber, Git::GitType lineType )
    : line( line ), lineNumber( lineNumber ), lineType( lineType ) {

}

QString Git::GitFileDiffBlockLine::getLineTypeAsString() {
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

