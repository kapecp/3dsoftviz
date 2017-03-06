#include "Repository/Git/GitGraphUpdater.h"

#include "Data/Graph.h"
#include "Data/Node.h"
#include "Data/Edge.h"

#include "Manager/Manager.h"

#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitFile.h"
#include "GitLib/GitType.h"
#include "GitLib/GitVersion.h"

#include "Importer/GraphOperations.h"

#include <QDebug>
#include <QMapIterator>


Repository::Git::GitGraphUpdater::GitGraphUpdater( int currentVersion, Repository::Git::GitEvolutionGraph* evolutionGraph, Data::Graph* activeGraph )
	: moreVersionChanged( false ), currentVersion( currentVersion ), evolutionGraph( evolutionGraph ), activeGraph( activeGraph )
{

}

Repository::Git::GitGraphUpdater::~GitGraphUpdater()
{
	/*
	delete this->activeGraph;
	delete this->evolutionGraph;
	*/
}

bool Repository::Git::GitGraphUpdater::import()
{
	bool ok = true;

	// inicializiacia default typov
	this->getActiveGraph()->addType( "edge" );
	this->getActiveGraph()->addType( "node" );

	// inicializacia a pridanie typov hran a uzlov do aktivneho grafu
	QMap<QString, QString>* settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
	this->getActiveGraph()->addType( "file", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.RemoveNode" ) );
	this->getActiveGraph()->addType( "removedFile", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "0" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
	this->getActiveGraph()->addType( "dir", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "0" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
	this->getActiveGraph()->addType( "root", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.AuthorNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Author" ) );
	this->getActiveGraph()->addType( "author", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "0" );
	settings->insert( "color.G", "0" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.OrientedEdge" ) );
	this->getActiveGraph()->addType( "authorEdge", settings );

	settings = new QMap<QString, QString>;
	/* Visualizacia pre dokument
	settings->insert( "color.R", "0.5" );
	settings->insert( "color.G", "0.5" );
	settings->insert( "color.B", "0.5" );
	settings->insert( "color.A", "1" );
	*/
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );
	this->getActiveGraph()->addType( "Edge", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "0" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
	this->getActiveGraph()->addType( "newE", settings );

	// Ziskame pridane subory a meno autora prve verzie
	QMap<QString, Repository::Git::GitFile*> lAddedGitFiles = *this->getEvolutionGraph()->getVersion( 0 )->getGitFilesByType( Repository::Git::GitType::ADDED );
	QString lAuthor = this->getEvolutionGraph()->getVersion( 0 )->getAuthor();

	int i = 0;

	// Pre kazdu cestu suboru pridam uzly a hrany na zaklade separatora '/'
	for ( QMap<QString, Repository::Git::GitFile*>::iterator iterator = lAddedGitFiles.begin(); iterator != lAddedGitFiles.end(); ++iterator ) {
		QString line = iterator.value()->getFilepath();

		// Rozlozim cestu suboru na tokeny podla separatora '/'
		QStringList list = line.split( "/" );
		QString pom = "";

		// Pre kazdy token vytvorim jeho celu cestu od korena projektu. Prva polozka v liste bude project, druha project/nieco, atd.
		for ( int k = 0; k < list.size(); k++ ) {
			pom += list.at( k );
			list.replace( k, pom );
			pom += "/";
		}

		// Pridam cesty od korena projektu ako uzly a hrany do grafu
		addNodesToGraph( list );
		addEdgesToGraph( list );

		// Aktualizujem percento spracovania
		Manager::GraphManager::getInstance()->setProgressBarValue( static_cast<int>( ( static_cast<double>( i++ + 1 ) /  static_cast<double>( lAddedGitFiles.size() ) ) * 100 ) );
	}

	Data::Node* lAuthorNode = nullptr;
///*
	lAuthorNode = this->getActiveGraph()->findNodeByName( lAuthor );
	// Ak autor neexistuje v grafe, tak ho vytvorim
	if ( !lAuthorNode ) {
		lAuthorNode = this->getActiveGraph()->addNode( lAuthor, this->getActiveGraph()->getTypesByName( "author" ).at( 0 ) );
		lAuthorNode->setLabelText( lAuthorNode->Data::AbsNode::getName() );
		lAuthorNode->showLabel( true, false );
	}

	addAuthorEdgesToGraph( lAuthor, lAddedGitFiles );
//*/
	return ok;
}

void Repository::Git::GitGraphUpdater::nextVersion()
{
	// zistime zmenene subory v dalsej verzii, s ktorymi budeme dalej pracovat
	QMap<QString, Repository::Git::GitFile*> gitFiles = *this->getEvolutionGraph()->getVersion( this->getCurrentVersion() + 1 )->getChangedFiles();
	Data::Node* lAuthorNode = nullptr;
	QString lAuthorName = nullptr;

	// Ak ide o zmenu len cez jednu verziu, tak vymazem hrany od autora a uzol autora z grafu a pridam autora dalsej verzie
	if ( !moreVersionChanged ) {
		// Vymazanie hran od autora a vymazanie autorovho uzla
		removeAuthorEdgesFromGraph( this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor(), *this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getChangedFiles() );
		this->getActiveGraph()->removeNode( this->getActiveGraph()->findNodeByName( this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor() ) );

		// Ziskame meno autora
		lAuthorName = this->getEvolutionGraph()->getVersion( this->currentVersion + 1 )->getAuthor();

		lAuthorNode = this->getActiveGraph()->findNodeByName( lAuthorName );
		// Ak autor neexistuje v grafe, tak ho vytvorim
		if ( !lAuthorNode ) {
			lAuthorNode = this->getActiveGraph()->addNode( lAuthorName, this->getActiveGraph()->getTypesByName( "author" ).at( 0 ) );
			lAuthorNode->setLabelText( lAuthorNode->Data::AbsNode::getName() );
			lAuthorNode->showLabel( true, false );
		}
	}

	bool ok = true;

	// Pre kazdy zmeneny subor vo verzii, zistim jeho ciastocne cesty a podla typu suboru, vykonam akciu nad tymito cestami
	for ( QMap<QString, Repository::Git::GitFile*>::iterator iterator = gitFiles.begin(); iterator != gitFiles.end(); ++iterator ) {
		Repository::Git::GitFile* gitFile = iterator.value();

		QStringList lList = gitFile->getFilepath().split( "/" );
		QString lPom = "";
		bool debug = false;
		if ( gitFile->getFilepath() == "" ) {
			debug = true;
		}

		for ( int i = 0; i < lList.size(); i++ ) {
			lPom += lList.at( i );
			lList.replace( i, lPom );
			lPom += "/";
		}
		switch ( gitFile->getType() ) {
			// Ak bol pridany v dalsej verzii, tak ho pridame do grafu
			case Repository::Git::GitType::ADDED :
				if ( debug ) {
					qDebug() << "Added version" << getCurrentVersion() << getEvolutionGraph()->getVersion( getCurrentVersion() + 1 )->getCommitId();
				}
				addNodesToGraph( lList );
				addEdgesToGraph( lList );
				break;
			// Ak bol vymazany v dalsej verzii, tak ho vymazeme z grafu
			case Repository::Git::GitType::REMOVED :
				if ( debug ) {
					qDebug() << "Removed version" << getCurrentVersion() << getEvolutionGraph()->getVersion( getCurrentVersion() + 1 )->getCommitId();
				}
				this->getEvolutionGraph()->addRemovedFiles( lList, this->getCurrentVersion() );
				removeEdgesFromGraph( lList );
				removeNodesFromGraph( lList );
				break;
			case Repository::Git::GitType::MODIFIED :
				if ( debug ) {
					qDebug() << "Modified version" << getCurrentVersion() << getEvolutionGraph()->getVersion( getCurrentVersion() + 1 )->getCommitId();
				}
				modifyNodesInGraph( lList );
				break;

			case Repository::Git::GitType::NONE:
			default:
				break;
		}
	}

	// Vymaz subory, ktorych dlzka zivota prekrocila zadany lifespan
	processRemovedNodes();


	// Ak ide o zmenu len cez jednu verziu, tak pridam pre vsetky zmenene subory hrany od autora
	if ( !moreVersionChanged ) {
		addAuthorEdgesToGraph( lAuthorName, *this->getEvolutionGraph()->getVersion( this->getCurrentVersion() + 1 )->getChangedFiles() );
	}
}

void Repository::Git::GitGraphUpdater::previousVersion()
{
	// zistime si zmenene subory v aktualnej verzii, s ktorymi budeme dalej pracovat
	QMap<QString, Repository::Git::GitFile*> gitFiles = *this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getChangedFiles();
	Data::Node* lAuthorNode = nullptr;
	QString lAuthorName = nullptr;

	// Ak ide o zmenu len cez jednu verziu, tak vymazem hrany od autora a uzol autora z grafu a pridam autora dalsej verzie
	if ( !moreVersionChanged ) {
		// Vymazanie hran od autora a vymazanie autorovho uzla
		removeAuthorEdgesFromGraph( this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor(), *this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getChangedFiles() );
		this->getActiveGraph()->removeNode( this->getActiveGraph()->findNodeByName( this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor() ) );

		// Ziskame meno autora
		lAuthorName = this->getEvolutionGraph()->getVersion( this->currentVersion - 1 )->getAuthor();

		lAuthorNode = this->getActiveGraph()->findNodeByName( lAuthorName );
		// Ak autor neexistuje v grafe, tak ho vytvorim
		if ( !lAuthorNode ) {
			lAuthorNode = this->getActiveGraph()->addNode( lAuthorName, this->getActiveGraph()->getTypesByName( "author" ).at( 0 ) );
			lAuthorNode->setLabelText( lAuthorNode->Data::AbsNode::getName() );
			lAuthorNode->showLabel( true, false );
		}
	}

	bool ok = true;

	// Pre kazdy zmeneny subor vo verzii, zistim jeho ciastocne cesty a podla typu suboru, vykonam akciu nad tymito cestami
//	foreach ( Repository::Git::GitFile* gitFile, gitFiles ) {
	for ( QMap<QString, Repository::Git::GitFile*>::iterator iterator = gitFiles.begin(); iterator != gitFiles.end(); ++iterator ) {
		Repository::Git::GitFile* gitFile = iterator.value();

		QStringList lList = gitFile->getFilepath().split( "/" );
		QString lPom = "";

		for ( int i = 0; i < lList.size(); i++ ) {
			lPom += lList.at( i );
			lList.replace( i, lPom );
			lPom += "/";
		}

		switch ( gitFile->getType() ) {
			// Ak bol pridany v aktualnej verzii, tak ho vymazeme z grafu
			case Repository::Git::GitType::ADDED :
//            this->getEvolutionGraph()->addRemovedFiles( lList, this->getCurrentVersion() - 1 );
				removeEdgesFromGraph( lList );
				removeNodesFromGraph( lList );
				break;
			// Ak bol vymazany v aktualnej verzii, tak ho pridame do grafu
			case Repository::Git::GitType::REMOVED :
				addNodesToGraph( lList );
				addEdgesToGraph( lList );
				break;
			case Repository::Git::GitType::MODIFIED :
				modifyNodesInGraph( lList );
				break;
			case Repository::Git::GitType::NONE :
			default:
				break;
		}
	}

	// Vymaz subory, ktorych dlzka zivota prekrocila zadany lifespan
	processRemovedNodes();

	// Ak ide o zmenu len cez jednu verziu, tak pridam pre vsetky zmenene subory hrany od autora
	if ( !moreVersionChanged ) {
		addAuthorEdgesToGraph( lAuthorName, *this->getEvolutionGraph()->getVersion( this->getCurrentVersion() - 1 )->getChangedFiles() );
	}
}

void Repository::Git::GitGraphUpdater::changeToVersion( int toVersion )
{
	int inc;

	// Ak sucasna verzie vacsia ako verzia, ktoru chceme, tak nastavime inkrement na -1, opacne na 1.
	if ( this->getCurrentVersion() > toVersion ) {
		inc = -1;
	}
	else {
		inc = 1;
	}

	int startVersion = this->getCurrentVersion();
	Manager::GraphManager::getInstance()->showProgressBar();

	// Vymazanie hran od autora a uzla autora zaciatocnej verzie
	removeAuthorEdgesFromGraph( this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor(), *this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getChangedFiles() );
	this->getActiveGraph()->removeNode( this->getActiveGraph()->findNodeByName( this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor() ) );

	// Nastavime na true, aby sa pre kazdu verziu nevytvarali autory a ich hrany ku zmenenym uzlov
	this->moreVersionChanged = true;

	// Prechadzame po verziach az dokym dokym neprideme k volanej verzie
	for ( int i = this->getCurrentVersion(); i != toVersion; i += inc ) {
		// Ak je inkrement -1, tak sa vraciame vo verziach, opacne ideme na dalsiu verziu.
		if ( inc == -1 ) {
			this->previousVersion();
		}
		else {
			this->nextVersion();
		}

		// Nastavenie aktualnej verzie grafu a aktualizacia progressbaru
		this->setCurrentVersion( this->getCurrentVersion() + inc );
		this->getActiveGraph()->setCurrentVersion( this->getCurrentVersion() );
		Manager::GraphManager::getInstance()->setProgressBarValue( static_cast<int>( ( static_cast<double>( abs( i - startVersion ) ) / static_cast<double>( abs( toVersion - startVersion ) ) ) * 100 ) );
	}

	// Nastavime na false, aby sa pre dalsie spracovanie mohol pridat autor a jeho hrany k zmenenych uzlom.
	this->moreVersionChanged = false;

	// Ziskam meno volanej verzie a skontrolujem ci existuje v grafe.
	QString lAuthorName = this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getAuthor();
	Data::Node* lAuthorNode = this->getActiveGraph()->findNodeByName( lAuthorName );

	// Ak autor v grafe neexistuje, tak ho pridam.
	if ( !lAuthorNode ) {
		lAuthorNode = this->getActiveGraph()->addNode( lAuthorName, this->getActiveGraph()->getTypesByName( "author" ).at( 0 ) );
		lAuthorNode->setLabelText( lAuthorNode->Data::AbsNode::getName() );
		lAuthorNode->showLabel( true, false );
	}

	// Pridam hrany od autora k zmenenych uzlom vo vyslednej verzii
	addAuthorEdgesToGraph( lAuthorName, *this->getEvolutionGraph()->getVersion( this->getCurrentVersion() )->getChangedFiles() );

	Manager::GraphManager::getInstance()->closeProgressBar();
}

void Repository::Git::GitGraphUpdater::addNodesToGraph( QStringList list )
{
	bool ok = true;

	// Pre kazdu ciastocnu cestu zachytenu v liste stringov skontrolujem, ci sa uz nenachadza v grafe. Ak sa nenachadza
	// tak potom zistim typ uzla(koren adresar, subor) a nasledne danu cestu pridam do grafu ako uzol.
	for ( int i = 0; i < list.size(); i++ ) {
		bool exist = false;
		QString lNodeName = list.at( i );

		// Skontrolujem ci dany QString nie je prazdny
		ok = !lNodeName.isEmpty();

		osg::ref_ptr<Data::Node> lNode( nullptr );
		// Ak sa cesta v grafe uz nachadza, tak nastavim exist na true
		if ( this->getActiveGraph()->findNodeByName( lNodeName ) ) {
			exist = true;
		}

		Data::Type* lType( nullptr );

		QString lVal;
		// Ak je ciastocna cesta z listu na pozicie n - 1, tak ide o subor, ak na pozici 0 tak ide o root ostatne su adresare
		if ( i == list.size() - 1 ) {
			lVal = "file";
		}
		else {
			lVal = "dir";
		}

		// Ziskam typ ulozeny v grafe so ziskanym nazvom typu
		lType = this->getActiveGraph()->getTypesByName( lVal ).at( 0 );

		// Ak sa cesta v grafe nenachadzala, tak zistim typ ciastocnej cesty a pridam ho do grafu s jeho labelom. Ak už existuje,
		// tak zresetujem typ uzla na povodny typ
		if ( !exist ) {

			// Vytvorim uzol pre danu cestu s danym typom
			lNode = this->getActiveGraph()->addNode( lNodeName, lType );

			// Ak ide o korenovy uzol, tak ho fixnem na jedno miesto
			if ( QString::compare( lVal, "root" ) == 0 ) {
				lNode->setFixed( true );
			}

			// Nastavim label a pridam do mnoziny uzlov v grafe
			lNode->setLabelText( lNodeName );
			lNode->showLabel( true, false );
		}
		else {
			Data::Node* node = this->getActiveGraph()->findNodeByName( lNodeName );
			node->setType( lType );
			node->reloadConfig();
			node->showLabel( true, false );
		}
	}

	// prepoj novo vzniknute uzly s rootom
	osg::ref_ptr<Data::Node> lRoot = this->getActiveGraph()->findNodeByName( this->getEvolutionGraph()->getFilePath() );
	if ( !lRoot ) {
		lRoot = this->activeGraph->addNode( this->getEvolutionGraph()->getFilePath(), this->getActiveGraph()->getTypesByName( "root" ).at( 0 ) );
		lRoot->setFixed( true );
		lRoot->setLabelText( this->getEvolutionGraph()->getFilePath() );
		lRoot->showLabel( true, false );
	}
}

void Repository::Git::GitGraphUpdater::addEdgesToGraph( QStringList list )
{
	bool ok = true;

	Data::Type* lType = this->getActiveGraph()->getTypesByName( "Edge" ).at( 0 );

	// Pre ciastocne cesty vytvorim hranu medzi dvojicami i a i+1, vtedy ak dana hrana v grafe neexistuje
	// a uzly, ktore hranu definuju, existuju v grafe.
	for ( int i = 0; i < list.size() - 1; i++ ) {
		bool exist = false;
		osg::ref_ptr<Data::Edge> lEdge( nullptr );

		QString lNodeNameFrom = list.at( i );
		QString lNodeNameTo = list.at( i + 1 );

		// Vytvorim nazov hrany kombinaciou uzlov, ktore definuju hranu
		QString lEdgeName = lNodeNameFrom + lNodeNameTo;

		// Pre kazdu hranu sa inkrementuje pocitadlo, kolkokrat je dana hrana naviazana v grafe.
		if ( i + 1 == list.size() - 1 && this->getActiveGraph()->getEdgeOccurence().value( lEdgeName ) ) {
			qDebug() << "Pridavam zdvojenu hodnotu pre" << lEdgeName;
//            this->getActiveGraph()->removeEdgeOccurence( lEdgeName );
		}
		this->getActiveGraph()->addEdgeOccurence( lEdgeName );

		// Ak sa dana hrana v grafe nachadza, tak potom nastavim exist na true, co zamedzi dalsie spracovanie.
		if ( this->getActiveGraph()->findEdgeByName( lEdgeName ) ) {
			exist =  true;
		}

		// Ak sa hrana v grafe nenachadza, tak zistim, ci jej uzly sa nachadzaju v grafe. Ak nie, tak sa hrana neprida.
		// V opacnom pripade sa prida.
		if ( !exist ) {
			if ( ok ) {
				// Ak sa uzol nenachadza v grafe, tak skonci dalsie spracovanie
				if ( this->getActiveGraph()->findNodeByName( lNodeNameFrom ) != nullptr ) {
					ok = true;
				}
				else {
					ok = false;
				}
			}

			if ( ok ) {
				// Ak sa uzol nenachadza v grafe, tak skonci dalsie spracovanie
				if ( this->getActiveGraph()->findNodeByName( lNodeNameTo ) != nullptr ) {
					ok = true;
				}
				else {
					ok = false;
				}
			}

			bool oriented =  false;

			// Ak sa obidva uzly nachadzaju v grafe, tak vytvor medzi nimi hranu.
			if ( ok ) {
				lEdge = this->getActiveGraph()->addEdge( lEdgeName, this->getActiveGraph()->findNodeByName( lNodeNameFrom ), this->getActiveGraph()->findNodeByName( lNodeNameTo ), lType, oriented );
			}
		}
	}

	// prepojim root s prvym uzlom
	QString lEdgeName = "root" + list.at( 0 );
	this->getActiveGraph()->addEdgeOccurence( lEdgeName );
	if ( !this->getActiveGraph()->findEdgeByName( lEdgeName ) ) {
		bool ok = true;

		if ( ok ) {
			// Ak sa uzol nenachadza v grafe, tak skonci dalsie spracovanie
			if ( this->getActiveGraph()->findNodeByName( this->getEvolutionGraph()->getFilePath() ) != nullptr ) {
				ok = true;
			}
			else {
				ok = false;
			}
		}

		if ( ok ) {
			// Ak sa uzol nenachadza v grafe, tak skonci dalsie spracovanie
			if ( this->getActiveGraph()->findNodeByName( list.at( 0 ) ) != nullptr ) {
				ok = true;
			}
			else {
				ok = false;
			}
		}

		bool oriented =  false;

		// Ak sa obidva uzly nachadzaju v grafe, tak vytvor medzi nimi hranu.
		if ( ok ) {
			this->getActiveGraph()->addEdge( lEdgeName, this->getActiveGraph()->findNodeByName( this->getEvolutionGraph()->getFilePath() ), this->getActiveGraph()->findNodeByName( list.at( 0 ) ), lType, oriented );
		}
	}
}

void Repository::Git::GitGraphUpdater::addAuthorEdgesToGraph( QString authorName, QMap<QString, Repository::Git::GitFile*> gitFiles )
{
	// Pre kazdy subor zisti, ci existuje hrana medzi autorom a uzlom, ak nie, tak pridam hranu do grafu
	for ( QMap<QString, Repository::Git::GitFile*>::iterator iterator = gitFiles.begin(); iterator != gitFiles.end(); ++iterator ) {
		Repository::Git::GitFile* gitFile = iterator.value();

		// Vyskladam nazov hrany spojenim mena autora a cesty k uzlu
		QString lEdgeName = authorName + gitFile->getFilepath();
		bool exist = true;

		if ( exist ) {
			// Ak sa uzol k danemu suboru nachadza v grafe, tak je true, inak false.
			if ( this->getActiveGraph()->findNodeByName( gitFile->getFilepath() ) != nullptr ) {
				exist = true;
			}
			else {
				exist = false;
			}
		}

		// Ak sa cielovy uzol nachadza v grafe a v grafe neexistuje dana hrana
		if ( exist && !this->getActiveGraph()->findEdgeByName( lEdgeName ) ) {
			Data::Type* lType = this->getActiveGraph()->getTypesByName( "authorEdge" ).at( 0 );
			this->getActiveGraph()->addEdge( lEdgeName, this->getActiveGraph()->findNodeByName( authorName ), this->getActiveGraph()->findNodeByName( gitFile->getFilepath() ), lType, true );
		}
	}
}

void Repository::Git::GitGraphUpdater::removeAuthorEdgesFromGraph( QString authorName, QMap<QString, Repository::Git::GitFile*> gitFiles )
{
	// Pre kazdy subor zisti, odstran hranu medzi autorom a uzlov, ak existuje v grafe
	for ( QMap<QString, Repository::Git::GitFile*>::iterator iterator = gitFiles.begin(); iterator != gitFiles.end(); ++iterator ) {
		Repository::Git::GitFile* gitFile = iterator.value();
		// Vyskladam nazov hrany spojenim mena autora a cesty k uzlu
		QString lEdgeName = authorName + gitFile->getFilepath();

		Data::Edge* lEdge = this->getActiveGraph()->findEdgeByName( lEdgeName );
		// Ak sa hrana nachadza v grafe, tak vymazem danu hranu
		if ( lEdge ) {
			this->getActiveGraph()->removeEdge( lEdge );
		}
	}

}

void Repository::Git::GitGraphUpdater::removeNodesFromGraph( QStringList list )
{
	// Pre ciastocne cesty zisti, ci sa hrana uz v grafe nenachadza a nasledne vymaz cielovy uzol
	for ( int i =  0; i < list.size() - 1; i++ ) {
		// Ziskanie cesty zdrojoveho a cieloveho uzla
		QString lNodeNameFrom = list.at( i );
		QString lNodeNameTo = list.at( i + 1 );

		// Vyskladam nazov hrany spojenim zdrojoveho a cieloveho uzla
		QString lEdgeName = lNodeNameFrom + lNodeNameTo;

		// Ak je rozdiel sucasnej verzie a verzie vymazania mensi ako lifespan, tak nastavime passedLifespan na true
		// a uzol nemoze byt vymazany.
		bool passedLifespan = false;
		if ( this->getEvolutionGraph()->getRemovedFiles().contains( lNodeNameTo ) ) {
			int difference = this->getCurrentVersion() - this->getEvolutionGraph()->getRemovedFiles().value( lNodeNameTo );
			if ( difference < this->getEvolutionGraph()->getLifespan() ) {
				passedLifespan = true;

				// Ak ide o subor, tak zmenime obrazok uzla.
				if ( i + 1 == list.size() - 1 ) {
					Data::Node* node = this->getActiveGraph()->findNodeByName( lNodeNameTo );
					if ( node ) {
						node->setType( this->getActiveGraph()->getTypesByName( "removedFile" ).at( 0 ) );
						node->reloadConfig();
						node->showLabel( true, false );
					}
				}
			}
		}

		// Ak hrana v grafe uz neexistuje a sucasne lifespan uzla prekrocil treshold, tak vymaz cielovy uzol
		if ( this->getActiveGraph()->getEdgeOccurence().value( lEdgeName ) <= 0 && !passedLifespan ) {
			this->getActiveGraph()->removeNode( this->getActiveGraph()->findNodeByName( lNodeNameTo ) );
			this->getEvolutionGraph()->getRemovedFiles().remove( lNodeNameTo );
		}
	}

	QString lEdgeName = "root" + list.at( 0 );

	// Ak je rozdiel sucasnej verzie a verzie vymazania mensi ako lifespan, tak nastavime passedLifespan na true
	// a uzol nemoze byt vymazany.
	bool passedLifespan = false;
	if ( this->getEvolutionGraph()->getRemovedFiles().contains( list.at( 0 ) ) ) {
		int difference = this->getCurrentVersion() - this->getEvolutionGraph()->getRemovedFiles().value( list.at( 0 ) );
		if ( difference < this->getEvolutionGraph()->getLifespan() ) {
			passedLifespan = true;

			// Ak ide o subor, tak zmenime obrazok uzla.
			if ( 1 == list.size() - 1 ) {
				Data::Node* node = this->getActiveGraph()->findNodeByName( list.at( 0 ) );
				if ( node ) {
					node->setType( this->getActiveGraph()->getTypesByName( "removedFile" ).at( 0 ) );
					node->reloadConfig();
					node->showLabel( true, false );
				}
			}
		}
	}

	// Ak hrana v grafe uz neexistuje a sucasne lifespan uzla prekrocil treshold, tak vymaz cielovy uzol
	if ( this->getActiveGraph()->getEdgeOccurence().value( lEdgeName ) <= 0 && !passedLifespan ) {
		this->getActiveGraph()->removeNode( this->getActiveGraph()->findNodeByName( list.at( 0 ) ) );
		this->getEvolutionGraph()->getRemovedFiles().remove( list.at( 0 ) );
	}

}

void Repository::Git::GitGraphUpdater::removeEdgesFromGraph( QStringList list )
{
	// Pre ciastocne cesty zisti, ci sa hrana uz v grafe nenachadza a nasledne vymaz cielovy uzol
	for ( int i = 0; i < list.size() - 1; i++ ) {
		// Ziskanie cesty zdrojoveho a cieloveho uzla
		QString lNodeNameFrom = list.at( i );
		QString lNodeNameTo =  list.at( i + 1 );

		// Vyskladam nazov hrany spojenim zdrojoveho a cieloveho uzla
		QString lEdgeName =  lNodeNameFrom + lNodeNameTo;

		// Ak je rozdiel sucasnej verzie a verzie vymazania mensi ako lifespan, tak nastavime passedLifespan na true
		// a uzol nemoze byt vymazany.
		bool passedLifespan = false;
		int difference = this->getCurrentVersion() - this->getEvolutionGraph()->getRemovedFiles().value( lNodeNameTo );
		if ( difference < this->getEvolutionGraph()->getLifespan() ) {
			passedLifespan = true;
		}

		// Ak hrana v grafe uz neexistuje a sucasne lifespan uzla prekrocil treshold, tak vymaz hranu
		if ( !this->getActiveGraph()->removeEdgeOccurence( lEdgeName ) && !passedLifespan ) {
			this->getActiveGraph()->removeEdge( this->getActiveGraph()->findEdgeByName( lEdgeName ) );
		}
	}

	QString lEdgeName = "root" + list.at( 0 );
	if ( this->getActiveGraph()->findEdgeByName( lEdgeName ) ) {

		// Ak je rozdiel sucasnej verzie a verzie vymazania mensi ako lifespan, tak nastavime passedLifespan na true
		// a uzol nemoze byt vymazany.
		bool passedLifespan = false;
		int difference = this->getCurrentVersion() - this->getEvolutionGraph()->getRemovedFiles().value( list.at( 0 ) );
		if ( difference < this->getEvolutionGraph()->getLifespan() ) {
			passedLifespan = true;
		}

		// Ak hrana v grafe uz neexistuje a sucasne lifespan uzla prekrocil treshold, tak vymaz hranu
		if ( !this->getActiveGraph()->removeEdgeOccurence( lEdgeName ) && !passedLifespan ) {
			this->getActiveGraph()->removeEdge( this->getActiveGraph()->findEdgeByName( lEdgeName ) );
		}
	}
}

void Repository::Git::GitGraphUpdater::modifyNodesInGraph( QStringList list )
{

}

void Repository::Git::GitGraphUpdater::processRemovedNodes()
{
	QMapIterator<QString, int> iter( this->getEvolutionGraph()->getRemovedFiles() );
	// Prejdeme vsetky subory, ktore maju byt vymazane a zistime, ci ma uzol aj s hranami
	// ostat v grafe, ak nie tak ho vymaz aj hranami.
	while ( iter.hasNext() ) {
		iter.next();

		bool debug = false;
		if ( iter.key() == "" ) {
			debug = true;
		}

		QStringList lList = iter.key().split( "/" );
		QString lPom = "";

		// Ziskam vsetky ciastkove cesty v danej ceste suboru
		for ( int i = 0; i < lList.size(); i++ ) {
			lPom += lList.at( i );
			lList.replace( i, lPom );
			lPom += "/";
		}

		for ( int i = 0; i < lList.size() - 1; i++ ) {
			// Ziskanie cesty zdrojoveho a cieloveho uzla
			QString lNodeNameFrom = lList.at( i );
			QString lNodeNameTo =  lList.at( i + 1 );

			// Vyskladam nazov hrany spojenim zdrojoveho a cieloveho uzla
			QString lEdgeName =  lNodeNameFrom + lNodeNameTo;

			// Ak je rozdiel sucasnej verzie a verzie vymazania mensi ako lifespan, tak nastavime passedLifespan na true
			// a uzol nemoze byt vymazany.
			bool passedLifespan = false;
			int difference = this->getCurrentVersion() - this->getEvolutionGraph()->getRemovedFiles().value( lNodeNameTo );
			if ( difference < this->getEvolutionGraph()->getLifespan() ) {
				passedLifespan = true;
			}

			if ( debug ) {
				qDebug() << getCurrentVersion() << lEdgeName << this->getEvolutionGraph()->getRemovedFiles().value( lNodeNameTo ) << this->getActiveGraph()->getEdgeOccurence().value( lEdgeName );
			}

			// Ak uz nie su naviazane ziadne hrany k danemu cielovemu uzlu, tak ho vymazem, ak je to povolene
			if ( this->getActiveGraph()->getEdgeOccurence().value( lEdgeName ) <= 0 ) {
				// Ak je mozne ho vymazat, tak vymazeme hranu, uzol a odstranime uzol zo zoznamu uzlov na vymazanie
				if ( !passedLifespan ) {
					this->getActiveGraph()->removeEdge( this->getActiveGraph()->findEdgeByName( lEdgeName ) );
					this->getActiveGraph()->removeNode( this->getActiveGraph()->findNodeByName( lNodeNameTo ) );
					this->getEvolutionGraph()->removeRemovedFiles( lNodeNameTo );
				}
			}
			// V pripade, ze k danemu uzlu este smeruju nejake hrany, tak uzol odstranime zo zoznamu uzlov na vymazanie
			else {
				this->getEvolutionGraph()->removeRemovedFiles( lNodeNameTo );
			}
		}

	}
}
