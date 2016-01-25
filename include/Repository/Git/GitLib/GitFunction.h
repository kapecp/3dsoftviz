#ifndef GITFUNCTION_H
#define GITFUNCTION_H

#include "GitLib/GitType.h"
#include "GitLib/GitFunctionType.h"
#include "GitLib/GitExport.h"

#include <QString>
#include <QList>

namespace Repository {

namespace Git {
class GitFileDiffBlock;

class GITLIB_EXPORT GitFunction
{
public:
    GitFunction();

    GitFunction( QString name );

    void addFunctionCallee( GitFunction* callee ) {
        this->functionCallees.append( callee );
    }

    bool removeFunctionCallee( GitFunction* callee ) {
        return this->functionCallees.removeOne( callee );
    }

    void addFunctionCaller( GitFunction* caller ) {
        this->functionCallers.append( caller );
    }

    bool removeFunctionCaller( GitFunction* caller ) {
        return this->functionCallers.removeOne( caller );
    }

    QString getTypeAsString();

    QString getFunctionTypeAsString();

    // getters and setters

    void setName( QString name ) {
        this->name = name;
    }

    QString getName() {
        return this->name;
    }

    void setId( int id ) {
        this->id = id;
    }

    int getId() {
        return this->id;
    }

    void setType( Repository::Git::GitType type ) {
        this->type = type;
    }

    Repository::Git::GitType getType() {
        return this->type;
    }

    void setFunctionType( Repository::Git::GitFunctionType functionType ) {
        this->functionType = functionType;
    }

    Repository::Git::GitFunctionType getFunctionType() {
        return this->functionType;
    }

    void setFunctionCallees( QList<GitFunction*> functionCallees ) {
        this->functionCallees =  functionCallees;
    }

    QList<GitFunction*> getFunctionCallees() {
        return this->functionCallees;
    }

    void setFunctionCallers( QList<GitFunction*> functionCallers ) {
        this->functionCallers =  functionCallers;
    }

    QList<GitFunction*> getFunctionCallers() {
        return this->functionCallers;
    }

    void setModule( QString module ) {
        this->module = module;
    }

    QString getModule() {
        return this->module;
    }

private:

    QString name;

    int id;

    GitType type;

    Repository::Git::GitFunctionType functionType;

    QList<GitFunction*> functionCallees;

    QList<GitFunction*> functionCallers;

    QString module;



}; // class
} // namespace
}
#endif // GITFUNCTION_H
