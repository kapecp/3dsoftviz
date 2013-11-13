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
    class FaceRecognitionWindow : public QDialog
	{
		Q_OBJECT

	public:

        FaceRecognitionWindow(QWidget *parent, QApplication *app, QOpenCV::FaceRecognitionThread *thr);
        QLabel *getLabel();
        QLabel *windowLabel;
    signals:
        void cancelLoop();

	private:
        QApplication * app;
        QPushButton *pauseButton;
        QImage qimage;
        FaceRecognitionThread *thr;

        QFrame createFrameLayout();
        void configureWindow();

		private slots:
            void pauseWindow();
            void quitWindow();
            void setLabel(cv::Mat);
	};
}

#endif //FACERECOGNITIONWINDOW_H
