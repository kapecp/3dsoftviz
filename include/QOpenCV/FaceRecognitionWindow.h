#ifndef FACERECOGNITIONWINDOW_H
#define FACERECOGNITIONWINDOW_H

#include <QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QComboBox>
#include <QtGui/QTableWidget>
#include <QtGui/QTableWidgetItem>

#include "QOpenCV/FaceRecognitionThread.h"

#include "QOpenCV/FaceRecognitionWindow.h"
#include "OpenCV/FaceRecognizer.h"
#include "OpenCV/CapVideo.h"

namespace QOpenCV
{
class FaceRecognitionThread;

/**
	 * @author Autor: Marek Jakab
	 * @brief Class FaceRecognitionWindow
	 */
class FaceRecognitionWindow : public QDialog
{
	Q_OBJECT

public:
	/**
		 * @author Autor: Marek Jakab
		 * @brief FaceRecognitionWindow Constructor
		 * @param parent Parent Widget
		 * @param app QApplication
		 * @param thr Instance of thread
		 */
	FaceRecognitionWindow(QWidget *parent, QApplication *app, QOpenCV::FaceRecognitionThread *thr);
	QLabel *getLabel();
	QLabel *windowLabel;
signals:
	/**
		 * @author Autor: Marek Jakab
		 * @brief cancelLoop Signal to cancel thread
		 */
	void cancelLoop(bool set);

private:
	QApplication * app;
	QPushButton *pauseButton;
	QPushButton *cancelButton;
	QImage qimage;
	FaceRecognitionThread *thr;

	/**
		 * @author Autor: Marek Jakab
		 * @brief configureWindow Sets window properties
		 */
	void configureWindow();
	void closeEvent(QCloseEvent *event);
private slots:
	/**
			 * @author Autor: Marek Jakab
			 * @brief pauseWindow Cancel run
			 */
	void pauseWindow();

	/**
			 * @author Autor: Marek Jakab
			 * @brief quitWindow Cancel thread and call destructor
			 */
	void quitWindow();

	/**
			 * @author Autor: Marek Jakab
			 * @brief setLabel Draw image on label
			 */
	void setLabel(cv::Mat);
};
}

#endif //FACERECOGNITIONWINDOW_H
