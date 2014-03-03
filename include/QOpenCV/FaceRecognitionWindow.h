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
    FaceRecognitionWindow(QWidget *parent, QApplication *mApp, QOpenCV::FaceRecognitionThread *thr);
	QLabel *getLabel();
	QLabel *windowLabel;
signals:
	/**
		 * @author Autor: Marek Jakab
		 * @brief cancelLoop Signal to cancel thread
		 */
	void cancelLoop(bool set);

private:
    QApplication * mApp;
    QPushButton *mPauseButton;
    QPushButton *mCancelButton;
    //radio buttons for switching the camera
    QRadioButton *mFaceRadioButton;
    QRadioButton *mMarkerRadioButton;
    //check buttons for switching the threads on and off
    QCheckBox *mFaceCheckbox;
    QCheckBox *mMarkerCheckbox;
    QImage mqimage;
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

    /**
            * @author Autor: Martina Tregerova
            * @brief switchImage Switches the image shown between marker detection and face recognition
            */
    void switchImage();

    /**
            * @author Autor:Martina Tregerova
            * @brief startThread Starts threads based on the checked box
            */

    void startThread();

    /**
            * @author Autor:Martina Tregerova
            * @brief startThread Stops threads based on the checked box
            */

    void stopThread();
};
}

#endif //FACERECOGNITIONWINDOW_H
