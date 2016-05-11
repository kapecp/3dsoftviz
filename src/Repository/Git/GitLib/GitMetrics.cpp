#include "GitLib/GitMetrics.h"

#include "GitLib/GitVersion.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitFile.h"
#include "GitLib/GitFunction.h"

#include <QDebug>
#include <QSet>

Repository::Git::GitMetrics::GitMetrics() {

}

Repository::Git::GitMetrics::GitMetrics( Repository::Git::GitEvolutionGraph *evolutionGraph ) : evolutionGraph( evolutionGraph ) {
}

int Repository::Git::GitMetrics::getFileChangedCount( QString identifier ) {
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
//                qDebug() << "Changed in version with commitId" << version->getCommitId();
            } else {
                result = 0;
//                qDebug() << "Changed to 0 in version with commitId" << version->getCommitId();
            }
        }
    }

//    qDebug() << identifier << "was changed" << result << "times";

    // vrat pocet modifikacii suboru
    return result;
}

int Repository::Git::GitMetrics::getFileChangedCount( QString identifier, int count, int start ) {
    // Inicializacia vystupnej premennej
    int result = 0;

    // Ak je startovaci index mensi ako 0 a vacsi alebo rovny ako pocet verzii alebo rozdiel start a count je mensi ako 0,
    // tak nevieme vypocitat pocet zmien
    if( start < 0 || start > this->evolutionGraph->getVersions().size() || start + count < 0 ) {
        return -1;
    }

    // Definicia pomocnych premennych
    int end, begin;

    // Ak je count kladne cislo, tak dopocitam maximalny mozny end
    // inak posuniem end na start a begin na posunuty start o count
    if( count >= 0 ) {
        end = ( start + count > this->evolutionGraph->getVersions().size() ) ? this->evolutionGraph->getVersions().size() : start + count ;
        begin = start;
    } else {
        end = start;
        begin = start + count;
    }

//    qDebug() << "Begin = " << begin << " and end = " << end;

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
//                qDebug() << "Changed in version with commitId" << version->getCommitId();
            } else {
                result = 0;
//                qDebug() << "Changed to 0 in version with commitId" << version->getCommitId();
            }
        }
    }

//    qDebug() << identifier << "was changed" << result << "times";

    // vrat pocet modifikacii suboru
    return result;
}

int Repository::Git::GitMetrics::getFileChangedCount( QString identifier, int count, QString startCommitId ) {
    // inicializacia vystupnej premennej
    int result = -1;

    int versionIndex = -1;

    // Zistime poziciu verzie v zozname verzii evolucneho grafu
    for( int i = 0; i < this->evolutionGraph->getVersions().size(); i++ ) {
        if( this->evolutionGraph->getVersion( i )->getCommitId() == startCommitId ) {
            versionIndex = i;
            break;
        }
    }

    // Ak sme nasli commitId vo verziach, tak vyhladame pocet zmien podla vstupnych parametrov ( moze vratit aj -1 )
    // inak vrat nastavenu hodnotu
    if( versionIndex != -1 ) {
        result = getFileChangedCount( identifier, count, versionIndex );
    }

//    qDebug() << identifier << "was changed" << result << "times";

    // vrat pocet modifikacii suboru
    return result;
}

int Repository::Git::GitMetrics::getFileChangedCount( QString identifier, QString endCommitId, QString startCommitId ) {
    // inicializacia vystupnej premennej
    int result = -1;

    int versionStartIndex = -1;
    int versionEndIndex = -1;

    for( int i = 0; i < this->evolutionGraph->getVersions().size(); i++ ) {
        if( this->evolutionGraph->getVersion( i )->getCommitId() == endCommitId ) {
            versionEndIndex = i;
        }

        if( this->evolutionGraph->getVersion( i )->getCommitId() == startCommitId ) {
            versionStartIndex = i;
        }

        if( versionEndIndex != -1 && versionStartIndex != -1 ) {
            break;
        }
    }

//    qDebug() << "VersionStartIndex = " << versionStartIndex << " and VersionEndIndex = " << versionEndIndex;

    if( versionEndIndex != -1 && versionStartIndex != -1 ) {
        if( versionEndIndex - versionStartIndex > 0 ) {
            result =  getFileChangedCount( identifier, versionEndIndex + 1 - versionStartIndex, versionStartIndex );
        } else {
            result =  getFileChangedCount( identifier, versionEndIndex - 1 - versionStartIndex, versionStartIndex + 1 );
        }
    }


//    qDebug() << identifier << "was changed" << result << "times";

    // vrat pocet modifikacii suboru
    return result;
}

