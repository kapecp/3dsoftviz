#include "Git/GitFileLoader.h"

#include "Git/GitVersion.h"
#include "Git/GitFile.h"
#include "Git/GitFileDiffBlock.h"
#include "Git/GitFileDiffBlockLine.h"

#include <QTemporaryFile>
#include <QDir>
#include <QProcess>
#include <QTextStream>
#include <QtCore/QRegExp>
#include <QDebug>

Git::GitFileLoader::GitFileLoader()
{

	// Inicializacia premennych na defaultne hodnoty
	this->filePath = "";
	this->extensions = "";
}

Git::GitFileLoader::GitFileLoader( QString filepath, QString extensions ) : filePath( filepath ), extensions( extensions )
{
}

QList<Git::GitVersion*> Git::GitFileLoader::getDataAboutGit()
{

	// Inicializacia lokalnych premmenych
	bool ok = true;
	QList<Git::GitVersion*> versions = QList<Git::GitVersion*>();;
	QString lFilePath = this->filePath;
	QString lGitCommand = "git log --raw --name-status --reverse --date=short --pretty=format:\"%H%n%an%n%ad\"";

	// Vykona command, vystup ulozi do temp suboru a vrati cestu k temp suboru
	QString lTmp = makeTmpFileFromCommand( lGitCommand, lFilePath );

	QFile file( lTmp );

	// Kontrola na uspesnost ulozenia vystupu commandu do suboru, ak sa podarilo tak "ok" ostane true
	if ( !file.size() ) {
		ok = false;
	}

	// Ak je velkost suboru vacsia ako 0 a podarilo sa otvorit subor, tak z neho citame az do konca
	if ( ok  && file.open( QIODevice::ReadOnly ) ) {
		QTextStream reader( &file );
		QString line;

		// Pokial neprideme na koniec suboru tak citame po riadkoch
		while ( !reader.atEnd() ) {

			// Prvy riadok vstupnu je vzdy commit identifikator
			line = reader.readLine();
			Git::GitVersion* version = new Git::GitVersion( line );

			// Druhy riadok je meno autora
			line = reader.readLine();
			version->setAuthor( line );

			// Treti riadok je datum commitu
			line = reader.readLine();
			version->setDate( line );

			bool read = true;
			// Citame dokym nepride prvy prazdny riadok, co znaci koniec commitu
			while ( read ) {
				line = reader.readLine();

				// Vytvorime si regex na pridane(A), modifikovane(M) a vymazane(D) subory
				QRegExp A( "^A\\s" );
				QRegExp M( "^M\\s" );
				QRegExp D( "^D\\s" );

				// Ak sa najde zhoda s jednym z regexov, tak nahradime regex za "project/" (co bude root pre vsetky subory), vytvorime
				// subor s jeho nazvom, celou cestou a typom podla zisteneho regexu a pridame ho do verzie.
				// V pripade, ze sa nenasiel ani jeden regex, tak skontrolujeme dlzku nacitaneho riadku. Ak je rovny 40, tak ide
				// o viacnasobne uvedenie identifikatoru a autora, vtedy precitame a zahodime. Ak nie je rovny 40, skonci vytvaranie verzie.
				if ( A.indexIn( line ) != -1 ) {
					line = line.replace( A, "projekt/" );
					Git::GitFile* file = new Git::GitFile( line.mid( line.lastIndexOf( "/" ) + 1 ), line, Git::GitType::ADDED );
					version->addChangedFile( file );
				}
				else if ( M.indexIn( line ) != -1 ) {
					line = line.replace( M, "projekt/" );
					Git::GitFile* file = new Git::GitFile( line.mid( line.lastIndexOf( "/" ) + 1 ), line, Git::GitType::MODIFIED );
					version->addChangedFile( file );
				}
				else if ( D.indexIn( line ) != -1 ) {
					line = line.replace( D, "projekt/" );
					Git::GitFile* file = new Git::GitFile( line.mid( line.lastIndexOf( "/" ) + 1 ), line, Git::GitType::REMOVED );
					version->addChangedFile( file );
				}
				else if ( line.length() == 40 ) {
					reader.readLine();
					reader.readLine();
				}
				else {
					read = false;
				}
			}

			// Nastavim zmenene subory podla zadanych filtrovacich kriterii z appConfig
			version->setChangedFiles( version->getGitFilesByExtension( this->extensions ) );

			// Ak je velkost zmenenych suborov vacsia ako 0, tak pridam verziu
			if ( version->getChangedFiles().size() ) {
				versions.append( version );
			}
		}
	}

	// Ak sa mi nepodari vymazat subor, tak vypisem jeho nazov
	if ( !file.remove() ) {
		qDebug() << "Nepodarilo sa odstranit temp subor" << file.fileName();
	}

	// Vratim list verzii, ktore splnaju filtrovacie kriteria
	return versions;
}

