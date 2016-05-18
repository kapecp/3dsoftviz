#include "GitLib/GitUtils.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitVersion.h"
#include "GitLib/GitFile.h"
#include "GitLib/GitFunction.h"

#include <QTemporaryFile>
#include <QDir>
#include <QProcess>
#include <QDebug>

QString Repository::Git::GitUtils::makeTmpFileFromCommand( QString command, QString filepath )
{
	bool ok = true;

	// Ulozim si current working directory
	QString cwd = QDir::currentPath();

	// Nastavim absolutnu cestu k  temp file ako template a zakazem automaticke mazanie
	QTemporaryFile tempFile;
	tempFile.setFileTemplate( QDir::toNativeSeparators( QDir::tempPath() + "/" +  "qXXXXXX" ) );
	tempFile.setAutoRemove( false );

	// Ak sa nepodarilo vytvorit temp subor, tak nastavim flag "ok" na false a vypisem chybu
	if ( !tempFile.open() ) {
		qDebug() << "Nepodarilo sa vytvorit tmp subor";
		ok = false;
	}

	// Ak sa podarilo vytvorit temp subor, tak zmenim current working directory
	if ( ok ) {
		ok = changeDir( filepath );
	}

	// Ak sa podarilo zmenit current working directory, tak skontroluje existenciu git repozitara
	if ( ok ) {
		ok = existGit( filepath );
	}

	// Ak existuje na danej ceste git repozitar, tak vykonam command a vystup ulozim do temp suboru
	if ( ok ) {
		QProcess process;
		process.setStandardOutputFile( QDir::toNativeSeparators( tempFile.fileName() ) );
        process.start( command );
		process.waitForFinished();
		process.close();
		process.terminate();

	}

	// Vratim povodny current working directory, ak sa nepodari zmenit, vypisem do konzoly
	if ( !changeDir( cwd ) ) {
		qDebug() << "Nepodarilo sa vratit na povodny current working directory";
	}

	// Vratim absolutnu cestu k temp suboru
	return tempFile.fileName();
}

bool Repository::Git::GitUtils::changeDir( QString path )
{
	return QDir::setCurrent( path );
}

bool Repository::Git::GitUtils::existGit( QString path )
{
	return QDir( QDir::toNativeSeparators( path + "/.git" ) ).exists();
}

bool Repository::Git::GitUtils::changeCommit( QString commitId, QString filePath ) {
    bool ok = true;
    QString lCommand = "git checkout " + commitId;

    QString lTmp = makeTmpFileFromCommand( lCommand, filePath );

    QFile file( lTmp );

    // Kontrola na uspesnost ulozenia vystupu commandu do suboru, ak sa podarilo tak "ok" ostane true
    if ( !file.size() ) {
        ok = false;
    }

    // Ak sa mi nepodari vymazat subor, tak vypisem jeho nazov
    if ( !file.remove() ) {
        qDebug() << "Nepodarilo sa odstranit temp subor" << file.fileName();
    }

    return ok;
}

QList<QString> Repository::Git::GitUtils::getModifiedLuaNodesFromVersion( Repository::Git::GitEvolutionGraph* evolutionGraph, int versionNumber ) {
    /*
    Repository::Git::GitVersion* version = evolutionGraph->getVersion( versionNumber );
    QMap<QString, Repository::Git::GitFile*> modifiedFiles = version->getChangedFiles();
*/
    QList<QString> identifiers = QList<QString>();
/*
    // pre kazdy zmeneny subor, ktory je typu MODIFIED najdeme uzly, ktore sa zmenili
    foreach( Repository::Git::GitFile* file, modifiedFiles ) {
        // zistime si v ktorej verzii, bol posledne zmeneny dany subor
        int nearestVersionNumber = getLastModifiedVersionIndex( file->getFilename(), versionNumber, evolutionGraph );

        // ak je hodnota zmenenej verzie
        if( nearestVersionNumber > -1 ) {
//            qDebug() << "Subor" << file->getIdentifier() << "sa zmenil v " << nearestVersionNumber << " from " << versionNumber;

            // ziskame subor oproti ktoremu budem porovnavat
            Repository::Git::GitFile* otherFile = evolutionGraph->getVersion( nearestVersionNumber )->getGitFileByName( file->getFilename() );

            identifiers += getIdentifiersOfChangedItems( file, otherFile );
        } else {
//            qDebug() << "Subor" << file->getIdentifier() << "ma stav" << file->getTypeAsString();
            identifiers += getIdentifiersOfChangedItems( file, NULL );
        }
    }
*//*
    foreach( QString string, identifiers ) {
        QStringList list = string.split(";");
        qDebug() << list.at( 0 ) << "->" << list.at( 1 ) << "from" << list.at( 2 );
    }
*//*
    qDebug() << "Velkost vsetkych zmien = " << identifiers.size();
*/
    return identifiers;
}
/*
int Repository::Git::GitUtils::getLastModifiedVersionIndex( QString fileName, int fromIndex, Repository::Git::GitEvolutionGraph* evolutionGraph ) {
    for( int i = fromIndex - 1; i >= 0; i-- ) {
        QMap<QString, Repository::Git::GitFile*> changedFiles = evolutionGraph->getVersion( i )->getChangedFiles();
        foreach( Repository::Git::GitFile* file, changedFiles ) {
            if( !QString::compare( fileName, file->getFilename() ) ) {
                return i;
            }
        }
    }

    return -1;
}*/

