#include "Git/GitFileDiffBlock.h"

#include "Git/GitFileDiffBlockLine.h"

#include <QDebug>

Git::GitFileDiffBlock::GitFileDiffBlock() {
    this->addCount = 0;
    this->addStart = 1;
    this->removeCount = 0;
    this->removeStart = 1;
    this->gitFileDiffBlockLines = QList<Git::GitFileDiffBlockLine*>();
}

void Git::GitFileDiffBlock::printDiffBlockLines() {
    foreach( Git::GitFileDiffBlockLine* line, getGitFileDiffBlockLines() ) {
        qDebug() << line->getLineNumber() << " " << line->getLineTypeAsString() << " " << line->getLine();
    }
}

void Git::GitFileDiffBlock::printCurrentInfo() {
    foreach( Git::GitFileDiffBlockLine* line, getGitFileDiffBlockLines() ) {
        if( line->getLineType() == Git::GitType::ADDED || line->getLineType() == Git::GitType::NONE ) {
            qDebug() << line->getLineNumber() << " " << line->getLineTypeAsString() << " " << line->getLine();
        }
    }
}

void Git::GitFileDiffBlock::printPreviousInfo() {
    foreach( Git::GitFileDiffBlockLine* line, getGitFileDiffBlockLines() ) {
        if( line->getLineType() == Git::GitType::REMOVED || line->getLineType() == Git::GitType::NONE ) {
            qDebug() << line->getLineNumber() << " " << line->getLineTypeAsString() << " " << line->getLine();
        }
    }
}

void Git::GitFileDiffBlock::printInfo() {
    qDebug() << "-" << removeStart << "," << removeCount << " +" << addStart << "," << addCount;
//    qDebug() << "************* STARA VERZIA ***************";
//    printPreviousInfo();
//    qDebug() << "************* NOVA VERZIA ***************";
    printCurrentInfo();
}
