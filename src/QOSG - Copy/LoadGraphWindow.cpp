#include "QOSG/LoadGraphWindow.h"

using namespace QOSG;

LoadGraphWindow::LoadGraphWindow(QWidget *parent)
{
	setModal(true);
	resize(600,250);	 
    setWindowTitle(tr("Load graph from database"));

	loadButton = createButton(tr("&Load"), SLOT(loadGraph()));

	QPushButton *cancelButton = new QPushButton(tr("Cancel"));
	cancelButton->setFocusPolicy(Qt::NoFocus);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	numberOfGraphs = new QLabel;

	graphList<<tr("ID")<<tr("Name")<<tr("Number of nodes")<<tr("Number of edges");
	graphsTable=new QTableWidget(this);
	graphsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	graphsTable->setRowCount(0);
	graphsTable->setColumnCount(4);
	graphsTable->setHorizontalHeaderLabels(graphList);
	graphsTable->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
	graphsTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
	graphsTable->horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);
	graphsTable->horizontalHeader()->setResizeMode(3, QHeaderView::ResizeToContents);
	graphsTable->verticalHeader()->hide();
    graphsTable->setShowGrid(true);
	//graphsTable->setSortingEnabled(false);

	createGraphTable();

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(loadButton);
	buttonsLayout->addWidget(cancelButton);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(graphsTable, 3, 0, 1, 3);
	mainLayout->addWidget(numberOfGraphs, 4, 0, 1, 3);
    mainLayout->addLayout(buttonsLayout, 5, 0, 1, 3);
    setLayout(mainLayout);
}

void LoadGraphWindow::createGraphTable()
{
	Manager::GraphManager * manager = Manager::GraphManager::getInstance();
	Model::DB * db = manager->getDB();
	bool error = false;
	//Data::Graph * graph;
	qlonglong id;
	QString name;
	
	///////////
	//QSqlQuery query;
    //query.exec("insert into graphs values(102, 'Graf1')");
	/////////////

	QMap<qlonglong, Data::Graph *> graphs = Model::GraphDAO::getGraphs(db->tmpGetConn(), &error);
	int graphsCount = graphs.count(); 

	numberOfGraphs->setText(tr("%1 graph(s) found").arg(graphsCount));
	graphsTable->setRowCount(graphsCount);

	QMap<qlonglong, Data::Graph*>::iterator i;
	int row;
	
	for(i = graphs.begin(), row=0; i != graphs.end(); ++i, row++)
	{
		id = i.key();
		//graph = i.value();
		
		name = graphs.value(id)->getName();
		//cout << id << ": " << name.toStdString() << endl;

		QTableWidgetItem *itemID = new QTableWidgetItem(tr("%1").arg(id));
		QTableWidgetItem *itemName = new QTableWidgetItem(name);
		graphsTable->setItem(row, 0, itemID);
		graphsTable->setItem(row, 1, itemName);
	}
}

void LoadGraphWindow::loadGraph()
{
	int row;
	row = graphsTable->currentRow();
	QTableWidgetItem *item;
	item = graphsTable->currentItem();
	
}

QPushButton *LoadGraphWindow::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}
