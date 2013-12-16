/*!
 * Core.h
 * Projekt 3DVisual
 */
#ifndef Core_CORE_DEF
#define Core_CORE_DEF 1

#include <QMap>
#include <QString>
#include <QApplication>

#include "Model/DB.h"
#include "Data/Graph.h"
#include "Layout/FRAlgorithm.h"
#include "Layout/LayoutThread.h"
#include "QOSG/CoreWindow.h"
#include "Viewer/CoreGraph.h"
#include "QOSG/MessageWindows.h"

namespace QOSG
{
class CoreWindow;
}

namespace AppCore
{

/**
	 * \class Core
	 * \brief Core control whole application
	 *
	 * Class is implemented as singleton. Core provides functionality to work with layout threads, application windows and core graph.
	 *
	 * \author Pavol Perdik
	 * \date 8.5.2010
	 */
class Core
{
public:
	~Core();

	/**
		 * \fn restartLayout
		 * \brief Restarts layouting of graph.
		 */
	void restartLayout();

	/**
		 * \fn getLayoutThread
		 * \brief Returns Layout::LayoutThread
		 */
	Layout::LayoutThread* getLayoutThread(){return this->thr;}

	/**
		 * \fn getInstance
		 * \brief Returns instance.
		 */
	static Core* getInstance(QApplication * app = 0);

	/**
		 * QOSG::MessageWindows * messageWindows;
		 * \brief Provides functionality to show information or error popup windows and progress bar.
		 */
	QOSG::MessageWindows * messageWindows;

	/**
		 * Layout::LayoutThread * thr
		 * \brief Thread of Layout algorithm.
		 */
	Layout::LayoutThread * thr;

	/**
		  * \fn getCoreWindow
		  * \brief Returns QOSG::CoreWindow
		  **/
	QOSG::CoreWindow* getCoreWindow(){return this->cw;}
private:
	/**
		*  QOSG::CoreWindow * cw
		*  \brief instance of CoreWindow
		*/
	QOSG::CoreWindow * cw;

	/**
		*  \fn private constructor  GraphManager(QApplication * app)
		*  \brief
		*  \param app
		*/
	Core(QApplication * app);

	/**
		*  Layout::FRAlgorithm * alg
		*  \brief instance of layout algorithm.
		*/
	Layout::FRAlgorithm * alg;


	/**
		*  Vwr::CoreGraph * cg
		*  \brief instance of CoreGraph
		*/
	Vwr::CoreGraph * cg;

	/**
		*  Core * core;
		*  \brief instance of this class
		*/
	static Core * core;
};
}

#endif