int Repository::Git::GitMetrics::getAuthorCount( QString author ) {
    // inicializacia vystupnej premennej
    int result = 0;

    foreach( Repository::Git::GitVersion* version, this->evolutionGraph->getVersions() ) {
        if( version->getAuthor() == author ) {
            result++;
//            qDebug() << author << " changed commitId " << version->getCommitId();
        }
    }

//    qDebug() << author << "changed" << result << "versions";

    // vrat pocet modifikacii verzie autorom
    return result;
}

int Repository::Git::GitMetrics::getAuthorCount( QString author, int count, int start ) {
    // inicializacia vystupnej premennej
    int result = 0;

    if( start < 0 || start > this->evolutionGraph->getVersions().size() || start + count < 0 ) {
        return -1;
    }

    int begin, end;

    if( count >= 0 ) {
        end = ( start + count > this->evolutionGraph->getVersions().size() ) ? this->evolutionGraph->getVersions().size() : start + count ;
        begin = start;
    } else {
        end = start;
        begin = start + count;
    }

//    qDebug() << "Begin = " << begin << " and end = " << end;

    for( int i = begin; i < end; i++ ) {
        Repository::Git::GitVersion* version = this->evolutionGraph->getVersion( i );

        if( version->getAuthor() == author ) {
            result++;
        }
    }

//    qDebug() << author << "changed" << result << "versions";

    // vrat pocet modifikacii verzie autorom
    return result;
}

int Repository::Git::GitMetrics::getAuthorCount( QString author, int count, QString startCommitId ) {
    // inicializacia vystupnej premennej
    int result = -1;

    int versionIndex = -1;

    // Zistime poziciu verzie v zozname verzii evolucneho grafu
    for( int i = 0; i < this->evolutionGraph->getVersions().size(); i++ ) {
        if( this->evolutionGraph->getVersion( i )->getCommitId() == startCommitId ) {
            versionIndex = i;
            break;
        }
    }

    // Ak sme nasli commitId vo verziach, tak vyhladame pocet zmien podla vstupnych parametrov ( moze vratit aj -1 )
    // inak vrat nastavenu hodnotu
    if( versionIndex != -1 ) {
        result = getAuthorCount( author, count, versionIndex );
    }

//    qDebug() << author << "changed" << result << "versions";

    // vrat pocet modifikacii autorom
    return result;
}

int Repository::Git::GitMetrics::getAuthorCount( QString author, QString endCommitId, QString startCommitId ) {
    // inicializacia vystupnej premennej
    int result = -1;

    int versionStartIndex = -1;
    int versionEndIndex = -1;

    for( int i = 0; i < this->evolutionGraph->getVersions().size(); i++ ) {
        if( this->evolutionGraph->getVersion( i )->getCommitId() == endCommitId ) {
            versionEndIndex = i;
        }

        if( this->evolutionGraph->getVersion( i )->getCommitId() == startCommitId ) {
            versionStartIndex = i;
        }

        if( versionEndIndex != -1 && versionStartIndex != -1 ) {
            break;
        }
    }

//    qDebug() << "VersionStartIndex = " << versionStartIndex << " and VersionEndIndex = " << versionEndIndex;

    if( versionEndIndex != -1 && versionStartIndex != -1 ) {
        if( versionEndIndex - versionStartIndex > 0 ) {
            result =  getAuthorCount( author, versionEndIndex + 1 - versionStartIndex, versionStartIndex );
        } else {
            result =  getAuthorCount( author, versionEndIndex - 1 - versionStartIndex, versionStartIndex + 1 );
        }
    }


//    qDebug() << author << "changed" << result << "versions";

    // vrat pocet modifikacii autorom
    return result;
}

QList<QString> Repository::Git::GitMetrics::getAuthorList() {
    QSet<QString> authors = QSet<QString>();

    foreach( Repository::Git::GitVersion* version, this->evolutionGraph->getVersions() ) {
        authors.insert( version->getAuthor() );
    }

    return authors.toList();
}

