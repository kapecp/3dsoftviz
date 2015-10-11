#ifndef GITFILEDIFFBLOCK_H
#define GITFILEDIFFBLOCK_H

#include <QList>

namespace Git {

class GitFileDiffBlockLine;

enum GitType;

class GitFileDiffBlock {
public:
    GitFileDiffBlock();

    void setAddStart( int addStart ) {
        this->addStart = addStart;
    }

    int getAddStart() {
        return this->addStart;
    }

    void setAddCount( int addCount ) {
        this->addCount = addCount;
    }

    int getAddCount() {
        return this->addCount;
    }

    void setRemoveStart( int removeStart ) {
        this->removeStart = removeStart;
    }

    int getRemoveStart() {
        return this->removeStart;
    }

    void setRemoveCount( int removeCount ) {
        this->removeCount = removeCount;
    }

    int getRemoveCount() {
        return this->removeCount;
    }

    void incAddCount() {
        this->addCount++;
    }

    QList<Git::GitFileDiffBlockLine*> getGitFileDiffBlockLines() {
        return this->gitFileDiffBlockLines;
    }

    void addGitFileDiffBlockLine( Git::GitFileDiffBlockLine* line ) {
        this->gitFileDiffBlockLines.append( line );
    }

    void printDiffBlockLines();

    void printDiffBlockLinesByType() {}

    void printInfo();

    void printCurrentInfo();
    void printPreviousInfo();

private:
    int addStart;
    int addCount;
    int removeStart;
    int removeCount;
    QList<Git::GitFileDiffBlockLine*> gitFileDiffBlockLines;
};
}
#endif // GITFILEDIFFBLOCK_H
