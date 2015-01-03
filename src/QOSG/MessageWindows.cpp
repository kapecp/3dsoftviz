#include  "QOSG/MessageWindows.h"

namespace QOSG {

MessageWindows::MessageWindows(void)
{
	progressBar = NULL;
	loadingDialog = NULL;
}

MessageWindows::~MessageWindows(void)
{
}

void MessageWindows::showProgressBar()
{
	if (progressBar == NULL)
	{
		progressBar = new QProgressDialog("Loading","",0,100,NULL,Qt::Dialog);
		progressBar->setModal(true);
		progressBar->setCancelButton(NULL);
	}
	progressBar->setValue(0);
	progressBar->show();
}

void MessageWindows::closeProgressBar()
{
	if ((progressBar != NULL) &&(progressBar->isVisible()))
	{
		progressBar->hide();
	}
}


void MessageWindows::setProgressBarValue(int value)
{
	if (progressBar->isVisible())
	{
		progressBar->setValue(value);
	}
}

void MessageWindows::showMessageBox(QString title, const QString &message, bool isError)
{
	QMessageBox msgBox;
	msgBox.setText(message);
	msgBox.setWindowTitle(title);
	if (isError)
	{
		msgBox.setIcon(QMessageBox::Warning);
	}
	else
	{
		msgBox.setIcon(QMessageBox::Information);
	}
	msgBox.exec();
}


void MessageWindows::showLoadingDialog(QString message)
{
	if (loadingDialog == NULL)
	{
		loadingDialog = new QMessageBox();
	}
	loadingDialog->setText(message);
	loadingDialog->setStandardButtons(QMessageBox::NoButton);

	loadingDialog->show();
}


void MessageWindows::closeLoadingDialog()
{
	if ((loadingDialog != NULL) && (loadingDialog->isVisible()))
	{
		loadingDialog->hide();
	}
}

} // namespace QOSG
