/*!
 * Manager.cpp
 * Projekt 3DVisual
 */

#include "Manager/Manager.h"
#include "Manager/ImportInfoHandlerImpl.h"

#include "Model/DB.h"
#include "Model/GraphDAO.h"
#include "Model/GraphLayoutDAO.h"

#include "Importer/ImporterContext.h"
#include "Importer/ImporterFactory.h"
#include "Importer/StreamImporter.h"

#include "Network/Server.h"

#include "Core/Core.h"

#include "Data/Graph.h"
#include "Data/GraphLayout.h"

#include "GitLib/GitFileLoader.h"
#include "GitLib/GitVersion.h"
#include "GitLib/GitFile.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitEvolutionGraphManager.h"
#include "Repository/Git/GitGraphUpdater.h"

#include "Layout/LayoutThread.h"
#include "QOSG/MessageWindows.h"

#include <QFile>
#include <qfileinfo.h>

#include <memory>

Manager::GraphManager* Manager::GraphManager::manager;

Manager::GraphManager::GraphManager()
{
	manager = this;

	//konfiguracia/vytvorenie DB
	this->activeGraph = NULL;
	this->activeEvolutionGraph = NULL;
	this->db = new Model::DB();
	noDatabaseFind=false;
}

Manager::GraphManager::~GraphManager()
{
	delete this->getActiveEvolutionGraph();
	this->activeEvolutionGraph = NULL;
	delete this->db;
	this->db = NULL;
}

Data::Graph* Manager::GraphManager::loadGraph( QString filepath )
{
	//otvaranie suboru
	bool ok = true;

	AppCore::Core::getInstance()->thr->pauseAllAlg();
	AppCore::Core::getInstance()->messageWindows->showProgressBar();

	// vytvorenie infoHandler
	std::shared_ptr<Importer::ImportInfoHandler> infoHandler( NULL );
	if ( ok ) {
		infoHandler.reset( new ImportInfoHandlerImpl );
	}

	// meno a pripona
	QString name;
	QString extension;

	if ( ok ) {
		QFileInfo fileInfo( filepath );
		name = fileInfo.fileName();
		extension = fileInfo.suffix();
	}

	// nastavenie importera
	std::shared_ptr<Importer::StreamImporter> importer( NULL );
	if ( ok ) {
		bool importerFound;

		ok =
			Importer::ImporterFactory::createByFileExtension(
				importer,
				importerFound,
				extension
			)
			&&
			importerFound
			;

		infoHandler->reportError( ok, "No suitable importer has been found for the file extension." );
	}

	// vytvorenie nacitavaneho streamu
	std::shared_ptr<QIODevice> stream( NULL );
	if ( ok ) {
		stream.reset( new QFile( filepath ) );
	}

	if ( ok ) {
		ok = ( stream->open( QIODevice::ReadOnly ) );

		infoHandler->reportError( ok, "Unable to open the input file." );
	}

	// vytvorenie noveho grafu
	std::shared_ptr<Data::Graph> newGraph( NULL );
	if ( ok ) {
		newGraph.reset( this->createGraph( name ) );
		ok = ( newGraph.get() != NULL );
	}



	// vytvorenie kontextu
	std::shared_ptr<Importer::ImporterContext> context( NULL );
	if ( ok ) {
		context.reset(
			new Importer::ImporterContext(
				*stream,
				*newGraph,
				*infoHandler
			)
		);
	}

	// spustenie importera
	if ( ok ) {
		ok = importer->import( *context );
	}

	// ukoncenie streamu
	if ( stream.get() != NULL ) {
		stream->close();
	}

	// pridanie obmedzovaca
	if ( ok ) {
		Data::GraphLayout* gLay = newGraph->addLayout( "new Layout" );
		newGraph->selectLayout( gLay );
	}

	// ukoncenie streamu
	if ( stream.get() != NULL ) {
		stream->close();
	}

	// nastavenie aktivneho grafu
	if ( ok ) {
		// ak uz nejaky graf mame, tak ho najprv sejvneme a zavrieme
		if ( this->activeGraph != NULL ) {
			//this->saveGraph(this->activeGraph);
			this->closeGraph( this->activeGraph.get() );
		}
		//this->activeGraph = newGraph.release();
		this->activeGraph = newGraph;
		newGraph = nullptr;
	}


	if ( ok ) {
		// robime zakladnu proceduru pre restartovanie layoutu
		AppCore::Core::getInstance()->restartLayout();
	}

	AppCore::Core::getInstance()->messageWindows->closeProgressBar();

	Network::Server* server = Network::Server::getInstance();
	server -> sendGraph();

	return ( ok ? this->activeGraph.get() : NULL );
}