QString Git::GitFileLoader::makeTmpFileFromCommand( QString command, QString filepath )
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
		QString lCommand = QString( command );
        process.start( lCommand );
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

bool Git::GitFileLoader::changeDir( QString path )
{
	return QDir::setCurrent( path );
}

bool Git::GitFileLoader::existGit( QString path )
{
	return QDir( QDir::toNativeSeparators( path + "/.git" ) ).exists();
}

void Git::GitFileLoader::readGitShowFile( QString tmpFile, Git::GitFile* gitFile )
{
	bool ok = true;
	QFile lFile( tmpFile );

	// Kontrola na uspesnost ulozenia vystupu commandu do suboru, ak sa podarilo tak "ok" ostane true
	if ( !lFile.size() ) {
		ok = false;
	}

	// Ak je velkost suboru vacsia ako 0 a podarilo sa otvorit subor, tak z neho citame az do konca
	if ( ok && lFile.open( QIODevice::ReadOnly ) ) {
		QTextStream lReader( &lFile );
		int lLineNumber = 1;
		Git::GitFileDiffBlock* diffBlock = new Git::GitFileDiffBlock();

		// Citam do konca a pridavam jednotlive riadky typu ADDED
		while ( !lReader.atEnd() ) {
			QString lLine = lReader.readLine();
			Git::GitFileDiffBlockLine* lDiffBlockLine = new Git::GitFileDiffBlockLine( lLine, lLineNumber++, Git::GitType::ADDED );
			diffBlock->incAddCount();
			diffBlock->addGitFileDiffBlockLine( lDiffBlockLine );
		}

		// Pridam diff blok do suboru
		gitFile->addGitFileDiffBlock( diffBlock );
	}

	// Ak sa mi nepodari vymazat subor, tak vypisem jeho nazov
	if ( !lFile.remove() ) {
		qDebug() << "Nepodarilo sa odstranit temp subor" << lFile.fileName();
	}
}

