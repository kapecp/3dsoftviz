#include "Repository/Git/GitGraphImporter.h"

#include "Importer/GraphOperations.h"
#include "Importer/ReadNodesStore.h"
#include "Importer/ReadEdgesStore.h"

#include "Manager/Manager.h"

#include "Data/Graph.h"

#include "GitLib/GitVersion.h"
#include "GitLib/GitEvolutionGraph.h"
#include "GitLib/GitFile.h"

#include <QDebug>
#include <QDir>

Importer::GitGraphImporter::~GitGraphImporter()
{
	delete this->edgeType;
	delete this->nodeType;
}

bool Importer::GitGraphImporter::import( ImporterContext& context )
{
	// context
	this->context = &context;

	// helpers
	this->filepath = this->context->getFilePath();
	this->graphOp.reset( new GraphOperations( this->context->getGraph() ) );
	this->readNodes.reset( new Importer::ReadNodesStore() );
	this->readEdges.reset( new Importer::ReadEdgesStore() );

	// default types
	this->edgeType = NULL;
	this->nodeType = NULL;
	this->graphOp->addDefaultTypes( edgeType, nodeType );

	bool ok = true;

	// inicializacia a pridanie typov hran a uzlov do aktivneho grafu
	QMap<QString, QString>* settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
	this->context->getGraph().addType( "file", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.RemoveNode" ) );
	this->context->getGraph().addType( "removedFile", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "0" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
	this->context->getGraph().addType( "dir", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "0" );
	settings->insert( "color.B", "0" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
	this->context->getGraph().addType( "root", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.AuthorNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Author" ) );
	this->context->getGraph().addType( "author", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "0" );
	settings->insert( "color.G", "0" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.OrientedEdge" ) );
	this->context->getGraph().addType( "authorEdge", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "1" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Edge" ) );
	this->context->getGraph().addType( "Edge", settings );

	settings = new QMap<QString, QString>;
	settings->insert( "color.R", "0" );
	settings->insert( "color.G", "1" );
	settings->insert( "color.B", "1" );
	settings->insert( "color.A", "1" );
	settings->insert( "scale", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.DefaultNodeScale" ) );
	settings->insert( "textureFile", Util::ApplicationConfig::get()->getValue( "Viewer.Textures.Node" ) );
	this->context->getGraph().addType( "newE", settings );

	// Ziskame pridane subory a meno autora prve verzie
	QList<Repository::Git::GitFile*> lAddedGitFiles = Manager::GraphManager::getInstance()->getActiveEvolutionGraph()->getVersion( 0 )->getGitFilesByType( Repository::Git::GitType::ADDED );
	QString lAuthor = Manager::GraphManager::getInstance()->getActiveEvolutionGraph()->getVersion( 0 )->getAuthor();

	// Vytvorime graf za pomoci pridanych suborov prvej verzie
	ok = makeGraph( lAddedGitFiles );

	// Ziskame typ s nazvom 'author' na zobrazenie autora v grafe
	Data::Type* lType;
	QList<Data::Type*> lTypes = this->context->getGraph().getTypesByName( "author" );
	lType = lTypes.at( 0 );

	// Vytvorime a nastavime node pre autora v grafe. Pridame ho taktiet do mnozinu uzlov v grafe
	Data::Node* lAuthorNode = this->context->getGraph().addNode( lAuthor, lType );
	lAuthorNode->setLabelText( lAuthor );
	lAuthorNode->showLabel( true );
	readNodes->addNode( lAuthor, lAuthorNode );

	// Ziskame typ s nazov 'authorEdge' na zobrazenie hrany od autora k zmenenemu uzlu
	lTypes = this->context->getGraph().getTypesByName( "authorEdge" );
	lType = lTypes.at( 0 );

	// Pre kazdy pridany subor vytvorime orientovanu hranu od autora k uzlu suboru
	foreach ( Repository::Git::GitFile* file, lAddedGitFiles ) {

		// Ziskame uzol pre odpovedajucu cestu suboru z mnoziny vlozenych uzlov
		osg::ref_ptr<Data::Node> osgNode = readNodes->get( file->getFilepath() );

		// Ak takyto uzol existuje v grafe, tak vytvorime hranu od autora k suboru. Hranu taktiez pridame do mnoziny hran v grafe.
		if ( osgNode ) {
			osg::ref_ptr<Data::Edge> newEdge = this->context->getGraph().addEdge( lAuthorNode->Data::AbsNode::getName() + osgNode.get()->Data::AbsNode::getName(), readNodes->get( lAuthor ), osgNode, lType, true );
			readEdges->addEdge( lAuthorNode->Data::AbsNode::getName() + osgNode->Data::AbsNode::getName(), newEdge );
		}
	}

	return ok;
}

bool Importer::GitGraphImporter::makeGraph( QList<Repository::Git::GitFile*> gitFiles )
{
	bool ok = true;

	// Pre kazdu cestu suboru pridam uzly a hrany na zaklade separatora '/'
	for ( int i = 0; i < gitFiles.size(); i++ ) {
		QString line = gitFiles.at( i )->getFilepath();

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
		ok = addNode( list );
		ok = addEdge( list );

		// Aktualizujem percento spracovania
		Manager::GraphManager::getInstance()->setProgressBarValue( int( ( double( i + 1 ) /  double( gitFiles.size() ) ) * 100 ) );
	}

	return ok;
}

bool Importer::GitGraphImporter::addNode( QStringList& list )
{
	bool ok = true;

	// Pre kazdu ciastocnu cestu zachytenu v liste stringov skontrolujem, ci sa uz nenachadza v grafe. Ak sa nenachadza
	// tak potom zistim typ uzla(koren adresar, subor) a nasledne danu cestu pridam do grafu ako uzol.
	for ( int i = 0; i < list.size(); i++ ) {
		bool exist = false;
		QString lNodeName = list.at( i );

		// Skontrolujem ci dany QString nie je prazdny
		ok = !lNodeName.isEmpty();
		this->context->getInfoHandler().reportError( ok, "Node ID can not be empty" );

		osg::ref_ptr<Data::Node> lNode( NULL );
		// Ak sa cesta v grafe uz nachadza, tak nastavim exist na true
		if ( this->readNodes->contains( lNodeName ) ) {
			exist = true;
		}

		// Ak sa cesta v grafe nenachadzala, tak zistim typ ciastocnej cesty a pridam ho do grafu s jeho labelom.
		if ( !exist ) {
			Data::Type* lType = NULL;

			QString lVal;
			// Ak je ciastocna cesta z listu na pozicie n - 1, tak ide o subor, ak na pozici 0 tak ide o root ostatne su adresare
			if ( i == ( list.size() - 1 ) ) {
				lVal = "file";
			}
			else {
				lVal = "dir";
				if ( i == 0 ) {
					lVal = "root";
				}
			}

			// Ziskam typ ulozeny v grafe so ziskanym nazvom typu
			QList<Data::Type*> lTypes = this->context->getGraph().getTypesByName( lVal );
			lType = lTypes.at( 0 );

			// Vytvorim uzol pre danu cestu s danym typom
			lNode = this->context->getGraph().addNode( lNodeName, lType );

			// Ak ide o korenovy uzol, tak ho fixnem na jedno miesto
			if ( QString::compare( lVal, "root" ) == 0 ) {
				lNode->setFixed( true );
			}

			// Nastavim label a pridam do mnoziny uzlov v grafe
			lNode->setLabelText( lNodeName );
			lNode->showLabel( true );
			this->readNodes->addNode( lNodeName, lNode );
		}
	}
	return ok;
}

bool Importer::GitGraphImporter::addEdge( QStringList& list )
{
	bool ok = true;

	Data::Type* lType = this->context->getGraph().getTypesByName( "Edge" ).at( 0 );


	// Pre ciastocne cesty vytvorim hranu medzi dvojicami i a i+1, vtedy ak dana hrana v grafe neexistuje
	// a uzly, ktore hranu definuju, existuju v grafe.
	for ( int i = 0; i < list.size() - 1; i++ ) {
		QString lNodeNameFrom = list.at( i );
		QString lNodeNameTo = list.at( i + 1 );

		// Vytvorim nazov hrany kombinaciou uzlov, ktore definuju hranu
		QString lEdgeName = lNodeNameFrom + lNodeNameTo;
		osg::ref_ptr<Data::Edge> lEdge( NULL );
		bool exist = false;

		// Pre kazdu hranu sa inkrementuje pocitadlo, kolkokrat je dana hrana naviazana v grafe.
		this->context->getGraph().addEdgeOccurence( lEdgeName );

		// Ak sa dana hrana v grafe nachadza, tak potom nastavim exist na true, co zamedzi dalsie spracovanie.
		if ( this->readEdges->contains( lEdgeName ) ) {
			exist = true;
		}

		// Ak sa hrana v grafe nenachadza, tak zistim, ci jej uzly sa nachadzaju v grafe. Ak nie, tak sa hrana neprida.
		// V opacnom pripade sa prida.
		if ( !exist ) {
			if ( ok ) {
				// Ak sa uzol nenachadza v grafe, tak vypis chybu a skonci dalsie spracovanie
				ok = this->readNodes->contains( lNodeNameFrom );
				this->context->getInfoHandler().reportError( ok, "Edge references invalid source node." );
			}

			if ( ok ) {
				// Ak sa uzol nenachadza v grafe, tak vypis chybu a skonci dalsie spracovanie
				ok = this->readNodes->contains( lNodeNameTo );
				this->context->getInfoHandler().reportError( ok, "Edge references invalid source node." );
			}

			bool oriented = false;

			// Ak sa obidva uzly nachadzaju v grafe, tak vytvor medzi nimi hranu.
			if ( ok ) {
				lEdge = this->context->getGraph().addEdge( lEdgeName, this->readNodes->get( lNodeNameFrom ), this->readNodes->get( lNodeNameTo ), lType, oriented );
				this->readEdges->addEdge( lEdgeName, lEdge );
			}
		}
	}
	return ok;
}