void Manager::GraphManager::saveActiveGraphToDB()
{

	if ( this->getActiveGraph() == NULL ) {
		qDebug() << "[Manager::GraphManager::saveActiveGraphToDB()] There is no active graph loaded";
		return;
	}

	// test ci graf je uz v DB
	if ( !noDatabaseFind &&  this->getActiveGraph()->isInDB() ) {
		qDebug() << "[Manager::GraphManager::saveActiveGraphToDB] Graph is in DB allready";
		return;
	}

	// ulozenie grafu to do databazy
	if ( !noDatabaseFind && db->tmpGetConn() != NULL  &&  db->tmpGetConn()->open() ) {

		// vytvorenie prazdneho grafu v databaze
		Model::GraphDAO::addGraph( this->getActiveGraph(), this->db->tmpGetConn() );

		//ulozime obycajne uzly a hrany
		this->activeGraph->saveGraphToDB( db->tmpGetConn(), this->activeGraph.get() );
		//nastavime meno grafu podla nazvu suboru
		Model::GraphDAO::setGraphName( this->activeGraph->getId(), this->activeGraph->getName(), db->tmpGetConn() );
		//ulozime a nastavime default layout
		Data::GraphLayout* layout = Model::GraphLayoutDAO::addLayout( "original layout", this->activeGraph.get(), db->tmpGetConn() );
		this->activeGraph->selectLayout( layout );
		//este ulozit meta uzly, hrany a pozicie vsetkych uzlov
		this->activeGraph->saveLayoutToDB( db->tmpGetConn(), this->activeGraph.get() );

	}
	else {
		qDebug() << "[Manager::GraphManager::saveActiveGraphToDB] Connection to DB not opened";
		noDatabaseFind=true;
	}

}

void Manager::GraphManager::saveActiveLayoutToDB( const QString layoutName )
{

	if ( this->getActiveGraph() == NULL ) {
		qDebug() << "[Manager::GraphManager::saveActiveLayoutToDB()] There is no active graph loaded";
		return;
	}

	if ( !noDatabaseFind &&  db->tmpGetConn() != NULL  &&  db->tmpGetConn()->open() ) {

		// ulozenie grafu do DB ak este nie je
		if ( this->getActiveGraph()->isInDB() == false ) {
			this->saveActiveGraphToDB();
		}

		Data::GraphLayout* layout = Model::GraphLayoutDAO::addLayout( layoutName, this->getActiveGraph(), db->tmpGetConn() );

		this->getActiveGraph()->selectLayout( layout );
		this->getActiveGraph()->saveLayoutToDB( db->tmpGetConn(), this->getActiveGraph() );

	}
	else {
		qDebug() << "[Manager::GraphManager::saveActiveLayoutToDB()] Connection to DB not opened";
		noDatabaseFind=true;
	}
}

Data::Graph* Manager::GraphManager::createNewMatrixGraph( QString name )
{
	bool ok = true;

	AppCore::Core::getInstance()->thr->pauseAllAlg();

	// vytvor graf
	std::shared_ptr<Data::Graph> newGraph( NULL );
	if ( ok ) {
		newGraph.reset( this->createGraph( name ) );
		ok = ( newGraph.get() != NULL );
	}

	// pridaj rozlozenie
	if ( ok ) {
		Data::GraphLayout* gLay = newGraph->addLayout( "new Layout" );
		newGraph->selectLayout( gLay );
	}

	// nastav aktivny graf a zmaz predosly
	if ( ok ) {
		if ( this->activeGraph != NULL ) {
			this->closeGraph( this->activeGraph.get() );
		}
		this->activeGraph = newGraph;
		newGraph = nullptr;
	}

	return ( ok ? this->activeGraph.get() : NULL );
}