void Git::GitFileLoader::readGitDiffFile( QString tmpFile, Git::GitFile* gitFile )
{
	bool ok = true;
	QFile lFile( tmpFile );

	// Kontrola na uspesnost ulozenia vystupu commandu do suboru, ak sa podarilo tak "ok" ostane true
	if ( !lFile.size() ) {
		ok = false;
	}

	// Ak je velkost suboru vacsia ako 0 a podarilo sa otvorit subor, tak z neho citame az do konca
	if ( ok && lFile.open( QIODevice::ReadOnly ) ) {
		QTextStream lReader( &lFile );
		QString lLine;

		Git::GitFileDiffBlock* diffBlock = NULL;

		// Defaultny pocet riadkov diff hlavicky
		int iter = 4;

		// Preskocime hlavicku diffu, ktora ma default pocet riadkov 4
		for ( int i = 0; i < iter; i++ ) {
			lLine = lReader.readLine();

			// Ak sa v diff hlavicke nachadza retazec "new file mode" alebo "deleted file mode" treba preskocit
			// o jeden riadok naviac
			if ( lLine.indexOf( "new file mode" ) >= 0 ) {
				iter++;
			}
			if ( lLine.indexOf( "deleted file mode" ) >= 0 ) {
				iter++;
			}
		}

		int addPos = 0, removePos = 0;

		// Pokial neprideme na koniec suboru tak citame po riadkoch
		while ( !lReader.atEnd() ) {
			lLine = lReader.readLine();
			QRegExp regex;
			Git::GitFileDiffBlockLine* blockLine;

			// Podla hodnoty prveho znaku z nacitaneho riadku rozhodujem, ako dalej spracujem dany riadok
			switch ( lLine.at( 0 ).toAscii() ) {

				// Ak prvy znak bol '@', tak zistim zaciatocnu poziciu pridania a pocet riadkov oblasti pridania,
				// zaciatok mazania a pocet riadkov oblasti mazania. Nasledne tieto udaje ulozim do diff bloku.
				case '@':
					// Preskoci poslednu instanciu a prida ju az, ked sa precita cely subor
					if ( diffBlock != NULL ) {
						gitFile->addGitFileDiffBlock( diffBlock );
					}

					int firstComma, secondComma, middleSpace, endSpace;

					diffBlock = new Git::GitFileDiffBlock();

					// zistenie pozicii pre ciarky a medzery pre @@ -0,0 +0,0
					firstComma = lLine.indexOf( ",", 4 );
					secondComma = lLine.indexOf( ",", firstComma + 1 );
					middleSpace = lLine.indexOf( " ", firstComma );
					endSpace = lLine.indexOf( " ", middleSpace + 1 );

					// Nastavenie zaciatkov a poctu riadko oblasti
					diffBlock->setRemoveStart( lLine.mid( 4, firstComma - 4 ).toInt( false, 10 ) );
					diffBlock->setRemoveCount( lLine.mid( firstComma + 1, middleSpace - ( firstComma + 1 ) ).toInt( false, 10 ) );
					diffBlock->setAddStart( lLine.mid( middleSpace + 2, secondComma - ( middleSpace + 2 ) ).toInt( false, 10 ) );
					diffBlock->setAddCount( lLine.mid( secondComma + 1, endSpace - ( secondComma + 1 ) ).toInt( false, 10 ) );

					// Inicializacia pocitadiel pozicie pre pridanie a odstranenie
					addPos = diffBlock->getAddStart();
					removePos = diffBlock->getRemoveStart();

					break;

				// Ak prvy znak bol '+', tak tento znak odstranim, vytvorim diff blok riadok s typom ADDED, pridam ho do diff bloku
				// a zvacsim pocitadlo pozicie pre pridanie
				case '+':

					regex = QRegExp( "^\\+" );

					if ( regex.indexIn( lLine ) != -1 ) {
						lLine = lLine.replace( regex, "" );
					}

					blockLine = new Git::GitFileDiffBlockLine( lLine, addPos++ , Git::GitType::ADDED );
					diffBlock->addGitFileDiffBlockLine( blockLine );

					break;

				// Ak prvy znak bol '-', tak tento znak odstranim, vytvorim diff blok riadok s typom REMOVED, pridam ho do diff bloku
				// a zvacsim pocitadlo pozicie pre odstranenie
				case '-':

					regex = QRegExp( "^-" );

					if ( regex.indexIn( lLine ) != -1 ) {
						lLine = lLine.replace( regex, "" );
					}

					blockLine = new Git::GitFileDiffBlockLine( lLine, removePos++ , Git::GitType::REMOVED );
					diffBlock->addGitFileDiffBlockLine( blockLine );

					break;

				// Ak prvy znak bol ' ', tak tento znam odstranim, vytvorim diff block riadok s typom NONE, pridam ho do diff bloku
				// a zvacsim obidve pocitadla, pricom typ NONE ukladam s pozicou pocitadla pre pridanie
				case ' ':

					regex = QRegExp( "^ " );

					if ( regex.indexIn( lLine ) != -1 ) {
						lLine = lLine.replace( regex, "" );
					}

					removePos++;
					blockLine = new Git::GitFileDiffBlockLine( lLine, addPos++ , Git::GitType::NONE );
					diffBlock->addGitFileDiffBlockLine( blockLine );

					break;
				default:
					break;
			}
		}

		// Pridam poslednu iteraciu diff bloku do suboru
		if ( diffBlock != NULL ) {
			gitFile->addGitFileDiffBlock( diffBlock );
		}
	}

	// Ak sa mi nepodari vymazat subor, tak vypisem jeho nazov
	if ( !lFile.remove() ) {
		qDebug() << "Nepodarilo sa odstranit temp subor" << lFile.fileName();
	}
}


void Git::GitFileLoader::getDiffInfo( Git::GitFile* gitFile, QString currentCommitId, QString oldCommitId )
{
	QString lCommand;
	QString lFilePath = this->filePath;
	QString lFile = gitFile->getFilepath().replace( "projekt/", "" );

	// Ak bol pridany subor pridany, nemame jeho predchadzajucu verziu, preto nacitame celu verziu suboru
	if ( gitFile->getType() == Git::GitType::ADDED ) {
		lCommand = QString( "git show " + currentCommitId + ":" + lFile );
	}
	else {
		lCommand = QString( "git diff -u " + oldCommitId + " " + currentCommitId + " -- " + lFile );
	}

	// Vykona command, vystup ulozi do temp suboru a vrati cestu k temp suboru
	QString lTmp = makeTmpFileFromCommand( lCommand, lFilePath );

	// Ak je typ suboru ADDED, tak nema predchadzajucu verziu a nacitam celu verziu suboru.
	// Ak nie, tak nacitam diff bloky daneho suboru
	if ( gitFile->getType() == Git::GitType::ADDED ) {
		readGitShowFile( lTmp, gitFile );
	}
	else {
		readGitDiffFile( lTmp, gitFile );
	}
}

