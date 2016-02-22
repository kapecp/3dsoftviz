#ifndef GITMETADATA_H
#define GITMETADATA_H

#include "Repository/Git/GitLib/GitExport.h"

#include <QString>

namespace Repository {

namespace Git {
class GitFile;

class GITLIB_EXPORT GitMetaData {
public:
    GitMetaData();

    ~GitMetaData();

    int increaseOccurence() {
        return ++this->occurence;
    }

    int decreaseOccurence() {
        return --this->occurence;
    }

    int getOccurence() {
        return this->occurence;
    }

    void setLastDiffVersion( QString commitId ) {
        this->lastDiffVersion = commitId;
    }

    QString getLastDiffVersion() {
        return this->lastDiffVersion;
    }

    void setLastFunctionInterval( QString interval ) {
        this->lastFunctionInterval = interval;
    }

    QString getLastFunctionInterval() {
        return this->lastFunctionInterval;
    }

    void setLuaMapping( qlonglong luaId ) {
        this->luaMapping = luaId;
    }

    qlonglong getLuaMapping() {
        return this->luaMapping;
    }

    void setRemovedVersion( int version ) {
        this->removedVersion = version;
    }

    int getRemovedVersion() {
        return this->removedVersion;
    }

    void clearRemovedVersion() {
        this->removedVersion = -1;
    }

    void setCallTree( Repository::Git::GitFile* file );

    Repository::Git::GitFile* getCallTree() {
        return this->callTree;
    }

private:
    int occurence;

    QString lastDiffVersion;

    QString lastFunctionInterval;

    qlonglong luaMapping;

    int removedVersion;

    Repository::Git::GitFile* callTree;

}; // class
} // namespace
}
#endif // GITMETADATA_H