Data::Graph* Manager::GraphManager::createNewGraph( QString name )
{
	bool ok = true;

	AppCore::Core::getInstance()->thr->pauseAllAlg();

	// vytvor graf
	std::shared_ptr<Data::Graph> newGraph( NULL );
	if ( ok ) {
		newGraph.reset( this->createGraph( name ) );
		ok = ( newGraph.get() != NULL );
	}

	// pridaj rozlozenie
	if ( ok ) {
		Data::GraphLayout* gLay = newGraph->addLayout( "new Layout" );
		newGraph->selectLayout( gLay );
	}

	// nastav aktivny graf
	if ( ok ) {
		// ak uz nejaky graf mame, tak ho najprv sejvneme a zavrieme
		if ( this->activeGraph != NULL ) {
			//this->saveGraph(this->activeGraph);
			this->closeGraph( this->activeGraph.get() );
		}
		//this->activeGraph = newGraph.release();
		this->activeGraph = newGraph;
		newGraph = nullptr;
	}

	if ( ok ) {
		// robime zakladnu proceduru pre restartovanie layoutu
		AppCore::Core::getInstance()->restartLayout();
	}

	return ( ok ? this->activeGraph.get() : NULL );
}





Data::Graph* Manager::GraphManager::loadGraphFromDB( qlonglong graphID, qlonglong layoutID )
{
	bool error;

	Data::Graph* newGraphRaw = Model::GraphDAO::getGraph( db->tmpGetConn(), &error, graphID, layoutID );
	std::shared_ptr<Data::Graph> newGraph( newGraphRaw );

	if ( !error ) {
		qDebug() << "[Manager::GraphManager::loadGraphFromDB] Graph loaded from database successfully";

		// ak uz nejaky graf mame, tak ho zavrieme
		if ( this->activeGraph != NULL ) {
			this->closeGraph( this->activeGraph.get() );
		}

		this->activeGraph = newGraph;

		//urobime zakladnu proceduru pre restartovanie layoutu
		AppCore::Core::getInstance()->restartLayout();
	}
	else {
		qDebug() << "[Manager::GraphManager::loadGraphFromDB] Error while loading graph from database";
	}

	return this->activeGraph.get();
}

bool Manager::GraphManager::loadEvolutionGraphFromGit( QString filepath )
{
	bool lGit = false;

	QString lName = NULL;
	QString lExtension = NULL;

	QFileInfo lFileInfo( filepath );
	lName = lFileInfo.fileName();
	lExtension = lFileInfo.suffix();

	if ( lExtension.length() == 0 ) {
		lExtension = "git";
		lGit = true;
	}

	// Vytvor triedu na nacitanie dat o verziach, sparsuj informacie a vytvor verzie podla zadaneho filtera suborov
	Repository::Git::GitFileLoader lGitFileLoader = Repository::Git::GitFileLoader( filepath, Util::ApplicationConfig::get()->getValue( "Git.ExtensionFilter" ) ) ;
	QList<Repository::Git::GitVersion*> lVersions = lGitFileLoader.getDataAboutGit();

	// Vytvor evolucny graf, napln ho ziskanymi verziami a nastav graf ako aktivny
	Repository::Git::GitEvolutionGraph* evolutionGraph = new Repository::Git::GitEvolutionGraph( filepath );
	evolutionGraph->setVersions( lVersions );

	Repository::Git::GitEvolutionGraphManager::getInstance()->setEvolutionGraph( evolutionGraph );

	this->activeEvolutionGraph = Repository::Git::GitEvolutionGraphManager::getInstance()->createEvolutionGraphClone()->filterByExtension( Util::ApplicationConfig::get()->getValue( "Git.ExtensionFilter" ) )->excludeDirectories( Util::ApplicationConfig::get()->getValue( "Git.ExcludeDirectories" ) )->getFilteredEvolutionGraph();
//    this->activeEvolutionGraph = Repository::Git::GitEvolutionGraphManager::getInstance()->getEvolutionGraphByAuthor( "Jack Lawson" );

	return lGit;
}

