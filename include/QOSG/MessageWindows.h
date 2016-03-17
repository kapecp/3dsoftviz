/**
*  MessageWindows.h
*  Projekt 3DVisual
*/
#ifndef QOSG_MESSAGE_WINDOWS
#define QOSG_MESSAGE_WINDOWS 1

#include <QtGui>

#include <QString>

namespace QOSG {

/**
	*  \class MessageWindows
	*  \brief Class that show user progress bar, error, message and loading dialog
	*  \author Adam Pazitnaj
	*  \date 3. 5. 2010
	*/
class MessageWindows
{
public:
	/**
		*  \fn public constructor  MessageWindows
		*  \brief Constructor
		*/
	MessageWindows( void );

	/**
		*  \fn public destructor  ~MessageWindows
		*  \brief Destructor
		*/
	~MessageWindows( void );

	/**
		*  \fn public  showProgressBar
		*  \brief Show user the progress bar
		*/
	void showProgressBar();

	/**
		*  \fn public  closeProgressBar
		*  \brief Close the progress bar
		*/
	void closeProgressBar();

	/**
		*  \fn public  setProgressBarValue(int value)
		*  \brief Set the actual value for progress bar
		*  \param  value
		*/
	void setProgressBarValue( int value );

	/**
		*  \fn public  showMessageBox(QString title, QString message, bool isError)
		*  \brief Show user error or message dialog
		*  \param  title   title of the dialog
		*  \param  message message of the dialog
		*  \param  isError flag if the dialog is message or error
		*/
	void showMessageBox( QString title, const QString& message, bool isError );

	/**
		*  \fn public  showLoadingDialog(QString message)
		*  \brief Show user loading dialog
		*  \param message QString    message of the dialog
		*/
	void showLoadingDialog( QString message );

	/**
		*  \fn public  closeLoadingDialog
		*  \brief Close the loading dialog
		*/
	void closeLoadingDialog();

private:
	/**
		*  QProgressDialog * progressBar
		*  \brief  Poninter to progress bar
		*/
	QProgressDialog* progressBar;

	/**
		*  QMessageBox * loadingDialog
		*  \brief Pointer to loading dialog
		*/
	QMessageBox* loadingDialog;

};
}

#endif
