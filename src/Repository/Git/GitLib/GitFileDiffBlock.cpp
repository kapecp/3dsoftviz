#include "GitLib/GitFileDiffBlock.h"

#include "GitLib/GitFileDiffBlockLine.h"

#include <QDebug>

Repository::Git::GitFileDiffBlock::GitFileDiffBlock()
	: addStart( 1 ), addCount( 0 ), removeStart( 0 ), removeCount( 0 ) , gitFileDiffBlockLines( QList<Repository::Git::GitFileDiffBlockLine*>() )
{
}

Repository::Git::GitFileDiffBlock::~GitFileDiffBlock()
{
	qDeleteAll( this->gitFileDiffBlockLines );
}

void Repository::Git::GitFileDiffBlock::printDiffBlockLines()
{
	// Vsetky diff block lines vypis do konzoly
	foreach ( Repository::Git::GitFileDiffBlockLine* line, getGitFileDiffBlockLines() ) {
		qDebug() << line->getLineNumber() << " " << line->getLineTypeAsString() << " " << line->getLine();
	}
}

void Repository::Git::GitFileDiffBlock::printCurrentInfo()
{
	// Pre vsetky diff block lines vypis do konzoly tie, ktore predstavuju sucasnu verziu diff blocku
	foreach ( Repository::Git::GitFileDiffBlockLine* line, getGitFileDiffBlockLines() ) {

		// Iba diff block lines, ktore maju typ ADDED a NONE predstavuju sucasnu verziu diff blocku
		if ( line->getLineType() == Repository::Git::GitType::ADDED || line->getLineType() == Repository::Git::GitType::NONE ) {
			qDebug() << line->getLineNumber() << " " << line->getLineTypeAsString() << " " << line->getLine();
		}
	}
}

void Repository::Git::GitFileDiffBlock::printPreviousInfo()
{
	// Pre vsetky diff block lines vypis do konzoly tie, ktore predstavuju predchadzajucu verziu diff blocku
	foreach ( Repository::Git::GitFileDiffBlockLine* line, getGitFileDiffBlockLines() ) {

		// Iba diff block lines, ktore maju typ REMOVED a NONE predstavuju predchadzajucu verziu diff blocku
		if ( line->getLineType() == Repository::Git::GitType::REMOVED || line->getLineType() == Repository::Git::GitType::NONE ) {
			qDebug() << line->getLineNumber() << " " << line->getLineTypeAsString() << " " << line->getLine();
		}
	}
}

void Repository::Git::GitFileDiffBlock::printInfo()
{
	qDebug() << "-" << removeStart << "," << removeCount << " +" << addStart << "," << addCount;
	printDiffBlockLines();
}
