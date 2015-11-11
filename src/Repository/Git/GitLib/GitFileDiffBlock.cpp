#include "Repository/Git/GitLib/GitFileDiffBlock.h"

#include "Repository/Git/GitLib/GitFileDiffBlockLine.h"

#include <QDebug>

Git::GitFileDiffBlock::GitFileDiffBlock()
    : gitFileDiffBlockLines(QList<Git::GitFileDiffBlockLine*>()), addCount(0), addStart(1), removeCount(0), removeStart(0)
{
}

Git::GitFileDiffBlock::~GitFileDiffBlock() {
    qDeleteAll(this->gitFileDiffBlockLines);
}

void Git::GitFileDiffBlock::printDiffBlockLines()
{
	// Vsetky diff block lines vypis do konzoly
	foreach ( Git::GitFileDiffBlockLine* line, getGitFileDiffBlockLines() ) {
		qDebug() << line->getLineNumber() << " " << line->getLineTypeAsString() << " " << line->getLine();
	}
}

void Git::GitFileDiffBlock::printCurrentInfo()
{
	// Pre vsetky diff block lines vypis do konzoly tie, ktore predstavuju sucasnu verziu diff blocku
	foreach ( Git::GitFileDiffBlockLine* line, getGitFileDiffBlockLines() ) {

		// Iba diff block lines, ktore maju typ ADDED a NONE predstavuju sucasnu verziu diff blocku
		if ( line->getLineType() == Git::GitType::ADDED || line->getLineType() == Git::GitType::NONE ) {
			qDebug() << line->getLineNumber() << " " << line->getLineTypeAsString() << " " << line->getLine();
		}
	}
}

void Git::GitFileDiffBlock::printPreviousInfo()
{
	// Pre vsetky diff block lines vypis do konzoly tie, ktore predstavuju predchadzajucu verziu diff blocku
	foreach ( Git::GitFileDiffBlockLine* line, getGitFileDiffBlockLines() ) {

		// Iba diff block lines, ktore maju typ REMOVED a NONE predstavuju predchadzajucu verziu diff blocku
		if ( line->getLineType() == Git::GitType::REMOVED || line->getLineType() == Git::GitType::NONE ) {
			qDebug() << line->getLineNumber() << " " << line->getLineTypeAsString() << " " << line->getLine();
		}
	}
}

void Git::GitFileDiffBlock::printInfo()
{
	qDebug() << "-" << removeStart << "," << removeCount << " +" << addStart << "," << addCount;
	printDiffBlockLines();
}
