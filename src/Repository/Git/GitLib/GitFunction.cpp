#include "GitLib/GitFunction.h"

#include <QDebug>

// TODO zmenit konstruktory

Repository::Git::GitFunction::GitFunction() {
    this->name = "";
    this->id = -1;
    this->functionRowNumber = -1;
    this->type = Repository::Git::GitType::ADDED;
    this->functionType = Repository::Git::GitFunctionType::LOCALFUNCTION;
    this->module = "";

    this->functionCallees = new QMap<QString, Repository::Git::GitFunction*>();
    this->functionCallers = new QMap<QString, Repository::Git::GitFunction*>();
}

Repository::Git::GitFunction::GitFunction( QString name ) : name( name ) {
    this->id = -1;
    this->functionRowNumber = -1;
    this->type = Repository::Git::GitType::ADDED;
    this->functionType = Repository::Git::GitFunctionType::LOCALFUNCTION;
    this->module = "";

    this->functionCallees = new QMap<QString, Repository::Git::GitFunction*>();
    this->functionCallers = new QMap<QString, Repository::Git::GitFunction*>();
}

QString Repository::Git::GitFunction::getFunctionTypeAsString() {

    // zoberie typ funkcie a vrati ho ako QString
    switch(getFunctionType()) {
    case Repository::Git::GitFunctionType::GLOBALFUNCTION:
        return "globalFunction";
    case Repository::Git::GitFunctionType::LOCALFUNCTION:
        return "function";
    default:
        return "Not set";
    }
}

QString Repository::Git::GitFunction::getTypeAsString() {


    // zoberie typ suboru a vrati ho ako QString
    switch ( this->getType() ) {
        case Repository::Git::GitType::ADDED:
            return "Added";
        case Repository::Git::GitType::MODIFIED:
            return "Modified";
        case Repository::Git::GitType::REMOVED:
            return "Removed";
        case Repository::Git::GitType::NONE:
            return "NONE";
        default:
            return "Wrong or Not set";
    }
}

QString Repository::Git::GitFunction::getIdentifier() {
    if( this->module != "" ) {
        return this->getFunctionTypeAsString() + ";" + this->module + ";" + this->name;
    } else {
        return this->getFunctionTypeAsString() + ";" + this->name;
    }
}

