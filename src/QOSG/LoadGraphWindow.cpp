#include "QOSG/LoadGraphWindow.h"

using namespace QOSG;

LoadGraphWindow::LoadGraphWindow(QWidget *parent)
{
	setModal(true);
	resize(600,250);
	setWindowTitle(tr("Load graph from database"));

	loadButton = createButton(tr("Load"), SLOT(loadGraph()));
	removeButton = createButton(tr("Remove"), SLOT(removeGraph()));
	renameButton = createButton(tr("Rename"), SLOT(renameGraph()));

	QPushButton *cancelButton = new QPushButton(tr("Cancel"));
	cancelButton->setFocusPolicy(Qt::NoFocus);
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	numberOfGraphs = new QLabel;

	graphList << tr("ID") << tr("Name") << tr("No. of layouts") << tr("No. of nodes") << tr("No. of edges");
	graphsTable = new QTableWidget(this);
	graphsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	graphsTable->setRowCount(0);
	graphsTable->setColumnCount(5);
	graphsTable->setHorizontalHeaderLabels(graphList);
	graphsTable->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
	graphsTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
	graphsTable->horizontalHeader()->setResizeMode(2, QHeaderView::ResizeToContents);
	graphsTable->horizontalHeader()->setResizeMode(3, QHeaderView::ResizeToContents);
	graphsTable->horizontalHeader()->setResizeMode(4, QHeaderView::ResizeToContents);
	graphsTable->verticalHeader()->hide();
	graphsTable->setShowGrid(true);

	createGraphTable();

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(loadButton);
	buttonsLayout->addWidget(renameButton);
	buttonsLayout->addWidget(removeButton);
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
	qlonglong id, graphsCount, row;
	QList<qlonglong> nodes;
	QList<qlonglong> edges;
	QList<qlonglong> layouts;
	QString name;
	QMap<qlonglong, Data::Graph*>::iterator iterGraph;

	QMap<qlonglong, Data::Graph *> graphs = Model::GraphDAO::getGraphs(db->tmpGetConn(), &error);
	graphsCount = graphs.count();

	numberOfGraphs->setText(tr("%1 graph(s) found").arg(graphsCount));
	graphsTable->setRowCount(graphsCount);
	nodes = Model::NodeDAO::getListOfNodes(db->tmpGetConn(), &error);
	edges = Model::EdgeDAO::getListOfEdges(db->tmpGetConn(), &error);
	layouts = Model::GraphLayoutDAO::getListOfLayouts(db->tmpGetConn(), &error);

	qDebug() << "[QOSG::LoadGraphWindow::createGraphTable] total number of nodes in DB: " << nodes.count();
	qDebug() << "[QOSG::LoadGraphWindow::createGraphTable] total number of edges in DB: " << edges.count();
	qDebug() << "[QOSG::LoadGraphWindow::createGraphTable] total number of layouts in DB: " << layouts.count();

	for(iterGraph = graphs.begin(), row=0; iterGraph != graphs.end(); ++iterGraph, row++)
	{
		id = iterGraph.key();

		name = graphs.value(id)->getName();

		QTableWidgetItem *itemID = new QTableWidgetItem(tr("%1").arg(id));
		QTableWidgetItem *itemName = new QTableWidgetItem(name);
		QTableWidgetItem *itemNumberOfLayouts = new QTableWidgetItem(tr("%1").arg(layouts.count(id)));
		QTableWidgetItem *itemNumberOfNodes = new QTableWidgetItem(tr("%1").arg(nodes.count(id)));
		QTableWidgetItem *itemNumberOfEdges = new QTableWidgetItem(tr("%1").arg(edges.count(id)));
		graphsTable->setItem(row, 0, itemID);
		graphsTable->setItem(row, 1, itemName);
		graphsTable->setItem(row, 2, itemNumberOfLayouts);
		graphsTable->setItem(row, 3, itemNumberOfNodes);
		graphsTable->setItem(row, 4, itemNumberOfEdges);
	}
	if(graphsTable->rowCount() > 0)
		graphsTable->selectRow(0);
}

void LoadGraphWindow::loadGraph()
{
	qlonglong graphID;

	if(graphsTable->rowCount() > 0)
	{
		graphID = graphsTable->item(graphsTable->currentRow(), 0)->text().toLongLong();

		qDebug() << "[QOSG::LoadGraphWindow::loadGraph] Selected graph ID: " << graphID;

		SelectLayoutWindow *selectLayout = new SelectLayoutWindow(this, graphID);
		selectLayout->show();

		this->close();
	}
	else
	{
		qDebug() << "[QOSG::LoadGraphWindow::loadGraph] There are no graphs saved in DB.";
	}
}

void LoadGraphWindow::renameGraph()
{
	qlonglong graphID;
	Manager::GraphManager * manager = Manager::GraphManager::getInstance();
	Model::DB * db = manager->getDB();
	bool ok;

	if(graphsTable->rowCount() > 0)
	{
		graphID = graphsTable->item(graphsTable->currentRow(), 0)->text().toLongLong();

		qDebug() << "[QOSG::LoadGraphWindow::renameGraph] Selected graph ID: " << graphID;

		QString newGraphName = QInputDialog::getText(this, tr("New graph name"), tr("Enter new graph name:"), QLineEdit::Normal, "", &ok);
		if (ok && !newGraphName.isEmpty())
		{
			Model::GraphDAO::setGraphName(graphID, newGraphName, db->tmpGetConn());

			createGraphTable();
			this->repaint();
			this->update();
		}
		else
		{
			qDebug() << "[QOSG::LoadGraphWindow::renameGraph] Input dialog canceled";
		}
	}
	else
	{
		qDebug() << "[QOSG::LoadGraphWindow::removeGraph] There are no graphs saved in DB.";
	}
}

void LoadGraphWindow::removeGraph()
{
	qlonglong graphID;
	Manager::GraphManager * manager = Manager::GraphManager::getInstance();
	Model::DB * db = manager->getDB();

	if(graphsTable->rowCount() > 0)
	{
		graphID = graphsTable->item(graphsTable->currentRow(), 0)->text().toLongLong();

		qDebug() << "[QOSG::LoadGraphWindow::removeGraph] Selected graph ID: " << graphID;

		QMessageBox msgBox;
		msgBox.setText("Do you want to remove selected graph and all its parts from database?");
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Ok);
		int ret = msgBox.exec();

		if(ret == QMessageBox::Ok)
		{
			Model::GraphDAO::removeGraph(graphID, db->tmpGetConn());

			createGraphTable();
			this->repaint();
			this->update();
		}
	}
	else
	{
		qDebug() << "[QOSG::LoadGraphWindow::lremoveGraph] There are no graphs saved in DB.";
	}
}

QPushButton *LoadGraphWindow::createButton(const QString &text, const char *member)
{
	QPushButton *button = new QPushButton(text);
	connect(button, SIGNAL(clicked()), this, member);
	return button;
}