QList<QString> Repository::Git::GitMetrics::getAuthorList( int position ) {
    QSet<QString> authors = QSet<QString>();

    int end =  ( position > this->evolutionGraph->getVersions().size() ) ? this->evolutionGraph->getVersions().size() : position;

    for( int i = 0; i < end; i++ ) {
        authors.insert( this->evolutionGraph->getVersion( i )->getAuthor() );
    }

    return authors.toList();
}

QList<QString> Repository::Git::GitMetrics::getFilesFromAuthor( QString author ) {
    QSet<QString> files =  QSet<QString>();

    foreach( Repository::Git::GitVersion* version, this->evolutionGraph->getVersions() ) {
        if( version->getAuthor() == author ) {
            for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = version->getChangedFiles()->begin(); iterator != version->getChangedFiles()->end(); ++iterator ) {
                files.insert( iterator.value()->getIdentifier() );
            }
        }
    }

    return files.toList();
}

QList<QString> Repository::Git::GitMetrics::getFilesFromAuthor( QString author, int position ) {
    QSet<QString> files =  QSet<QString>();

    int end = ( position > this->evolutionGraph->getVersions().size() ) ? this->evolutionGraph->getVersions().size() : position;

    for( int i = 0; i < end; i++ ) {
        Repository::Git::GitVersion* version = this->evolutionGraph->getVersion( i );
        if( version->getAuthor() == author ) {
            for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = version->getChangedFiles()->begin(); iterator != version->getChangedFiles()->end(); ++iterator ) {
                files.insert( iterator.value()->getIdentifier() );
            }
        }
    }

    return files.toList();
}

QList<QString> Repository::Git::GitMetrics::getAllFiles() {
    QSet<QString> files =  QSet<QString>();

    foreach( Repository::Git::GitVersion* version, this->evolutionGraph->getVersions() ) {
        for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = version->getChangedFiles()->begin(); iterator != version->getChangedFiles()->end(); ++iterator ) {
            files.insert( iterator.value()->getIdentifier() );
        }
    }

    return files.toList();
}

QList<QString> Repository::Git::GitMetrics::getFilesToPosition( int position ) {
    QSet<QString> files =  QSet<QString>();

    int end = ( position > this->evolutionGraph->getVersions().size() ) ? this->evolutionGraph->getVersions().size() : position;

    for( int i = 0; i < end; i++ ) {
        Repository::Git::GitVersion* version = this->evolutionGraph->getVersion( i );

        for( QMap<QString, Repository::Git::GitFile*>::iterator iterator = version->getChangedFiles()->begin(); iterator != version->getChangedFiles()->end(); ++iterator ) {
            files.insert( iterator.value()->getIdentifier() );
        }
    }

    return files.toList();
}

QList<QString> Repository::Git::GitMetrics::getFunctionsFromFile( QString identifier ) {
    QSet<QString> functions = QSet<QString>();

    foreach( Repository::Git::GitVersion* version, this->evolutionGraph->getVersions() ) {
        if( version->getIsLoaded() ) {
            Repository::Git::GitFile* file = version->getGitFileByIdentifier( identifier );
            if( file ) {
                for( QMap<QString, Repository::Git::GitFunction*>::iterator it = file->getGitFunctions()->begin(); it != file->getGitFunctions()->end(); ++it ) {
                    functions.insert( it.value()->getIdentifier() );

                    if( it.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
                        for( QMap<QString, Repository::Git::GitFunction*>::iterator iter = it.value()->getFunctionCallers()->begin(); iter != it.value()->getFunctionCallers()->end(); ++iter ) {
                            functions.insert( iter.value()->getIdentifier() );

                            if( iter.value()->getFunctionType() == Repository::Git::GitFunctionType::GLOBALFUNCTION && iter.value()->getModule() != "" ) {
                                functions.insert( "globalModule;" + iter.value()->getModule() );
                            }
                        }
                    } else {
                        if( it.value()->getModule() != "" ) {
                            functions.insert( "globalModule;" + it.value()->getModule() );
                        }
                    }

                }
            }
        } else {
            break;
        }
    }

//    qDebug() << identifier << "pocet funkcii" << functions.size();

    return functions.toList();
}

