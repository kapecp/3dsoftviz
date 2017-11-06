/**
*  LoadGraphWindow.h
*  Projekt 3DVisual
*
*  \author Tomas Hurban
*
*  \date 19. 11. 2010
*/
#ifndef QOSG_LOAD_GRAPH_WINDOW_H
#define QOSG_LOAD_GRAPH_WINDOW_H 1

#include <QtGlobal>

#ifdef __APPLE__
/* FIX COMPILE BUG:

	platform: OSX v10.11.6 (15G1611) with Qt v5.9.1 from Homebrew

	this solves the following compilation error:
	/usr/local/Cellar/qt/5.9.1/lib/QtGui.framework/Headers/qopenglversionfunctions.h:1089:23: error: unknown type name 'GLDEBUGPROC'

	somehow the GL_KHR_debug macro is set to 1 in qopengl.h, so
	#ifndef GL_KHR_debug
	typedef void (APIENTRY *GLDEBUGPROC)(GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const GLvoid *userParam);
	#endif
	are removed, causing the error "unknown type name" GLDEBUGPROC in qopenglversionfunctions.h

	possible causes:
		- some change in Qt v5.9.1 (older versions have worked, at least ~5.8 worked)
*/
typedef void ( APIENTRY* GLDEBUGPROC )( GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar* message,const GLvoid* userParam );
#endif
#include <QtGui>

#include <QDialog>
#include <QHeaderView>
#include <QMap>

#if QT_VERSION >= 0x050000
#define SETRESIZEMODE setSectionResizeMode
#else
#define SETRESIZEMODE setResizeMode
#endif

class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;

namespace QOSG {
/**
	*  \class LoadGraphWindow
	*
	*  \brief Window for loading graphs saved in database
	*/
class LoadGraphWindow : public QDialog
{
	Q_OBJECT

public:
	/**
		*  \fn LoadGraphWindow(QWidget *parent)
		*  \brief Constructor
		*
		*  \param  parent	Parent window
		*/
	explicit LoadGraphWindow( QWidget* parent );

private:
	/**
		*  \fn *createButton(const QString &text, const char *member)
		*  \brief	Create button with defined parameters
		*
		*  \param   text     Button name
		*  \param   member   Member
		*/
	QPushButton* createButton( const QString& text, const char* member );

	/**
		*  \fn		createGraphTable()
		*  \brief	Create and fill table with data about number of graphs, nodes and edges stored in database
		*/
	void createGraphTable();

	/**
		*  QPushButton *loadButton
		*  \brief Load button
		*/
	QPushButton* loadButton;

	/**
		*  QPushButton *removeButton
		*  \brief Remove button
		*/
	QPushButton* removeButton;

	/**
		*  QPushButton *renameButton
		*  \brief Rename button
		*/
	QPushButton* renameButton;

	/**
		*  QLabel *numberOfGraphs
		*  \brief Label which show number of graphs stored in database
		*/
	QLabel* numberOfGraphs;

	/**
		*  QStringList graphList
		*  \brief List of graphs stored in database
		*/
	QStringList graphList;

	/**
		*  QTableWidget *graphsTable
		*  \brief Table where are shown data about graphs stored in database
		*/
	QTableWidget* graphsTable;

private slots:
	/**
			*  \fn loadGraph()
			*  \brief	Slot which is called when user click on load button
			*/
	void loadGraph();

	/**
			*  \fn removeGraph()
			*  \brief	Slot which is called when user click on remove button
			*/
	void removeGraph();

	/**
			*  \fn renameGraph()
			*  \brief	Slot which is called when user click on rename button
			*/
	void renameGraph();
};
}

#endif
