#include "GitLib/GitMetaData.h"

#include "GitLib/GitFile.h"

Repository::Git::GitMetaData::GitMetaData() : occurence( 0 ), lastDiffVersion( "" ), lastFunctionInterval( "" ), luaMapping( -1 ), removedVersion( -1 ), callTree( nullptr ) {

}

Repository::Git::GitMetaData::~GitMetaData() {
    delete this->callTree;
}


void Repository::Git::GitMetaData::setCallTree( Repository::Git::GitFile *file ) {
    if( this->callTree != nullptr ) {
        delete this->callTree;
    }

    this->callTree = file;
}