Data::Graph* Manager::GraphManager::importEvolutionGraph( QString filepath )
{
	QString lName = NULL;
	bool ok = true;

	QFileInfo lFileInfo( filepath );
	lName = lFileInfo.fileName();

	Repository::Git::GitEvolutionGraph* evolutionGraph = this->getActiveEvolutionGraph();
	Data::Graph* lNewGraph = this->createGraph( lName );

	AppCore::Core::getInstance()->messageWindows->showProgressBar();

	Repository::Git::GitGraphUpdater updater = Repository::Git::GitGraphUpdater( 0, evolutionGraph, lNewGraph );
	updater.import();

	// Vytvorim layout a pridam ho grafu
	if ( ok ) {
		Data::GraphLayout* lGraphLayout = lNewGraph->addLayout( "new Layout " );
		lNewGraph->selectLayout( lGraphLayout );
	}

	// Ak existoval aktivny graf, tak ho zavrem a vratim nami vytvoreny graf ako aktivny
	if ( ok ) {
		if ( this->activeGraph != NULL ) {

			this->closeGraph( this->activeGraph.get() );
		}


		//this->activeGraph = updater.getActiveGraph();

		std::shared_ptr<Data::Graph> newGraph( updater.getActiveGraph() );
		this->activeGraph = newGraph;


	}

	// Restartnem layout
	if ( ok ) {
		AppCore::Core::getInstance()->restartLayout();
	}

	AppCore::Core::getInstance()->messageWindows->closeProgressBar();

	// Ak nenastala ziadna chyba, tak vratim aktivny graf, inak NULL
	return ( ok ? this->activeGraph.get() : NULL );
	//return ( ok ? this->activeGraph : NULL );

}

Data::Graph* Manager::GraphManager::createGraph( QString graphname )
{
	Data::Graph* g = this->emptyGraph( graphname );

	this->graphs.insert( g->getId(), g );
	return g;
}

void Manager::GraphManager::removeGraph( Data::Graph* graph )
{
	this->closeGraph( graph );
	// odstranime graf z DB
	Model::GraphDAO::removeGraph( graph, db->tmpGetConn() );
}

void Manager::GraphManager::closeGraph( Data::Graph* graph )
{
	// odstranime graf z working grafov
	this->graphs.remove( graph->getId() );
	// TODO zatial pracujeme len s aktivnym grafom, takze deletujeme len jeho, inak treba deletnut dany graf

	this->activeGraph = NULL;
	Clustering::Clusterer::getInstance().resetClusters();
}

Data::Graph* Manager::GraphManager::emptyGraph( QString name )
{
	Data::Graph* newGraph = new Data::Graph( 1, name, 0, 0, NULL );

	return newGraph;
}

Data::Graph* Manager::GraphManager::simpleGraph()
{
	//default vytvorenie jednoducheho grafu - pre ucely testovania
	Data::Graph* newGraph = new Data::Graph( 1, "simple", 0, 0, NULL );
	Data::Type* type = newGraph->addType( "default" );
	Data::Type* type2 = newGraph->addType( "default2" );

	Data::Node* u1 = newGraph->addNode( "u1", type );
	Data::Node* u2 = newGraph->addNode( "u2", type );
	Data::Node* u3 = newGraph->addNode( "u3", type );

	newGraph->addEdge( "e1", u1, u2, type2, false );
	newGraph->addEdge( "e2", u1, u3, type2, false );
	newGraph->addEdge( "e3", u2, u3, type2, false );

	return newGraph;
}

Manager::GraphManager* Manager::GraphManager::getInstance()
{
	//instancia GraphManager
	if ( manager == NULL ) {

		manager = new Manager::GraphManager();

	}

	return manager;
}

void Manager::GraphManager::setProgressBarValue( int value )
{
	AppCore::Core::getInstance()->messageWindows->setProgressBarValue( value );
}

void Manager::GraphManager::showProgressBar()
{
	AppCore::Core::getInstance()->messageWindows->showProgressBar();
}

void Manager::GraphManager::closeProgressBar()
{
	AppCore::Core::getInstance()->messageWindows->closeProgressBar();
}

