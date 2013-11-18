#ifndef FACERECOGNITIONWINDOW_H
#define FACERECOGNITIONWINDOW_H

#include <QDialog>

#include "QOpenCV/FaceRecognitionThread.h"

class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;

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
        void cancelLoop();

	private:
        QApplication * app;
        QPushButton *pauseButton;
        QImage qimage;
        FaceRecognitionThread *thr;

        /**
         * @author Autor: Marek Jakab
         * @brief configureWindow Sets window properties
         */
        void configureWindow();
        void FaceRecognitionWindow::closeEvent(QCloseEvent *event);
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
