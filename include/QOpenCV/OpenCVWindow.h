#ifndef OPENCVWINDOW_H
#define OPENCVWINDOW_H

#include <QDialog>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class QLabel;
class QPushButton;
class QRadioButton;
class QCheckBox;
class QStackedLayout;
class QSlider;

namespace OpenCV {
class CapVideo;
}

namespace QOpenCV {

class OpenCVWindow : public QDialog
{
    Q_OBJECT

public:
    /**
     * @author Autor: Michael Garaj
     * @brief OpenCVWindow Constructor
     * @param parent Parent Widget
     * @param mApp QApplication
     */
    OpenCVWindow(QWidget *parent, QApplication *mApp);

    /**
         * @author Autor: Michael Garaj
         * @brief getLabel Return mWindowLabel
         * @return QLabel*
         */
    QLabel		*getLabel() const;

signals:
    /**
         * @author Autor: Michael Garaj
         * @brief startMultiMarker Signal for starting Aruco thread
         */
    void startMultiMarker();

    /**
         * @author Autor: Michael Garaj
         * @brief stopMultiMarker Signal for canceling Aruco thread
         * @param set cancel if true
         */
    void stopMultiMarker( bool set );

    /**
         * @author Autor: Michael Garaj
         * @brief setMultiMarker Signal for setting MultiMarker in Aruco thread
         * @param set cancel if true
         */
    void setMultiMarker( bool set );

    /**
         * @author Autor: Michael Garaj
         * @brief setCapVideoFaceRec Signal for setting camera in ArucoThread
         * @param capVideo camera object
         */
    void setCapVideoMarker( OpenCV::CapVideo *capVideo );

public slots:
    /**
        * @author Autor: Michael Garaj
        * @brief setLabel Draw image on label
        */
    void setLabel( cv::Mat image );

private slots:
    /**
         * @author Autor: Michael Garaj
         * @brief onSelModulChange Switch between Aruco and Kinect layout
         */
    void onSelModulChange();

    /**
         * @author Autor: Michael Garaj
         * @brief onMultiMarkerStartCancel Start or stop Aruco thread
         * @param checked If true, thread will start
         */
    void onMultiMarkerStartCancel(bool checked);

private:
    /**
         * @author Autor: Michael Garaj
         * @brief closeEvent Reimplement close event. Disconnect all conections
         * @param event Close event
         */
    void closeEvent(QCloseEvent *event);

    /**
         * @author Autor: Michael Garaj
         * @brief configureWindow Create dialog's gui,( all layouts, widgets and connection inside class)
         */
    void configureWindow();

    QApplication	*mApp;
    QLabel			*mWindowLabel;

    QRadioButton	*mKinectRB;
    QRadioButton	*mArucoRB;

    QRadioButton    *mFaceRecRB;
    QRadioButton    *mMarkerRB;
    QRadioButton    *mMultiMarkerRB;

    QPushButton		*mKinectPB;
    QPushButton		*mArucoPB;

    QStackedLayout  *mModulesStackL;
    QStackedLayout  *mSubmodulesStackL;

    QPushButton     *mMultiMarkerStartCancelPB;
};

}

#endif //OPENCVWINDOW_H
