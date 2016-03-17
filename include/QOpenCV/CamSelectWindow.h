#ifndef CAMSELECTWINDOW_H
#define CAMSELECTWINDOW_H

#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QModelIndex>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVBoxLayout>

class QApplication;
class QLabel;
class QStandardItemModel;
class QTreeView;
class QPushButton;


namespace OpenCV {
/**
	 * @author Autor: Marek Jakab
	 * @brief Class CamSelectWindow
	 */
class CamSelectWindow : public QDialog
{
	Q_OBJECT

public:
	/**
		 * @author Autor: Marek Jakab
		 * @brief CamSelectWindow Constructor
		 * @param parent Parent Widget
		 * @param app QApplication
		 */
	CamSelectWindow( QWidget* parent, QApplication* app, QString stringdata );

signals:
	/**
	 * @author Autor: Marek Jakab
	 * @brief setCam Signal for selecting Camera by ID
	 * @param devid Camera ID
	 * @param width Selected width of Camera
	 * @param heigth Selected height of Camera
	 */
	void setCam( int devid, int width, int heigth );

public slots:
	/**
	 * @brief commitChanges Commit the selection of camera
	 */
	void commitChanges();

private:
	/**
		 * @author Autor: Marek Jakab
		 * @brief configureWindow Sets window properties
		 */
	void configureWindow( QString stringdata );
	/**
		 * @author Autor: Marek Jakab
		 * @brief createTableModel Create gui model and fill data
		 * @param stringdata Data from cameras
		 */
	void createTableModel( QString stringdata );

	QApplication* app;

	/**
	 * @brief model Model used for QT gui
	 */
	QStandardItemModel* model;
	/**
	 * @brief view Tree view used in gui model
	 */
	QTreeView* view;
	/**
	 * @brief applyButton Confirmation button
	 */
	QPushButton* applyButton;

};
}

#endif // CAMSELECTWINDOW_H
