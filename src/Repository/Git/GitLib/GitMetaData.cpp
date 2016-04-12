#include "GitLib/GitMetaData.h"

#include "GitLib/GitFile.h"

Repository::Git::GitMetaData::GitMetaData() : occurence( 0 ), lastDiffVersion( "" ), lastFunctionInterval( "" ), luaMapping( -1 ), changedVersion( -1 ), callTree( nullptr ), firstOccurenceInGraph( -1 ), changedCount( 0 ), metrics( new QMap<QString, int>() ) {

}

Repository::Git::GitMetaData::~GitMetaData() {
    delete this->callTree;
    delete this->metrics;
}


void Repository::Git::GitMetaData::setCallTree( Repository::Git::GitFile *file ) {
    if( this->callTree != nullptr ) {
        delete this->callTree;
    }

    this->callTree = file;
}

int Repository::Git::GitMetaData::getMetricsValue( QString key ) {
    if( !this->metrics->contains( key ) ) {
        this->metrics->insert( key, 0 );
    }

    return this->metrics->value( key );
}