bool Manager::GraphManager::nextVersion( Layout::LayoutThread* layout )
{
	bool ok = true;

	// Ziskam aktivny evolucny graf a graf s uzlami.
	Repository::Git::GitEvolutionGraph* lEvolutionGraph = Manager::GraphManager::getInstance()->getActiveEvolutionGraph();
	Data::Graph* lActiveGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	// Ziskam aktualnu verziu grafu
	int currentVersion =  lActiveGraph->getCurrentVersion();

	// Vytvorim updater
	Repository::Git::GitGraphUpdater* lUpdater = new Repository::Git::GitGraphUpdater( currentVersion, lEvolutionGraph, lActiveGraph );

	// Zastavim layout, updatnem graf, zmenim verziu a spustim layout
	layout->pause();
	lUpdater->nextVersion();
	lActiveGraph->setCurrentVersion( currentVersion + 1 );
	layout->play();

	delete lUpdater;
	return ok;
}

bool Manager::GraphManager::previousVersion( Layout::LayoutThread* layout )
{
	bool ok = true;

	// Ziskam aktivny evolucny graf a graf s uzlami.
	Repository::Git::GitEvolutionGraph* lEvolutionGraph = Manager::GraphManager::getInstance()->getActiveEvolutionGraph();
	Data::Graph* lActiveGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	// Ziskam aktualnu verziu grafu
	int currentVersion =  lActiveGraph->getCurrentVersion();

	// Vytvorim updater
	Repository::Git::GitGraphUpdater* lUpdater = new Repository::Git::GitGraphUpdater( currentVersion, lEvolutionGraph, lActiveGraph );

	// Zastavim layout, updatnem graf, zmenim verziu a spustim layout
	layout->pause();
	lUpdater->previousVersion();
	lActiveGraph->setCurrentVersion( currentVersion - 1 );
	layout->play();

	delete lUpdater;
	return ok;
}

bool Manager::GraphManager::changeToVersion( Layout::LayoutThread* layout, int toVersion )
{
	bool ok = true;

	// Ziskam aktivny evolucny graf a graf s uzlami.
	Repository::Git::GitEvolutionGraph* lEvolutionGraph = Manager::GraphManager::getInstance()->getActiveEvolutionGraph();
	Data::Graph* lActiveGraph = Manager::GraphManager::getInstance()->getActiveGraph();

	// Ziskam aktualnu verziu grafu
	int currentVersion =  lActiveGraph->getCurrentVersion();

	// Vytvorim updater
	Repository::Git::GitGraphUpdater* lUpdater = new Repository::Git::GitGraphUpdater( currentVersion, lEvolutionGraph, lActiveGraph );

	// Zastavim layout, updatnem graf, zmenim verziu a spustim layout
	layout->pause();
	lUpdater->changeToVersion( toVersion );
	lActiveGraph->setCurrentVersion( toVersion );
	layout->play();

	delete lUpdater;
	return ok;
}

void Manager::GraphManager::getDiffInfo( QString path, int version )
{
	// Ziskam aktivny evolucny graf
	Repository::Git::GitEvolutionGraph* lEvolutionGraph = Manager::GraphManager::getInstance()->getActiveEvolutionGraph();

	// Ziskam info konkretnej verzie
	Repository::Git::GitVersion* lVersion = lEvolutionGraph->getVersion( version );
	Repository::Git::GitFile* gitFile = nullptr;

	// Najdem subor, ktory bol zvoleny v grafe vo verzii, kde bol posledne modifikovany
	bool isFound = false;
	for ( int i = version; i >= 0; i-- ) {
		foreach ( Repository::Git::GitFile* file, *lEvolutionGraph->getVersion( i )->getChangedFiles() ) {
			if ( file->getFilepath() == path ) {
				gitFile = file;
				isFound = true;
				break;
			}
		}
		if ( isFound ) {
			break;
		}
	}

	// Vytvorim triedu na nacitanie diff pre subor
	Repository::Git::GitFileLoader loader = Repository::Git::GitFileLoader( lEvolutionGraph->getFilePath(), Util::ApplicationConfig::get()->getValue( "Git.ExtensionFilter" ) );

	// Ak sme nasli subor, tak nacitaj do suboru diff a vypis obsah do konzoly
	if ( gitFile ) {
		loader.getDiffInfo( gitFile, lVersion->getCommitId(), version - 1 < 0 ? NULL : lEvolutionGraph->getVersion( version - 1 )->getCommitId() );
		gitFile->printContent();
	}

}

