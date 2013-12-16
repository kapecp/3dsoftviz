/**
*  Cleaner.h
*  Projekt 3DVisual
*
*  \author Adam Pazitnaj
*
*  \date 27. 4. 2010
*/
#ifndef UTIL_CLEANER_DEF
#define UTIL_CLEANER_DEF 1

#include "QOSG/CoreWindow.h"


/**
*  \class MessageWindows
*
*  \brief Class for cleaning the memory before application atempts to quit
*
*  \author Adam Pazitnaj
*
*  \date 27. 4. 2010
*/
class Cleaner : public QObject
{
	Q_OBJECT


public slots:

	/**
			*  \fn clean
			*
			*  \brief Slot function that is called befoure application atempts to quit
			*
			*  \author Adam Pazitnaj
			*
			*  \date 27. 4. 2010
			*/
	void clean();

public:

	/**
	*  \fn Cleaner(QApplication * app)
	*
	*  \brief Constructor
	*
	*  \param  app   Pointer to QApplication for cleaning before quit
	*
	*  \author Adam Pazitnaj
	*
	*  \date 27. 4. 2010
	*/
	Cleaner(QApplication * app);


};

#endif
