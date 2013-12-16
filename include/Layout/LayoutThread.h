#ifndef LAYOUT_LAYOUT_THREAD_DEF
#define LAYOUT_LAYOUT_THREAD_DEF 1

#include <QThread>
#include "Layout/FRAlgorithm.h"

namespace Layout
{
/*!
	 * \brief
	 * Spusti generator layoutu v samostatnom threade.
	 * \author
	 * Michal Paprcka, Michal Pavlik
	 * \version
	 * 1.0
	 * \date
	 * 1.12.2009
	 */
class LayoutThread : public QThread
{
public:
	/**
		 * fn public constructor LayoutThread
		 * \brief Constructor of thread for layout algorithm.
		 *
		 */
	LayoutThread(Layout::FRAlgorithm* alg);

	/**
		 *
		 * fn public destructor ~LayoutThread
		 * \brief Destructor
		 *
		 */
	~LayoutThread(void);

	/**
		*  \fn public  pause
		*  \brief Pause layout algorithm
		*/
	void pause();

	/**
		*  \fn public  play
		*  \brief Play paused layout algorithm
		*/
	void play();

	/**
		*  \fn public  wakeUp
		*  \brief Wakes up frozen layout algorithm
		*/
	void wakeUp();

	/**
		*  \fn public  setAlphaValue(float val)
		*  \brief Sets multiplicity of forces
		*  \param      val  multipliciter of forces
		*/
	void setAlphaValue(float val);

	/**
		*  \fn public  isRunning
		*  \brief Returns if layout algorithm is running or not
		*  \return bool true, if algorithm is running
		*/
	bool isRunning();

	void requestEnd();

protected:

	/**
		*  \fn protected virtual  run
		*  \brief Starts layout algorithm process
		*/
	void run();


private:

	/**
		*  Layout::FRAlgorithm * alg
		*  \brief Object of Fruchterman-Reingold algorithm
		*/
	Layout::FRAlgorithm* alg;

	/**
		*  Util::ApplicationConfig * appConf
		*  \brief Object of application config
		*/
	Util::ApplicationConfig* appConf;
};
}

#endif
