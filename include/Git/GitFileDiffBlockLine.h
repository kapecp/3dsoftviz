#ifndef GITFILEDIFFBLOCKLINE_H
#define GITFILEDIFFBLOCKLINE_H

#include <QString>

namespace Git {

enum GitType;

class GitFileDiffBlockLine {
public:
    GitFileDiffBlockLine();

    GitFileDiffBlockLine( QString line, int lineNumber, Git::GitType lineType);

    void setLine( QString line ) {
        this->line = line;
    }

    QString getLine() {
        return this->line;
    }

    void setLineNumber( int lineNumber ) {
        this->lineNumber = lineNumber;
    }

    int getLineNumber() {
        return this->lineNumber;
    }

    void setLineType( Git::GitType lineType ) {
        this->lineType = lineType;
    }

    Git::GitType getLineType() {
        return this->lineType;
    }

    QString getLineTypeAsString();


private:
    QString line;
    int lineNumber;
    Git::GitType lineType;
};
}
#endif // GITFILEDIFFBLOCKLINE_H
