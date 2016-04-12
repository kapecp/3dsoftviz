#ifndef GITMETADATA_H
#define GITMETADATA_H

#include "Repository/Git/GitLib/GitExport.h"

#include <QString>
#include <QMap>

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

    void setChangedVersion( int version ) {
        this->changedVersion = version;
    }

    int getChangedVersion() {
        return this->changedVersion;
    }

    void clearChangedVersion() {
        this->changedVersion = -1;
    }

    void setCallTree( Repository::Git::GitFile* file );

    Repository::Git::GitFile* getCallTree() {
        return this->callTree;
    }

    void setFirstOccurence( int firstOccurence ) {
        this->firstOccurenceInGraph = firstOccurence;
    }

    int getFirstOccurence() {
        return this->firstOccurenceInGraph;
    }

    void setChangedCount( int count ) {
        this->changedCount = count;
    }

    int getChangedCount() {
        return this->changedCount;
    }

    void increaseChangedCount() {
        this->changedCount++;
    }

    void decreaseChangedCount() {
        this->changedCount--;
    }

    void insertMetrics(QString key, int value ) {
        this->metrics->insert( key, value );
    }

    int getMetricsValue( QString key );

    bool metricsContains( QString key ) {
        return this->metrics->contains( key );
    }

    QMap<QString, int>* getMetric() {
        return this->metrics;
    }

private:
    int occurence;

    QString lastDiffVersion;

    QString lastFunctionInterval;

    qlonglong luaMapping;

    int firstOccurenceInGraph;

    int changedVersion;

    Repository::Git::GitFile* callTree;

    QMap<QString, int>* metrics;

    int changedCount;
}; // class
} // namespace
}
#endif // GITMETADATA_H
