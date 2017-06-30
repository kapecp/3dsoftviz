/**
*  OptionsWindow.h
*  Projekt 3DVisual
*
*  \author Adam Pazitnaj
*
*  \date 27. 4. 2010
*/
#ifndef QOSG_OPTIONS_WINDOW_H
#define QOSG_OPTIONS_WINDOW_H 1

#include <QDialog>

#include <QModelIndex>
#include <QtGui>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QTreeView;

namespace Vwr {
class CoreGraph;
}

namespace QOSG {
class TreeItem;
class TreeModel;
class ViewerQT;

/**
	*  \class OptionsWindow
	*
	*  \brief Window in which options of the graph can be changed.
	*
	*  \author Adam Pazitnaj
	*
	*  \date 27. 4. 2010
	*/
class OptionsWindow : public QDialog
{
	Q_OBJECT

public:
	/**
		*  \fn OptionsWindow(Vwr::CoreGraph *cg, QOSG::ViewerQT * viewer)
		*  \brief Constructor
		*
		*  \param  cg	CoreGraph used for reloading of atributes
		*  \param  viewer    ViewerQT used for reloading of atributes
		*/
	OptionsWindow( Vwr::CoreGraph* cg, QOSG::ViewerQT* viewer );

public slots:
	/**
			*  \fn itemClicked(QModelIndex index)
			*  \brief	Slot which is called when user click on group of atributes
			*
			*  \param   index     Index of the group of atributes
			*/
	void itemClicked( QModelIndex index );

	/**
			*  \fn commitChanges
			*  \brief  Slot function which is called when the button Apply is clicked
			*/
	void commitChanges();

private:

	/**
		*  QTreeView * view
		*  \brief Tree view of the groups of atributes
		*/
	QTreeView* view;

	/**
		*  QOSG::TreeModel * treeModel
		*  \brief Model for the tree view
		*/
	QOSG::TreeModel* treeModel;

	/**
		*  QStandardItemModel * model
		*  \brief Model for the view of atributes of group in the right part of window
		*/
	QStandardItemModel* model;

	/**
		*  QList<QString> * changes
		*  \brief List of changes which are made during the editation of atributes
		*/
	QList<QString>* changes;

	/**
		*  QOSG::TreeItem * selectedItem
		*  \brief Pointer to the selected group of atributes in tree view
		*/
	QOSG::TreeItem* selectedItem;

	/**
		*  \fn createTableModel(QString data)
		*  \brief Function for creating data model for view of atributes of group
		*
		*  \param    data 	Atributes of the group in QString
		*/
	void createTableModel( QString data );

	/**
		*  \fn getModelData
		*  \brief	Function that return QString representation of group of atributes
		*
		*  \return QString Representation of group of atributes
		*/
	QString getModelData();

	/**
		*  \fn treeSearch(TreeItem * index, QString path)
		*  \brief Function that recurently search over the tree model and commint the changes
		*
		*  \param index    Actual index in tree model
		*  \param path     Path to the group of atributes
		*/
	void treeSearch( TreeItem* index, QString path );
	/**
		*  \fn applyChanges(QString path, QString data)
		*  \brief Function that commit the changes to the ApplicationConfig for actual group of atributes
		*
		*  \param  path     Path of the group of atributes
		*  \param  data     Atributes with their values
		*/
	void applyChanges( QString path, QString data );

	/**
		*  QPushButton * applyButton
		*  \brief Apply button
		*/
	QPushButton* applyButton;

	/**
		*  \fn saveNodeTypes(TreeItem * index)
		*  \brief Function that commit the changes made in node types
		*
		*  \param  index   Root index in the tree model of node types
		*/
	void saveNodeTypes( TreeItem* index );

	/**
		*  Vwr::CoreGraph * cg
		*  \brief Pointer to the CoreGraph for reloading of changes
		*/
	Vwr::CoreGraph* cg;
	/**
		*  QOSG::ViewerQT * viewer
		*  \brief Pointer to ViewerQT for reloading of changes
		*/
	QOSG::ViewerQT* viewer;

};
}

#endif
