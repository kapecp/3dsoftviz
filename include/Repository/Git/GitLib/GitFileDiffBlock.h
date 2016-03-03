#ifndef GITFILEDIFFBLOCK_H
#define GITFILEDIFFBLOCK_H

#include "GitLib/GitType.h"
#include "GitLib/GitExport.h"

#include <QList>

namespace Repository {
namespace Git {
class GitFileDiffBlockLine;

class GITLIB_EXPORT GitFileDiffBlock
{
public:
	/**
	 * Explicit default constructor
	 * @brief Creates blank diff block
	 */
	GitFileDiffBlock();

	~GitFileDiffBlock();

	/**
	 * void setAddStart( int addStart )
	 * @brief Setter of block addition start position
	 * @param addStart Line position, where block addition start.
	 */
	void setAddStart( int addStart )
	{
		this->addStart = addStart;
	}

	/**
	 * int getAddStart()
	 * @brief Getter of block addition start position
	 * @return block addition start position
	 */
	int getAddStart()
	{
		return this->addStart;
	}

	/**
	 * void setAddCount()
	 * @brief Setter of number of lines affected from block addition start position
	 * @param addCount Number of lines affected from block addition start position
	 */
	void setAddCount( int addCount )
	{
		this->addCount = addCount;
	}

	/**
	 * int getAddCount()
	 * @brief Getter of number of lines affected from block addition start position
	 * @return Number of lines affected from block addition start position
	 */
	int getAddCount()
	{
		return this->addCount;
	}

	/**
	 * void setRemoveStart()
	 * @brief Setter of block removal start position
	 * @param removeStart block removal start position
	 */
	void setRemoveStart( int removeStart )
	{
		this->removeStart = removeStart;
	}

	/**
	 * int getRemoveStart()
	 * @brief Getter of block removal start position
	 * @return block removal start position
	 */
	int getRemoveStart()
	{
		return this->removeStart;
	}

	/**
	 * void setRemoveCount
	 * @brief Setter of number of lines affected from block removal start position
	 * @param removeCount Number of lines affected from block removal start position
	 */
	void setRemoveCount( int removeCount )
	{
		this->removeCount = removeCount;
	}

	/**
	 * int getRemoveCount()
	 * @brief Getter of number of lines affected from block removal start position
	 * @return Number of lines affected from block removal start position
	 */
	int getRemoveCount()
	{
		return this->removeCount;
	}

	/**
	 * void incAddCount()
	 * @brief Increments count of lines with type of ADDED
	 */
	void incAddCount()
	{
		this->addCount++;
	}

	/**
	 * QList<Repository::Git::GitFileDiffBlockLine*> getGitFileDiffBlockLines()
	 * @brief Getter of list of diff block lines
	 * @return List of diff block lines
	 */
	QList<Repository::Git::GitFileDiffBlockLine*> getGitFileDiffBlockLines()
	{
		return this->gitFileDiffBlockLines;
	}

	/**
	 * void addGitFileDiffBlockLine( Repository::Git::GitFileDiffBlockLine* line )
	 * @brief Append diff block line to the list of diff block lines
	 * @param line Diff block line, which will be appended
	 */
	void addGitFileDiffBlockLine( Repository::Git::GitFileDiffBlockLine* line )
	{
		this->gitFileDiffBlockLines.append( line );
	}

	/**
	 * void printDiffBlockLines()
	 * @brief Prints out to console diff block lines with line number and line type
	 */
	void printDiffBlockLines();

	/**
	 * void printInfo()
	 * @brief Prints out all info about diff block
	 */
	void printInfo();

	/**
	 * void printCurrentInfo()
	 * @brief Prints out current version of diff block. Removed lines are omitted.
	 */
	void printCurrentInfo();

	/**
	 * void printPreviousInfo()
	 * @brief Prints out previous version of diff block. Added lines are omitted.
	 */
	void printPreviousInfo();

private:
	/**
	 * int addStart
	 * @brief Line positino, where block addition starts
	 */
	int addStart;

	/**
	 * int addCount
	 * @brief Number of lines affected from block addition start position.
	 */
	int addCount;

	/**
	 * int removeStart
	 * @brief Line position, where block removal starts.
	 */
	int removeStart;

	/**
	 * int removeCount
	 * @brief Number of lines affected from block removal start position.
	 */
	int removeCount;

	/**
	 * QList<Repository::Git::GitFileDiffBlockLine*> gitFileDiffBlockLines
	 * @brief List of diff block lines.
	 */
	QList<Repository::Git::GitFileDiffBlockLine*> gitFileDiffBlockLines;
}; // class
} // namespace
}
#endif // GITFILEDIFFBLOCK_H
