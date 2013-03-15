#include "QOSG/SelectLayoutWindow.h"

using namespace QOSG;

qlonglong graphID;

SelectLayoutWindow::SelectLayoutWindow(QWidget *parent, qlonglong graphID)
{
	this->graphID = graphID;

	setModal(true);
	resize(600,250);	 
    setWindowTitle(tr("Select layout for graph no. %1").arg(graphID));

	loadButton = createButton(tr("Load"), SLOT(loadLayout()));
	removeButton = createButton(tr("Remove"), SLOT(removeLayout()));

	QPushButton *cancelButton = new QPushButton(tr("Cancel"));
	cancelButton->setFocusPolicy(Qt::NoFocus);
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	numberOfLayouts = new QLabel;

	layoutList << tr("ID") << tr("Name");
	layoutsTable = new QTableWidget(this);
	layoutsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	layoutsTable->setRowCount(0);
	layoutsTable->setColumnCount(2);
	layoutsTable->setHorizontalHeaderLabels(layoutList);
	layoutsTable->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
	layoutsTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
	layoutsTable->verticalHeader()->hide();
    layoutsTable->setShowGrid(true);

	createLayoutTable();

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(loadButton);
	buttonsLayout->addWidget(removeButton);
	buttonsLayout->addWidget(cancelButton);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(layoutsTable, 3, 0, 1, 3);
	mainLayout->addWidget(numberOfLayouts, 4, 0, 1, 3);
    mainLayout->addLayout(buttonsLayout, 5, 0, 1, 3);
    setLayout(mainLayout);
}

void SelectLayoutWindow::createLayoutTable()
{
	Manager::GraphManager * manager = Manager::GraphManager::getInstance();
	Model::DB * db = manager->getDB();
	bool error = false;
	qlonglong layoutsCount, row; 
	QString name;
	QMap<qlonglong, QString>::iterator iterLayout;
	qlonglong layoutID;
	
	QMap<qlonglong, QString> layouts = Model::GraphLayoutDAO::getLayoutsNames(graphID, db->tmpGetConn(), &error);
	layoutsCount = layouts.count(); 

	numberOfLayouts->setText(tr("%1 layout(s) found").arg(layoutsCount));
	layoutsTable->setRowCount(layoutsCount);

	for(iterLayout = layouts.begin(), row=0; iterLayout != layouts.end(); ++iterLayout, row++)
	{
		layoutID = iterLayout.key();
		
		name = layouts.value(layoutID);

		QTableWidgetItem *itemID = new QTableWidgetItem(tr("%1").arg(layoutID));
		QTableWidgetItem *itemName = new QTableWidgetItem(name);
		layoutsTable->setItem(row, 0, itemID);
		layoutsTable->setItem(row, 1, itemName);
	}
	if(layoutsTable->rowCount() > 0)
		layoutsTable->selectRow(0);
}

void SelectLayoutWindow::loadLayout()
{
	qlonglong layoutID;

	if(layoutsTable->rowCount() > 0) 
	{
		layoutID = layoutsTable->item(layoutsTable->currentRow(), 0)->text().toLongLong(); 

		Manager::GraphManager * manager = Manager::GraphManager::getInstance();
		Model::DB * db = manager->getDB();
		bool error = false;

		qDebug() << "[QOSG::SelectLayoutWindow::loadLayout] Selected layout ID: " << layoutID;

		manager->loadGraphFromDB(graphID, layoutID);

		this->close();
	}
	else 
	{
		qDebug() << "[QOSG::SelectLayoutWindow::loadLayout] There are no layouts for graph saved in DB.";
	}
}

void SelectLayoutWindow::removeLayout()
{
	qlonglong layoutID;
	Manager::GraphManager * manager = Manager::GraphManager::getInstance();
	Model::DB * db = manager->getDB();

	if(layoutsTable->rowCount() > 0) 
	{
		layoutID = layoutsTable->item(layoutsTable->currentRow(), 0)->text().toLongLong(); 

		qDebug() << "[QOSG::SelectLayoutWindow::removeLayout] Selected layout ID: " << layoutID;

		QMessageBox msgBox;
		msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Ok);

		if(layoutsTable->rowCount() == 1)
		{
			msgBox.setText("There is only one layout in this graph!");
			msgBox.setInformativeText("Do you really want to remove this layout? (It will also remove the graph)");
			int ret = msgBox.exec();

			if(ret == QMessageBox::Ok)
			{
				Model::GraphDAO::removeGraph(graphID, db->tmpGetConn());

				createLayoutTable();
				this->repaint();
				this->update();
				qDebug() << "[QOSG::SelectLayoutWindow::removeLayout] Graph no. " << graphID << " was removed from database";
			}
		}
		else
		{
			msgBox.setText("Do you want to remove selected layout from database?");
			int ret = msgBox.exec();

			if(ret == QMessageBox::Ok)
			{
				Model::GraphLayoutDAO::removeLayout(graphID, layoutID, db->tmpGetConn());

				createLayoutTable();
				this->repaint();
				this->update();
				qDebug() << "[QOSG::SelectLayoutWindow::removeLayout] Layout no. " << layoutID << " was removed from database";
			}
		}
	}
	else 
	{
		qDebug() << "[QOSG::SelectLayoutWindow::removeLayout] There are no layouts for graph saved in DB.";
	}
}

QPushButton *SelectLayoutWindow::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}