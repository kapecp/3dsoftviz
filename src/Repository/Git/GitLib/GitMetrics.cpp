#include "GitLib/GitMetrics.h"

#include "GitLib/GitVersion.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitFile.h"

#include <QDebug>

Repository::Git::GitMetrics::GitMetrics() {

}

Repository::Git::GitMetrics::GitMetrics( Repository::Git::GitEvolutionGraph *evolutionGraph, QString commitId ) : evolutionGraph( evolutionGraph ), commitId( commitId ) {
}

int Repository::Git::GitMetrics::getChangedCount( QString identifier ) {
    // inicializacia vystupnej premennej
    int result = 0;

    // pre kazdu verziu v evolucnom grafe hladaj zvoleny identifikator suboru
    foreach( Repository::Git::GitVersion* version, this->evolutionGraph->getVersions() ) {
        // ak zmenene subory verzie obsahuju identifikator, tak ziskaj git file a skontroluj jeho typ
        if( version->getChangedFiles()->contains( identifier ) ) {
            // ziskame dany git file
            Repository::Git::GitFile* file = version->getChangedFiles()->value( identifier );

            // ak je rozny od REMOVED tak ho zvacsime
            // inak counter vynulujeme a pocitame od 0
            if( file->getType() != Repository::Git::GitType::REMOVED ) {
                result++;
                qDebug() << "Changed in version with commitId" << version->getCommitId();
            } else {
                result = 0;
                qDebug() << "Changed to 0 in version with commitId" << version->getCommitId();
            }
        }
    }

    qDebug() << identifier << "was changed" << result << "times";

    // vrat pocet modifikacii suboru
    return result;
}

int Repository::Git::GitMetrics::getChangedCount( QString identifier, int count, int start ) {
    // Inicializacia vystupnej premennej
    int result = 0;

    // Ak je startovaci index mensi ako 0 a vacsi alebo rovny ako pocet verzii alebo rozdiel start a count je mensi ako 0,
    // tak nevieme vypocitat pocet zmien
    if( start < 0 || start > this->evolutionGraph->getVersions().size() || start + count < 0) {
        return -1;
    }

    int end, begin;

    // Ak je count kladne cislo, tak dopocitam maximalny mozny end
    // inak posuniem end na start a begin na posunuty start o count
    if( count >= 0 ) {
        end = ( start + count >= this->evolutionGraph->getVersions().size() ) ? this->evolutionGraph->getVersions().size() : start + count ;
        begin = start;
    } else {
        end = start;
        begin = start + count;
    }

    qDebug() << "Begin = " << begin << " and end = " << end;

    // Iteracia cez verzie v evolucnom grafe od zadaneho indexu
    for( int i = begin; i < end; i++ ) {
        // Ziskame verziu
        Repository::Git::GitVersion* version =  this->evolutionGraph->getVersions().at( i );
        // Ak verzia obsahuje nami zvoleny identifikator, tak pokracujeme so spracovanim suboru
        if( version->getChangedFiles()->contains( identifier ) ) {
            // Ziskame dany subor
            Repository::Git::GitFile* file = version->getChangedFiles()->value( identifier );

            // ak je rozny od REMOVED tak ho zvacsime
            // inak counter vynulujeme a pocitame od 0
            if( file->getType() != Repository::Git::GitType::REMOVED ) {
                result++;
                qDebug() << "Changed in version with commitId" << version->getCommitId();
            } else {
                result = 0;
                qDebug() << "Changed to 0 in version with commitId" << version->getCommitId();
            }
        }
    }

    qDebug() << identifier << "was changed" << result << "times";

    // vrat pocet modifikacii suboru
    return result;
}
