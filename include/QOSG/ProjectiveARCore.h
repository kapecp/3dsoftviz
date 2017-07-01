#ifndef PROJECTIVE_AR_CORE_H
#define PROJECTIVE_AR_CORE_H

#include <QPointer>


class QApplication;

namespace QOSG {
class ProjectiveARWindow;
class ProjectiveARViewer;
}

namespace QOSG {
/**
	*@brief Class ProjectiveARCore Controller for projective AR functionality.
	*@author Autor: Viktor Košťan
	*@date 3.5.2016
	*/
class ProjectiveARCore
{

public:

	/**
		 * @author Autor: Viktor Košťan
		 * @brief getInstance Return instance of ProjectiveARCore class
		 * @param app QApplication
		 * @return ProjectiveARCore instance
		 */
	static ProjectiveARCore* getInstance( QApplication* app, QWidget* parent );

	/**
		 * @brief ~ProjectiveARCore Destructor
		 */
	~ProjectiveARCore( void );

	/**
		 * @author Autor: Viktor Košťan
		 * @brief init Creates and shows ProjectiveARWindow
		 */
	void init();

	QOSG::ProjectiveARViewer* getViewer()
	{
		return mProjectiveARViewer;
	}
	QOSG::ProjectiveARWindow* getWindow()
	{
		return mProjectiveARWindow;
	}

private:
	/**
		 * @author Autor: Viktor Košťan
		 * @brief ProjectiveARCore Costructor
		 * @param app QApplication
		 */
	ProjectiveARCore( QApplication* app, QWidget* parent );

	static ProjectiveARCore*	mProjectiveARCore;	// static singleton instance

	QWidget*                                mParent;
	QApplication*                           mApp;
	QOSG::ProjectiveARWindow*   mProjectiveARWindow;
	QOSG::ProjectiveARViewer*   mProjectiveARViewer;

};
}

#endif // PROJECTIVE_AR_CORE_H
