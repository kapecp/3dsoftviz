#ifndef QOSG_SELECT_LAYOUT_WINDOW_H
#define QOSG_SELECT_LAYOUT_WINDOW_H 1

#include <QDialog>
#include <QDir>
#include <QtGui>
#include <QMap>

#if QT_VERSION >= 0x050000
	#define SETRESIZEMODE setSectionResizeMode
#else
	#define SETRESIZEMODE setResizeMode
#endif

class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;

namespace QOSG {




/**
	*  \class SelectLayoutWindow
	*
	*  \brief Window for selecting graph layout saved in database
	*/
class SelectLayoutWindow : public QDialog
{
	Q_OBJECT

public:
	/**
		*  \fn SelectLayoutWindow(QWidget *parent)
		*  \brief Constructor
		*
		*  \param  parent	Parent window
		*  \param  graphID	graph ID
		*/
	SelectLayoutWindow( QWidget* parent, qlonglong graphID );

private:
	/**
		*  \fn *createButton(const QString &text, const char *member)
		*  \brief	Create button with defined parameters
		*
		*  \param   text     Button name
		*  \param   member   Member
		*/
	QPushButton* createButton( const QString& text, const char* member );

	/**
		*  \fn		createLayoutTable()
		*  \brief	Create and fill table with data about layouts in database
		*/
	void createLayoutTable();

	/**
		*  QPushButton *loadButton
		*  \brief Load button
		*/
	QPushButton* loadButton;

	/**
		*  QPushButton *removeButton
		*  \brief Remove button
		*/
	QPushButton* removeButton;

	/**
		*  QLabel *numberOfLayouts
		*  \brief Label which show number of graph layouts stored in database
		*/
	QLabel* numberOfLayouts;

	/**
		*  QStringList layoutList
		*  \brief List of graph layouts stored in database
		*/
	QStringList layoutList;

	/**
		*  QTableWidget *layoutsTable
		*  \brief Table where are shown data about graph layouts stored in database
		*/
	QTableWidget* layoutsTable;

	/**
		*  qlonglong graphID
		*  \brief Graph ID
		*/
	qlonglong graphID;

private slots:
	/**
			*  \fn loadLayout()
			*  \brief	Slot which is called when user click on load button
			*/
	void loadLayout();

	/**
			*  \fn removeLayout()
			*  \brief	Slot which is called when user click on remove button
			*/
	void removeLayout();
};
}

#endif
