#ifndef GITFILEDIFFBLOCKLINE_H
#define GITFILEDIFFBLOCKLINE_H

#include "Git/GitType.h"

#include <QString>

namespace Git {

class GitFileDiffBlockLine
{
public:
	/**
	 * Explicit default constructor
	 * @brief Creates blank diff block line
	 */
	GitFileDiffBlockLine();

	/**
	 * Explicit constructor
	 * @brief Creates git diff block line
	 * @param line
	 * @param lineNumber
	 * @param lineType
	 */
	GitFileDiffBlockLine( QString line, int lineNumber, Git::GitType lineType );

	/**
	 * void setLine( QString line )
	 * @brief Setter for line
	 * @param line
	 */
	void setLine( QString line )
	{
		this->line = line;
	}

	/**
	 * QString getLine()
	 * @brief Getter for line
	 * @return line
	 */
	QString getLine()
	{
		return this->line;
	}

	/**
	 * void setLineNumber( int lineNumber )
	 * @brief Setter for line number
	 * @param lineNumber
	 */
	void setLineNumber( int lineNumber )
	{
		this->lineNumber = lineNumber;
	}

	/**
	 * int getLineNumber()
	 * @brief Getter for line number
	 * @return line number
	 */
	int getLineNumber()
	{
		return this->lineNumber;
	}

	/**
	 * void setLineType( Git::GitType lineType )
	 * @brief Setter for line type
	 * @param lineType type of line
	 */
	void setLineType( Git::GitType lineType )
	{
		this->lineType = lineType;
	}

	/**
	 * Git::GitType getLineType()
	 * @brief Returns enum type value
	 * @return enum type value
	 */
	Git::GitType getLineType()
	{
		return this->lineType;
	}

	/**
	 * QString getLineTypeAsString()
	 * @brief Returns line type as QString
	 * @return line type as QString
	 */
	QString getLineTypeAsString();


private:
	/**
	 * QString line
	 * @brief Contains difference between two versions.
	 */
	QString line;

	/**
	 * int lineNumber
	 * @brief Contains line position for new version, if line type is ADDED or NONE, or previous position, if line type is REMOVED.
	 */
	int lineNumber;

	/**
	 * Git::GitType lineType
	 * @brief Type of line in diff. Contains enum values: ADDED, REMOVED, NONE.
	 */
	Git::GitType lineType;
}; // class
} // namespace
#endif // GITFILEDIFFBLOCKLINE_H
