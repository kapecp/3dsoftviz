#include "QOpenCV/CamSelectWindow.h"
#include <QVBoxLayout>

using namespace QOpenCVCam;

QOpenCVCam::CamSelectWindow::CamSelectWindow(QWidget *parent, QApplication * app, QString stringdata): QDialog(parent)
{
	this->app = app;
	configureWindow(stringdata);
}

void QOpenCVCam::CamSelectWindow::configureWindow(QString stringdata)
{
	setModal(true);
	resize(400,260);
	setWindowTitle(tr("Select camera to use"));

	changes = new QList<QString>();

	view = new QTreeView();
	view->setFocusPolicy(Qt::NoFocus);
	view->setVisible(false);

	applyButton= new QPushButton(tr("Apply"));
	applyButton->setFocusPolicy(Qt::NoFocus);
	//applyButton->setEnabled(false);
	connect(applyButton, SIGNAL(clicked()), this, SLOT(commitChanges()));

	QHBoxLayout *horizontalLayout = new QHBoxLayout;
	horizontalLayout->addWidget(view);

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(applyButton);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(horizontalLayout);
	mainLayout->addStretch(1);
	mainLayout->addSpacing(12);
	mainLayout->addLayout(buttonsLayout);
	createTableModel(stringdata);
	setLayout(mainLayout);
}

void CamSelectWindow::createTableModel(QString data)
{
	//vytvorenie polozky tabulky
	model = new QStandardItemModel(0, 4);
	model->setHeaderData(0, Qt::Horizontal, QObject::tr("Device ID"));
	model->setHeaderData(1, Qt::Horizontal, QObject::tr("In use"));
	model->setHeaderData(2, Qt::Horizontal, QObject::tr("Width"));
	model->setHeaderData(3, Qt::Horizontal, QObject::tr("Height"));

	QList<QString> rows = data.split(";");
	//QButtonGroup group;
	for (int i=0; i< rows.length();i++)
	{
		QList<QString> row = rows[i].split(",");
		//group.addButton(model->item(i));
		model->insertRow(i);
		model->setData(model->index(i, 0), row[0]);
		model->setData(model->index(i, 1), row[1]);
		model->setData(model->index(i, 2), row[2]);
		model->setData(model->index(i, 3), row[3]);
		//model->item(i,0)->setFlags(Qt::ItemIsUserCheckable);
		model->item(i,1)->setFlags(Qt::NoItemFlags);
		model->item(i,0)->setCheckable(true);
		if (row[2].compare("0")!=0) {
			model->item(i,2)->setFlags(Qt::NoItemFlags);
			model->item(i,3)->setFlags(Qt::NoItemFlags);
		}
	}
	view->setVisible(true);
	view->setModel(model);
}

void CamSelectWindow::commitChanges()
{

	OpenCVCam::CamSelectCore::getInstance(app)->setCam(1,320,240);
}


