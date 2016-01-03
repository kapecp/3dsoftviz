#include "QOSG/OptionsWindow.h"

#include "QOSG/ViewerQT.h"
#include "QOSG/TreeItem.h"
#include "QOSG/TreeModel.h"
#include "Layout/LayoutThread.h"
#include "Core/Core.h"
#include "Viewer/CoreGraph.h"
#include "Manager/Manager.h"
#include "Data/Graph.h"
#include "Util/ApplicationConfig.h"

#include <iostream>

namespace QOSG {

OptionsWindow::OptionsWindow( Vwr::CoreGraph* cg, QOSG::ViewerQT* viewer )
{
	//vytvorenie a inicializovanie celeho okna
	this->viewer = viewer;
	this->cg = cg;

	setModal( true );
	resize( 600,250 );

	changes = new QList<QString>();
	selectedItem = NULL;

	Util::ApplicationConfig* appConf = Util::ApplicationConfig::get();
	treeModel = new TreeModel( appConf->getList() );

	QTreeView* treeView = new QTreeView();
	treeView->setFocusPolicy( Qt::NoFocus );
	treeView->setModel( treeModel );
	treeView->setHeaderHidden( true );

	connect( treeView,SIGNAL( clicked( QModelIndex ) ),this,SLOT( itemClicked( QModelIndex ) ) );

	view = new QTreeView();
	view->setFocusPolicy( Qt::NoFocus );
	view->setVisible( false );

	QPushButton* closeButton = new QPushButton( tr( "Close" ) );
	closeButton->setFocusPolicy( Qt::NoFocus );
	connect( closeButton, SIGNAL( clicked() ), this, SLOT( close() ) );

	applyButton= new QPushButton( tr( "Apply" ) );
	applyButton->setFocusPolicy( Qt::NoFocus );
	applyButton->setEnabled( false );
	connect( applyButton, SIGNAL( clicked() ), this, SLOT( commitChanges() ) );

	QHBoxLayout* horizontalLayout = new QHBoxLayout;
	horizontalLayout->addWidget( treeView );
	horizontalLayout->addWidget( view );

	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch( 1 );
	buttonsLayout->addWidget( applyButton );
	buttonsLayout->addWidget( closeButton );

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addLayout( horizontalLayout );
	mainLayout->addStretch( 1 );
	mainLayout->addSpacing( 12 );
	mainLayout->addLayout( buttonsLayout );

	setLayout( mainLayout );

	setWindowTitle( tr( "Config Dialog" ) );
}

void OptionsWindow::itemClicked( QModelIndex index )
{

	TreeItem* item = static_cast<TreeItem*>( index.internalPointer() );
	//zisti sa ci existuju data v kliknutej polozke
	if ( item->data( 1 ).isValid() ) {
		QString data = item->data( 1 ).toString();

		// pri prekliknuti je treba ulozit zmeny, ktore boli uskutocnene v predchadzajucej polozke
		if ( selectedItem ) {
			if ( selectedItem != item ) {
				selectedItem->replaceDataColumn( getModelData() );
				selectedItem = item;
			}
		}
		else {
			selectedItem = item;
		}

		//vytvorenie modelu pre tabulku
		createTableModel( data );
		//po prvom kliknuti sa spristupni aplikovanie zmien
		applyButton->setEnabled( true );
	}
}

void OptionsWindow::createTableModel( QString data )
{
	//vytvorenie polozky tabulky
	model = new QStandardItemModel( 0, 2 );
	model->setHeaderData( 0, Qt::Horizontal, QObject::tr( "Attribute" ) );
	model->setHeaderData( 1, Qt::Horizontal, QObject::tr( "Value" ) );

	// rozdelenie dat
	QList<QString> rows = data.split( ";" );

	//postupne sa vytvaraju riadky tabulky
	for ( int i=0; i< rows.length(); i++ ) {
		QList<QString> row = rows[i].split( "," );

		model->insertRow( i );
		model->setData( model->index( i, 0 ), row[0] );
		model->setData( model->index( i, 1 ), row[1] );
		model->item( i,0 )->setFlags( Qt::NoItemFlags );
	}

	view->setVisible( true );
	view->setModel( model );
}

QString OptionsWindow::getModelData()
{
	QString data;
	for ( int i=0; i< model->rowCount(); i++ ) {
		data.append( model->item( i,0 )->text() );
		data.append( "," );
		data.append( model->item( i,1 )->text() );
		if ( i < model->rowCount()-1 ) {
			data.append( ";" );
		}
	}
	return data;
}

void OptionsWindow::commitChanges()
{

	Util::ApplicationConfig* appConf = Util::ApplicationConfig::get();
	QString str;
	// ulozenie dat aktivnej polozky do struktury
	selectedItem->replaceDataColumn( getModelData() );
	//spustenie prehladavania stromu
	treeSearch( treeModel->getRootItem(),str );

	//ak je otvoreny graf tak sa ulozia aj zmeny urobene v typoch grafu
	if ( Manager::GraphManager::getInstance()->getActiveGraph() != NULL ) {
		saveNodeTypes( treeModel->getRootItem() );
	}

	//ulozenie celych nastaveni
	appConf->saveConfig();

	//reloadovanie nastaveni v ostatnych castiach aplikacie
	Layout::LayoutThread* thr = AppCore::Core::getInstance()->getLayoutThread();

	bool running = thr->isRunning();

	if ( running ) {
		thr->pauseAllAlg();
	}

	viewer->reloadConfig();
	cg->reloadConfig();

	if ( running ) {
		thr->play();
	}

}

void OptionsWindow::treeSearch( TreeItem* index, QString path )
{
	//ukoncenie prehladavania v pripade ze sa narazi na typy uzlov
	if ( index->data( 0 ).toString() == "Node Types" ) {
		return;
	}

	//ak nie je polozka koren tak sa prida jeho meno do celkovej cesty
	if ( index->data( 0 ).toString().toStdString() != "root" ) {
		path.append( QString( "%1." ).arg( index->data( 0 ).toString() ) );
	}

	//ak sa narazi na polozku s datami tieto data sa ulozia
	if ( index->data( 1 ).isValid() ) {
		applyChanges( path, index->data( 1 ).toString() );
	}

	//prehladanie potomkov polozky
	for ( int i=0; i < index->childCount(); i++ ) {
		treeSearch( index->child( i ),path );
	}
}

void OptionsWindow::saveNodeTypes( TreeItem* index )
{
	//nacitanie typov grafu
	Manager::GraphManager* manager = Manager::GraphManager::getInstance();
	QMap<qlonglong, Data::Type*>* types = manager->getActiveGraph()->getTypes();

	for ( int i=0; i < index->childCount(); i++ ) {
		TreeItem* root = index->child( i );
		//ak je root typov grafu
		if ( root->data( 0 ) == "Node Types" ) {
			//pre kazdy uloz data
			for ( int j=0; j< root->childCount(); j++ ) {
				TreeItem* item = root->child( j );
				Data::Type* type = types->value( item->data( 2 ).toLongLong() );
				QMap<QString, QString>* settings = type->getSettings();
				QList<QString> atributes = item->data( 1 ).toString().split( ";" );

				for ( int k=0; k<atributes.length(); k++ ) {
					QList<QString> atribute = atributes[k].split( "," );
					settings->insert( atribute[0],atribute[1] );
				}

			}
		}
	}


}

void OptionsWindow::applyChanges( QString path, QString data )
{
	//ziskanie odkazu na konfigurak
	Util::ApplicationConfig* appConf = Util::ApplicationConfig::get();

	//rozdelenie dat
	QList<QString> list = data.split( ";" );
	//postupne ukladanie dat
	for ( int i=0; i< list.length(); i++ ) {
		QList<QString> att = list[i].split( "," );

		appConf->add( QString( "%1%2" ).arg( path ).arg( att[0] ),att[1] );
		std::cout <<path.toStdString()<< att[0].toStdString() << "\t" << att[1].toStdString() << std::endl;
	}

}

} // namespace QOSG