QList<QString> Repository::Git::GitMetrics::getFunctionsFromFile( QString identifier, int level ) {
    QSet<QString> functions = QSet<QString>();

    foreach( Repository::Git::GitVersion* version, this->evolutionGraph->getVersions() ) {
        if( version->getIsLoaded() ) {
            Repository::Git::GitFile* file = version->getGitFileByIdentifier( identifier );
            if( file ) {
                for( QMap<QString, Repository::Git::GitFunction*>::iterator it = file->getGitFunctions()->begin(); it != file->getGitFunctions()->end(); ++it ) {

                    if( it.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
                        if( level >= 0 ) {
                            functions.insert( it.value()->getIdentifier() );
                        }

                        for( QMap<QString, Repository::Git::GitFunction*>::iterator iter = it.value()->getFunctionCallers()->begin(); iter != it.value()->getFunctionCallers()->end(); ++iter ) {
                            if( iter.value()->getFunctionType() == Repository::Git::GitFunctionType::GLOBALFUNCTION ) {
                                if( level >= 1 ) {
                                    functions.insert( iter.value()->getIdentifier() );
                                }

                                if( iter.value()->getModule() != "" && level >= 2 ) {
                                    functions.insert( "globalModule;" + iter.value()->getModule() );
                                }
                            }
                        }
                    } else {
                        if( it.value()->getModule() != "" && level >= 2 ) {
                            functions.insert( "globalModule;" + it.value()->getModule() );
                        }

                        if( level >= 1 ) {
                            functions.insert( it.value()->getIdentifier() );
                        }
                    }

                }
            }
        } else {
            break;
        }
    }

//    qDebug() << identifier << "pocet funkcii" << functions.size();

    return functions.toList();
}

QList<QString> Repository::Git::GitMetrics::getFunctionsFromFile( QString identifier, QString author ) {
    QSet<QString> functions = QSet<QString>();

    foreach( Repository::Git::GitVersion* version, this->evolutionGraph->getVersions() ) {
        if( version->getIsLoaded() ) {
            if( version->getAuthor() == author ) {
                Repository::Git::GitFile* file = version->getGitFileByIdentifier( identifier );
                if( file ) {
                    for( QMap<QString, Repository::Git::GitFunction*>::iterator it = file->getGitFunctions()->begin(); it != file->getGitFunctions()->end(); ++it ) {
                        functions.insert( it.value()->getIdentifier() );

                        if( it.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
                            for( QMap<QString, Repository::Git::GitFunction*>::iterator iter = it.value()->getFunctionCallers()->begin(); iter != it.value()->getFunctionCallers()->end(); ++iter ) {
                                functions.insert( iter.value()->getIdentifier() );

                                if( iter.value()->getFunctionType() == Repository::Git::GitFunctionType::GLOBALFUNCTION && iter.value()->getModule() != "" ) {
                                    functions.insert( "globalModule;" + iter.value()->getModule() );
                                }
                            }
                        } else {
                            if( it.value()->getModule() != "" ) {
                                functions.insert( "globalModule;" + it.value()->getModule() );
                            }
                        }

                    }
                }
            }
        } else {
            break;
        }
    }

//    qDebug() << identifier << author << "pocet funkcii" << functions.size();

    return functions.toList();
}

QList<QString> Repository::Git::GitMetrics::getFunctionConnectorsFromFile( QString identifier ) {
    QSet<QString> functionConnectors = QSet<QString>();

    foreach( Repository::Git::GitVersion* version, this->evolutionGraph->getVersions() ) {
        if( version->getIsLoaded() ) {
            Repository::Git::GitFile* file = version->getGitFileByIdentifier( identifier );

            if( file ) {
                for( QMap<QString, Repository::Git::GitFunction*>::iterator it = file->getGitFunctions()->begin(); it != file->getGitFunctions()->end(); ++it ) {
                    functionConnectors.insert( file->getIdentifier() + "+" + it.value()->getIdentifier() );

                    if( it.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
                        for( QMap<QString, Repository::Git::GitFunction*>::iterator iter = it.value()->getFunctionCallers()->begin(); iter != it.value()->getFunctionCallers()->end(); ++iter ) {
                            functionConnectors.insert( it.value()->getIdentifier() + "+" + iter.value()->getIdentifier() );

                            if( iter.value()->getFunctionType() == Repository::Git::GitFunctionType::GLOBALFUNCTION && iter.value()->getModule() != "" ) {
                                functionConnectors.insert( iter.value()->getIdentifier() + "+globalModule;" + iter.value()->getModule() );
                            }
                        }
                    } else {
                        if( it.value()->getModule() != "" ) {
                            functionConnectors.insert( it.value()->getIdentifier() + "+globalModule;" + it.value()->getModule() );
                        }
                    }

                }
            }
        } else {
            break;
        }
    }

//    qDebug() << identifier << "pocet konektorov" << functionConnectors.size();

    return functionConnectors.toList();
}


