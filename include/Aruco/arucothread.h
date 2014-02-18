#ifndef ARUCOTHREAD_H
#define ARUCOTHREAD_H

#include <QThread>


namespace ArucoModul {

/**
* @brief Class ArucoThread
* @author Dávid Durčák
* @date 18.02.2014
*/
class ArucoThread : public QThread
	{
	Q_OBJECT

public:
	ArucoThread(void);
	~ArucoThread(void);

	/**
	* @brief run Starts thread
	*@author Dávid Durčák
	*/
	void run();


private slots:
	/**
	* @author Dávid Durčák
	* @brief setCancel sets cancel=true
	* @param set Cancel thread if set == true
	*/
	void setCancel(bool set);

	/**
	* @author Dávid Durčák
	* @brief pauses recognition window
	*/
	void pause();

private:
	bool cancel;

	};
} // ArucoModul namespace
#endif //ARUCOTHREAD_H