void Manager::GraphManager::runTestCase( qint32 action )
{
	//testovaci pripad, vytvorenie grafu, inicializacia Debug, 7 testovacich vstupov grafu
	switch ( action ) {
		case 1:
		case 2: {
			//inicializacia
			qDebug() << "TestCase initialization";
			bool error;
			Data::Graph* g = Model::GraphDAO::addGraph( "testCase1",this->db->tmpGetConn() ); //vytvorenie grafu
			g->addLayout( "testCase1 layout 1" ); //pridanie layoutu
			Data::GraphLayout* gl2 = g->addLayout( "testCase1 layout 2" ); //pridanie layoutu
			g->addLayout( "testCase1 layout 3" ); //pridanie layoutu
			qDebug() << "layouty grafu: " << g->toString();
			QMap<qlonglong,Data::GraphLayout*> layouts = g->getLayouts( &error );
			foreach ( qlonglong i, layouts.keys() ) { //vypis layoutov
				qDebug() << layouts.value( i )->toString();
			}

			g->selectLayout( gl2 ); //vybratie layoutu
			Data::Type* t1 = g->addType( "type1" ); //pridanie typu
			Data::Type* t2 = g->addType( "type2" ); //pridanie typu
			Data::Type* t3 = g->addType( "type3" ); //pridanie typu
			Data::Type* t4 = g->addType( "type4" ); //pridanie typu
			Data::Type* t5 = g->addType( "type5" ); //pridanie typu
			Data::Type* t6 = g->addType( "type6" ); //pridanie typu
			for ( qlonglong i=0; i<100; i++ ) {
				if ( i%3==1 ) {
					g->addNode( "node",t2 );
				}
				else if ( i%3==2 ) {
					g->addNode( "node",t3 );
				}
				else {
					g->addNode( "node",t1 );
				}
			}

			QMap<qlonglong, osg::ref_ptr<Data::Node> >* gNodes = g->getNodes();
			osg::ref_ptr<Data::Node> n1;
			osg::ref_ptr<Data::Node> n2;
			qlonglong iteration = 0;
			foreach ( qlonglong i, gNodes->keys() ) {
				if ( iteration==0 ) {
					n1 = gNodes->value( i );
				}
				else {
					n2 = gNodes->value( i );
					if ( iteration%3==1 ) {
						g->addEdge( "edge",n1,n2,t5,true );
					}
					else if ( iteration%3==2 ) {
						g->addEdge( "edge",n1,n2,t6,true );
					}
					else {
						g->addEdge( "edge",n1,n2,t4,true );
					}
					n1 = gNodes->value( i );
				}
				iteration++;
			}
			n1 = NULL;
			n2 = NULL;

			qDebug() << "Nodes count: " << g->getNodes()->size();
			qDebug() << "Types count: " << g->getTypes()->size();
			qDebug() << "Edges count: " << g->getEdges()->size();


			if ( action==1 ) {
				//testovanie remove metod
				qDebug() << "Starting testCase 1";

				qDebug() << "Removing type t1";
				g->removeType( t1 );
				qDebug() << "type t1 removed";

				qDebug() << "Counts after the type t1 was removed";
				qDebug() << "Nodes count: " << g->getNodes()->size();
				qDebug() << "Types count: " << g->getTypes()->size();
				qDebug() << "Edges count: " << g->getEdges()->size();

				qDebug() << "Ending testCase 1";
			}
			if ( action==2 ) {
				qDebug() << "Starting testCase 2";

				osg::ref_ptr<Data::Node> selectedNode = g->getNodes()->values().at( 10 );

				qDebug() << "Removing node: " << selectedNode->toString();
				g->removeNode( selectedNode );

				qDebug() << "Counts after the type t1 was removed";
				qDebug() << "Nodes count: " << g->getNodes()->size();
				qDebug() << "Types count: " << g->getTypes()->size();
				qDebug() << "Edges count: " << g->getEdges()->size();


				qDebug() << "Ending testCase 2";
				break;


			}


			//cleanup
			if ( Model::GraphDAO::removeGraph( g,this->db->tmpGetConn() ) ) {
				qDebug() << "graph successfully removed from db";
			}
			else {
				qDebug() << "could not be removed from db";
			}

			delete g;
			g = NULL;
			break;
		}
		case 3: {
			Data::Graph* g = Model::GraphDAO::addGraph( "testCase1",this->db->tmpGetConn() ); //vytvorenie grafu
			Data::Type* t1 = g->addType( "type" );
			Data::Type* t2 = g->addType( "type2" );
			osg::ref_ptr<Data::Node> n1 = g->addNode( "node1",t1 );
			osg::ref_ptr<Data::Node> n2 = g->addNode( "node2",t1 );
			osg::ref_ptr<Data::Edge> e1 = g->addEdge( "edge1",n1,n2,t2,true );
			g->addEdge( "edge2",n1,n2,t2,true );
			g->addEdge( "edge3",n1,n2,t2,true );
			g->addEdge( "edge4",n1,n2,t2,true );
			g->removeNode( n1 );
			n1 = NULL;
			qDebug() << "node should be deleted";
			e1 = NULL;
			qDebug() << "edge should be deleted";

			delete g;
			g = NULL;
			qDebug() << "graph deleted";
			n2 = NULL;
			break;
		}
		case 4: {
			Data::Graph* g = Model::GraphDAO::addGraph( "testCase1",this->db->tmpGetConn() ); //vytvorenie grafu
			Data::Type* t1 = g->addType( "type" );
			Data::Type* t2 = g->addType( "type2" );
			osg::ref_ptr<Data::Node> n1 = g->addNode( "node1",t1 );
			osg::ref_ptr<Data::Node> n2 = g->addNode( "node2",t1 );
			osg::ref_ptr<Data::Edge> e1 = g->addEdge( "edge1",n1,n2,t2,true );
			g->removeEdge( e1 );
			e1 = NULL;
			qDebug() << "edge should be deleted";
			n2 = NULL;
			n1 = NULL;

			delete g;
			g = NULL;
			qDebug() << "graph deleted";
			break;
		}

		case 5: {
			Data::Graph* g = Model::GraphDAO::addGraph( "testCase1",this->db->tmpGetConn() ); //vytvorenie grafu
			g->selectLayout( g->addLayout( "layout" ) );
			Data::Type* t1 = g->addType( "type" );
			Data::MetaType* t2 = g->addMetaType( "type2" );
			Data::Type* t3 = g->addType( "type3" );
			osg::ref_ptr<Data::Node> n1 = g->addNode( "node1",t1 );
			osg::ref_ptr<Data::Node> n2 = g->addNode( "node2",t1 );
			osg::ref_ptr<Data::Edge> e1 = g->addEdge( "edge1",n1,n2,t2,true );
			g->addEdge( "edge2",n1,n2,t2,true );

			g->addEdge( "edge3",n1,n2,t3,true );
			g->addEdge( "edge4",n1,n2,t3,true );
			g->removeNode( n1 );
			n1 = NULL;
			qDebug() << "node should be deleted";
			e1 = NULL;
			qDebug() << "edge should be deleted";

			n2 = NULL;

			delete g;
			g = NULL;
			qDebug() << "graph deleted";
			break;
		}


		case 6: {
			Data::Graph* g = Model::GraphDAO::addGraph( "testCase1",this->db->tmpGetConn() ); //vytvorenie grafu
			g->selectLayout( g->addLayout( "layout" ) );
			Data::Type* t1 = g->addType( "type" );
			Data::MetaType* t2 = g->addMetaType( "type2" );

			osg::ref_ptr<Data::Edge> e1 = g->addEdge( "edge1",g->addNode( "node1",t1 ),g->addNode( "node2",t1 ),t2,true );
			g->removeEdge( e1 );
			e1 = NULL;
			qDebug() << "edge should be deleted";

			delete g;
			g = NULL;
			qDebug() << "graph deleted";
			break;
		}


		case 7: {
			Data::Graph* g = Model::GraphDAO::addGraph( "testCase1",this->db->tmpGetConn() ); //vytvorenie grafu
			g->selectLayout( g->addLayout( "layout" ) );
			Data::Type* t1 = g->addType( "type" );
			g->addEdge( "edge1",g->addNode( "node1",t1 ),g->addNode( "node2",t1 ),g->addMetaType( "type2" ),true );
			g->removeType( t1 );

			delete t1;
			t1 = NULL;

			qDebug() << "type should be deleted";

			delete g;
			g = NULL;
			qDebug() << "graph deleted";
			break;
		}

		default:
			qDebug() << "Manager:runTestCase not supported action";
			break;


	}
}
