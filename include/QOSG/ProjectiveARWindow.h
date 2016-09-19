#ifndef PROJECTIVE_AR_WINDOW_H
#define PROJECTIVE_AR_WINDOW_H

#include <QDialog>

#include <QLabel>
//#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QToolBar>
#include <QToolBox>

namespace OpenCV {
class CapVideo;
}

namespace QOSG {

class ViewerQT;
class ProjectiveARViewer;
}

namespace QOSG {

/**
    *  \class ProjectiveARWindow
    *  \brief Window that displays view for protected augmented reality. It also contains scene settings.
    *  \author Viktor Košťan
    *  \date 25.04.2016
    */
class ProjectiveARWindow : public QDialog
{
	Q_OBJECT

public:
	/**
	 * @author Autor: Viktor Košťan
	 * @brief ProjectiveARWindow - Constructor
	 * @param parent - Parent Widget
	 * @param sourceViewer - Viewer that views desired image.
	 * @param mApp - QApplication
	 */
	ProjectiveARWindow( QWidget* parent, QApplication* mApp );

	/**
	 * @author Autor: Viktor Košťan
	 * @brief toggleToolBars - Constructor
	 * @param parent - Parent Widget
	 * @param sourceViewer - Viewer that views desired image.
	 * @param mApp - QApplication
	 */
	void toggleToolBars();

	ProjectiveARViewer* getViewerWidget()
	{
		return viewerWidget;
	}

	void setViewerPos( double x, double y, double z );

public slots:
	// functions that update Viewer's parameters
	void updateProjectorPos();
	void updateProjectorDir();
	void updateProjectorFOV();

	void updateViewerPos();
	void updateViewerDir();
	void updateViewerFOV();

	void updateGraphPos();
	void updateGraphRadius();
	void updateUseGraphParams();


private:
	/**
	    *  QOSG::ProjectiveARViewer * viewerWidget
	    *  \brief Ponter to viewer widget
	    */
	ProjectiveARViewer* viewerWidget;
	QApplication*	app;

	/**
	    *  QToolBar * toolBar
	    *  \brief Pointer to toolbar
	    */
	QToolBar* toolBar;

	/**
	    *  ToolBox * toolBox
	    *  \brief Pointer to toolbox
	    */
	QToolBox* toolBox;

	QDoubleSpinBox* dsb_projectorFOV;
	QDoubleSpinBox* dsb_projectorPosX;
	QDoubleSpinBox* dsb_projectorPosY;
	QDoubleSpinBox* dsb_projectorPosZ;
	QDoubleSpinBox* dsb_projectorDirX;
	QDoubleSpinBox* dsb_projectorDirY;
	QDoubleSpinBox* dsb_projectorDirZ;

	QDoubleSpinBox* dsb_viewerFOV;
	QDoubleSpinBox* dsb_viewerPosX;
	QDoubleSpinBox* dsb_viewerPosY;
	QDoubleSpinBox* dsb_viewerPosZ;
	QDoubleSpinBox* dsb_viewerDirX;
	QDoubleSpinBox* dsb_viewerDirY;
	QDoubleSpinBox* dsb_viewerDirZ;

	QDoubleSpinBox* dsb_graphPosX;
	QDoubleSpinBox* dsb_graphPosY;
	QDoubleSpinBox* dsb_graphPosZ;

	QDoubleSpinBox* dsb_graphRadius;

	QCheckBox* chb_useGraphParams;

	/**
	    *  \fn private  createLeftToolBar
	    *  \brief Creates left toolBar for ProjectiveARWindow
	    */
	void createLeftToolBar();

	/**
	 * @author Viktor Košťan
	 * @brief createSceneTab add elements to QWidget for scene configuration
	 * @param line pointer to add line
	 * @return QWidget for graph functionality
	 */
	QWidget* createSceneTab( QFrame* line );

	QDoubleSpinBox* createCoordSpinBox( double value, const QString& text );

	/**
	    *  \fn private  createHorizontalFrame
	    *  \brief Crate frame with horizontal label
	    *  \return QFrame * created frame
	    */
	QFrame* createHorizontalFrame();

	/**
	    *  \fn private  createLine
	    *  \brief Creates horizontal line
	    *  \return QFrame * created line
	    */
	QFrame* createLine();


	/**
	     * @author Autor: Michael Garaj
	     * @brief closeEvent Reimplement close event. Disconnect all conections
	     * @param event Close event
	     */
	void closeEvent( QCloseEvent* event );


};

}

#endif //PROJECTIVE_AR_WINDOW_H
