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
#include "QOSG/SelectLayoutWindow.h"

class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;

namespace QOSG
{
	/**
	*  \class LoadGraphWindow
	*
	*  \brief Window for loading graphs saved in database
	*/
	class LoadGraphWindow : public QDialog
	{
		Q_OBJECT

	public:
		/**
		*  \fn LoadGraphWindow(QWidget *parent)
		*  \brief Constructor
		*
		*  \param  parent	Parent window
		*/
		LoadGraphWindow(QWidget *parent);

	private:
		/**
		*  \fn *createButton(const QString &text, const char *member)
		*  \brief	Create button with defined parameters
		*
		*  \param   text     Button name 
		*  \param   member   Member
		*/
		QPushButton *createButton(const QString &text, const char *member);

		/**
		*  \fn		createGraphTable()
		*  \brief	Create and fill table with data about number of graphs, nodes and edges stored in database
		*/
		void createGraphTable();

		/**
		*  QPushButton *loadButton
		*  \brief Load button
		*/
		QPushButton *loadButton;

		/**
		*  QLabel *numberOfGraphs
		*  \brief Label which show number of graphs stored in database
		*/
		QLabel *numberOfGraphs;

		/**
		*  QStringList graphList
		*  \brief List of graphs stored in database
		*/
		QStringList graphList;

		/**
		*  QTableWidget *graphsTable
		*  \brief Table where are shown data about graphs stored in database
		*/
		QTableWidget *graphsTable;

		private slots:
			/**
			*  \fn loadGraph()
			*  \brief	Slot which is called when user click on load button
			*/
			void loadGraph();
				
	};
}

#endif
