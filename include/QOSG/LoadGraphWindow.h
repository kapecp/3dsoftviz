/**
*  LoadGraphWindow.h
*  Projekt 3DVisual
*
*  \author Tomas Hurban 
*
*  \date 19. 11. 2010
*/
#ifndef QOSG_LOAD_GRAPH_WINDOW_H
#define QOSG_LOAD_GRAPH_WINDOW_H 1

#include <QDialog>
#include <QDir>
#include <QtGui>
#include <QMap>
#include <QtSql>
#include <QSqlQueryModel>

#include "Data/Graph.h"
#include "Manager/Manager.h"
#include "Model/DB.h"
#include "Model/GraphDAO.h"

class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;

namespace QOSG
{
	class LoadGraphWindow : public QDialog
	{
		Q_OBJECT

	public:
		LoadGraphWindow(QWidget *parent);

	private:
		QPushButton *createButton(const QString &text, const char *member);
		void createGraphTable();

		QPushButton *loadButton;
		QLabel *numberOfGraphs;
		QStringList graphList;
		QTableWidget *graphsTable;

		private slots:
			void loadGraph();
				
	};
}

#endif
