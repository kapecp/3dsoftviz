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

void Repository::Git::GitUtils::getModifiedLuaNodesFromVersion( Repository::Git::GitEvolutionGraph* evolutionGraph, int versionNumber ) {
    Repository::Git::GitVersion* version = evolutionGraph->getVersion( versionNumber );
    QList<Repository::Git::GitFile*> modifiedFiles = version->getGitFilesByType( Repository::Git::GitType::MODIFIED );

    // pre kazdy zmeneny subor, ktory je typu MODIFIED najdeme uzly, ktore sa zmenili
    foreach( Repository::Git::GitFile* file, modifiedFiles ) {
        // zistime si v ktorej verzii, bol posledne zmeneny dany subor
        int nearestVersionNumber = getLastModifiedVersionIndex( file->getFilename(), versionNumber, evolutionGraph );

        qDebug() << "Subor" << file->getFilename() << "sa zmenil v " << nearestVersionNumber << " from " << versionNumber;

        // ak je hodnota zmenenej verzie
        if( nearestVersionNumber > -1 ) {
            // ziskame subor oproti ktoremu budem porovnavat
            Repository::Git::GitFile* otherFile = evolutionGraph->getVersion( nearestVersionNumber )->getGitFileByName( file->getFilename() );

            QList<QString> identifiers = getIdentifiersOfChangedItems( file, otherFile );
        }
    }
}

int Repository::Git::GitUtils::getLastModifiedVersionIndex( QString fileName, int fromIndex, Repository::Git::GitEvolutionGraph* evolutionGraph ) {
    for( int i = fromIndex - 1; i >= 0; i-- ) {
        QList<Repository::Git::GitFile*> changedFiles = evolutionGraph->getVersion( i )->getChangedFiles();
        foreach( Repository::Git::GitFile* file, changedFiles ) {
            if( !QString::compare( fileName, file->getFilename() ) ) {
                return i;
            }
        }
    }

    return -1;
}

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

        identifiers += compareTwoFunctions( iterator.value(), otherFile->getGitFunctions()->find( iterator.value()->getIdentifier() ).value(), functions, file->getFilename() );
    }

    for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = otherFile->getGitFunctions()->begin(); iterator != otherFile->getGitFunctions()->end(); ++iterator ) {
        if( iterator.value()->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION ) {
            if( functions->contains( iterator.value()->getIdentifier() ) ) {
                continue;
            } else {
                functions->insert( iterator.value()->getIdentifier(), true );
            }
        }

        identifiers += compareTwoFunctions( file->getGitFunctions()->find( iterator.value()->getIdentifier() ).value(), iterator.value(), functions, file->getFilename() );
    }

    qDebug() << "Velkost indetifiers " << identifiers.size();

    delete functions;

    return identifiers;
}

QList<QString> Repository::Git::GitUtils::compareTwoFunctions( Repository::Git::GitFunction *function, Repository::Git::GitFunction *otherFunction, QMap<QString, bool>* functions, QString masterIdentifier ) {
    QList<QString> identifiers =  QList<QString>();

    QMap<QString, Repository::Git::GitFunction*> i;

    // ak existuju referencie na obe funkcie, tak skontrolujeme, ci
    if( function != i.end().value()  && otherFunction != i.end().value() ) {
        if( function->getFunctionType() == Repository::Git::GitFunctionType::GLOBALFUNCTION ) {
            if( masterIdentifier.indexOf( ".lua") < 0 ) {
                qDebug() << "End" << function->getIdentifier() << "in master" << masterIdentifier;
            }
        }

        // TODO - tu bude treba doplnit kod, na priradenie zmeny v riadku k metode

    } else if( function == i.end().value() ) {
        qDebug() << "REMOVED ->" << otherFunction->getIdentifier() << "from" << masterIdentifier;
        identifiers.append( otherFunction->getIdentifier() );
        if( otherFunction->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION && otherFunction->getFunctionCallers()->size() > 0 ) {
            for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = otherFunction->getFunctionCallers()->begin(); iterator != otherFunction->getFunctionCallers()->end(); ++iterator ) {
                identifiers.append( iterator.value()->getIdentifier() );
                qDebug() << "REMOVED ->" << iterator.value()->getIdentifier() << "from" << otherFunction->getIdentifier();
            }
        }
    } else {
        qDebug() << "ADDED ->" << function->getIdentifier() << "from" << masterIdentifier;
        identifiers.append( function->getIdentifier() );
        if( function->getFunctionType() == Repository::Git::GitFunctionType::LOCALFUNCTION && function->getFunctionCallers()->size() > 0 ) {
            for( QMap<QString, Repository::Git::GitFunction*>::iterator iterator = function->getFunctionCallers()->begin(); iterator != function->getFunctionCallers()->end(); ++iterator ) {
                identifiers.append( iterator.value()->getIdentifier() );
                qDebug() << "ADDED ->" << iterator.value()->getIdentifier() << "from" << function->getIdentifier();
            }
        }
    }

    return identifiers;
}

