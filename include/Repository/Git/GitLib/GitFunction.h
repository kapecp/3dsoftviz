#ifndef GITFUNCTION_H
#define GITFUNCTION_H

#include "GitLib/GitType.h"
#include "GitLib/GitFunctionType.h"
#include "GitLib/GitExport.h"
#include "GitLib/GitFunction.h"

#include <QString>
#include <QMap>

namespace Repository {

namespace Git {
class GitFileDiffBlock;

class GITLIB_EXPORT GitFunction
{
public:
	GitFunction();

	GitFunction( QString name );

	void addFunctionCallee( GitFunction* callee )
	{
		this->functionCallees->insert( callee->getIdentifier(), callee );
	}

	bool removeFunctionCallee( GitFunction* callee )
	{
		return !!this->functionCallees->remove( callee->getIdentifier() );
	}

	void addFunctionCaller( GitFunction* caller )
	{
		this->functionCallers->insert( caller->getIdentifier(), caller );
	}

	bool removeFunctionCaller( GitFunction* caller )
	{
		return !!this->functionCallers->remove( caller->getIdentifier() );
	}

	QString getTypeAsString();

	QString getFunctionTypeAsString();

	// getters and setters

	void setName( QString name )
	{
		this->name = name;
	}

	QString getName()
	{
		return this->name;
	}

	void setId( qlonglong id )
	{
		this->id = id;
	}

	qlonglong getId()
	{
		return this->id;
	}

	void setType( Repository::Git::GitType type )
	{
		this->type = type;
	}

	Repository::Git::GitType getType()
	{
		return this->type;
	}

	void setFunctionType( Repository::Git::GitFunctionType functionType )
	{
		this->functionType = functionType;
	}

	Repository::Git::GitFunctionType getFunctionType()
	{
		return this->functionType;
	}

	void setFunctionCallees( QMap<QString, GitFunction*>* functionCallees )
	{
		this->functionCallees =  functionCallees;
	}

	QMap<QString, GitFunction*>* getFunctionCallees()
	{
		return this->functionCallees;
	}

	void setFunctionCallers( QMap<QString, GitFunction*>* functionCallers )
	{
		this->functionCallers =  functionCallers;
	}

	QMap<QString, GitFunction*>* getFunctionCallers()
	{
		return this->functionCallers;
	}

	void setModule( QString module )
	{
		this->module = module;
	}

	QString getIdentifier();

	QString getModule()
	{
		return this->module;
	}

	int getFunctionRowNumber()
	{
		return this->functionRowNumber;
	}

	void setFunctionRowNumber( int number )
	{
		this->functionRowNumber = number;
	}

private:

	QString name;

	qlonglong id;

	int functionRowNumber;

	GitType type;

	Repository::Git::GitFunctionType functionType;

	QMap<QString, GitFunction*>* functionCallees;

	QMap<QString, GitFunction*>* functionCallers;

	QString module;



}; // class
} // namespace
}
#endif // GITFUNCTION_H