QList<QString> Repository::Git::GitUtils::getIdentifiersOfChangedItems( Repository::Git::GitFile *file, Repository::Git::GitFile *otherFile ) {
    QList<QString> identifiers = QList<QString>();

    QMap<QString, bool>* functions = new QMap<QString, bool>();
        //
    for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = file->getGitFunctions()->begin(); iterator != file->getGitFunctions()->end(); ++iterator ) {
        if( iterator.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
            if( functions->contains( iterator.value()->getIdentifier() ) ) {
                continue;
            } else {
                functions->insert( iterator.value()->getIdentifier(), true );
            }
        }
        if( otherFile != NULL ) {
            identifiers += compareTwoFunctions( iterator.value(), otherFile->getGitFunctions()->find( iterator.value()->getIdentifier() ).value(), functions, file->getIdentifier() );
        } else {
            identifiers += compareTwoFunctions( iterator.value(), file->getGitFunctions()->end().value(), functions, file->getIdentifier() );
        }
    }

    if( otherFile != NULL ) {
        for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = otherFile->getGitFunctions()->begin(); iterator != otherFile->getGitFunctions()->end(); ++iterator ) {
            if( iterator.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
                if( functions->contains( iterator.value()->getIdentifier() ) ) {
                    continue;
                } else {
                    functions->insert( iterator.value()->getIdentifier(), true );
                }
            }

            identifiers += compareTwoFunctions( file->getGitFunctions()->find( iterator.value()->getIdentifier() ).value(), iterator.value(), functions, file->getIdentifier() );
        }
    }

//    qDebug() << "Velkost identifiers " << identifiers.size();

    delete functions;

    return identifiers;
}

QList<QString> Repository::Git::GitUtils::compareTwoFunctions( Repository::Git::GitFunction *function, Repository::Git::GitFunction *otherFunction, QMap<QString, bool>* functions, QString masterIdentifier ) {
    QList<QString> identifiers =  QList<QString>();

    QMap<QString, Repository::Git::GitFunction*> i;

    // ak existuju referencie na obe funkcie, tak skontrolujeme, ci neexistuju v subore zmeny, ktore sa dotykaju nejak z metod v subore
    // ak existuje referencia len na sucasnu funkciu, tak funkcia a jej volania by mali byt pridane do grafu
    // ak existuje referencia na predchadzajucu funkciu, tak funkcia a jej volania boli mali byt vymazane z grafu
    if( function != i.end().value()  && otherFunction != i.end().value() ) {
        // ak ide o funkciu, ktora je typu globalFunction, tak uz nic dalej nevola a koncime spracovanie
        if( function->getFunctionType() == Repository::Git::GitFunctionType::GLOBALFUNCTION ) {
//            qDebug() << "End" << function->getIdentifier() << "in master" << masterIdentifier;
        } else {
            // TODO - ak sa vyskytne LOCALFUNCTION, tak prejdi uzly, ktore vola (mapu caller) a pritom su typom GLOBALFUNCTION - lokalne netreba tie sa skontroluju samostatne pri
            //        prechadzany funkcii v subore
        }

        // TODO - tu bude treba doplnit kod, na priradenie zmeny riadku v subore k funkcii

    } else if( function == i.end().value() ) {
//        qDebug() << "REMOVED ->" << otherFunction->getIdentifier() << "from" << masterIdentifier;
        identifiers.append( "REMOVED;" + otherFunction->getIdentifier() + ";" + masterIdentifier );
        if( otherFunction->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION && otherFunction->getFunctionCallers()->size() > 0 ) {
            for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = otherFunction->getFunctionCallers()->begin(); iterator != otherFunction->getFunctionCallers()->end(); ++iterator ) {
                identifiers.append( "REMOVED;" +  iterator.value()->getIdentifier() + ";" + otherFunction->getIdentifier() );
//                qDebug() << "REMOVED ->" << iterator.value()->getIdentifier() << "from" << otherFunction->getIdentifier();
            }
        }
    } else {
//        qDebug() << "ADDED ->" << function->getIdentifier() << "from" << masterIdentifier;
        identifiers.append( "ADDED;" + function->getIdentifier() + ";" + masterIdentifier );
        if( function->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION && function->getFunctionCallers()->size() > 0 ) {
            for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = function->getFunctionCallers()->begin(); iterator != function->getFunctionCallers()->end(); ++iterator ) {
                identifiers.append( "ADDED;" + iterator.value()->getIdentifier() + ";" + function->getIdentifier() );
//                qDebug() << "ADDED ->" << iterator.value()->getIdentifier() << "from" << function->getIdentifier();
            }
        }
    }

    return identifiers;
}

QString Repository::Git::GitUtils::getBooleanAsString( bool value ) {
    return ( value == true ? QString( "true" ) : QString( "false") );
}