QList<QString> Repository::Git::GitMetrics::getFunctionConnectorsFromFile( QString identifier, int level ) {
    QSet<QString> functionConnectors = QSet<QString>();

    foreach( Repository::Git::GitVersion* version, this->evolutionGraph->getVersions() ) {
        if( version->getIsLoaded() ) {
            Repository::Git::GitFile* file = version->getGitFileByIdentifier( identifier );

            if( file ) {
                for( QMap<QString, Repository::Git::GitFunction*>::iterator it = file->getGitFunctions()->begin(); it != file->getGitFunctions()->end(); ++it ) {
                    if( it.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
                        if( level >= 0 ) {
                            functionConnectors.insert( file->getIdentifier() + "+" + it.value()->getIdentifier() );
                        }

                        for( QMap<QString, Repository::Git::GitFunction*>::iterator iter = it.value()->getFunctionCallers()->begin(); iter != it.value()->getFunctionCallers()->end(); ++iter ) {
                            if( iter.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
                                if( level >= 0 ) {
                                    functionConnectors.insert( it.value()->getIdentifier() + "+" + iter.value()->getIdentifier() );
                                }
                            } else {
                                if( iter.value()->getModule() != "" && level >= 2 ) {
                                    functionConnectors.insert( iter.value()->getIdentifier() + "+globalModule;" + iter.value()->getModule() );
                                }

                                if( level >= 1 ) {
                                    functionConnectors.insert( it.value()->getIdentifier() + "+" + iter.value()->getIdentifier() );
                                }
                            }
                        }
                    } else {
                        if( it.value()->getModule() != "" && level >= 2 ) {
                            functionConnectors.insert( it.value()->getIdentifier() + "+globalModule;" + it.value()->getModule() );
                        }

                        if( level >= 1 ) {
                            functionConnectors.insert( file->getIdentifier() + "+" + it.value()->getIdentifier() );
                        }
                    }

                }
            }
        } else {
            break;
        }
    }

//    qDebug() << identifier << "pocet konektorov" << functionConnectors.size();

    return functionConnectors.toList();
}

QList<QString> Repository::Git::GitMetrics::getFunctionConnectorsFromFile( QString identifier , QString author ) {
    QSet<QString> functionConnectors = QSet<QString>();

    foreach( Repository::Git::GitVersion* version, this->evolutionGraph->getVersions() ) {
        if( version->getIsLoaded() ) {
            if( version->getAuthor() == author ) {
                Repository::Git::GitFile* file = version->getGitFileByIdentifier( identifier );

                if( file ) {
                    for( QMap<QString, Repository::Git::GitFunction*>::iterator it = file->getGitFunctions()->begin(); it != file->getGitFunctions()->end(); ++it ) {
                        functionConnectors.insert( file->getIdentifier() + "+" + it.value()->getIdentifier() );

                        if( it.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
                            for( QMap<QString, Repository::Git::GitFunction*>::iterator iter = it.value()->getFunctionCallers()->begin(); iter != it.value()->getFunctionCallers()->end(); ++iter ) {
                                functionConnectors.insert( it.value()->getIdentifier() + "+" + iter.value()->getIdentifier() );

                                if( iter.value()->getFunctionType() == Repository::Git::GitFunctionType::GLOBALFUNCTION && iter.value()->getModule() != "" ) {
                                    functionConnectors.insert( iter.value()->getIdentifier() + "+globalModule;" + iter.value()->getModule() );
                                }
                            }
                        } else {
                            if( it.value()->getModule() != "" ) {
                                functionConnectors.insert( it.value()->getIdentifier() + "+globalModule;" + it.value()->getModule() );
                            }
                        }

                    }
                }
            }
        } else {
            break;
        }
    }

//    qDebug() << identifier << author << "pocet konektorov" << functionConnectors.size();

    return functionConnectors.toList();
}
